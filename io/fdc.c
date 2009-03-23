#include	"compiler.h"
#include	"z80core.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"nevent.h"
#include	"fddfile.h"
#include	"fdd_mtr.h"

enum {
	FDCCTYPE_CMD1		= 0x01,
	FDCCTYPE_CMD2		= 0x02,
	FDCCTYPE_CMD3		= 0x04,
	FDCCTYPE_CMD4		= 0x08,

	FDCCTYPE_RO			= 0x10,
	FDCCTYPE_DATA		= 0x80
};


static const UINT8 fdctype[] = {
					FDCCTYPE_CMD1,
					FDCCTYPE_CMD1,
					FDCCTYPE_CMD1,
					FDCCTYPE_CMD1,
					FDCCTYPE_CMD1,
					FDCCTYPE_CMD1,
					FDCCTYPE_CMD1,
					FDCCTYPE_CMD1,
					FDCCTYPE_CMD2 + FDCCTYPE_RO,
					FDCCTYPE_CMD2 + FDCCTYPE_RO,
					FDCCTYPE_CMD2,
					FDCCTYPE_CMD2,
					FDCCTYPE_CMD3 + FDCCTYPE_RO,
					FDCCTYPE_CMD4,
					FDCCTYPE_CMD3 + FDCCTYPE_RO,
					FDCCTYPE_CMD3};


/* write track */

#if !defined(CONST_DISKIMAGE)
enum {
	TAO_MODE_GAP	= 0x4e,
	TAO_MODE_SYNC	= 0x00,
	TAO_MODE_AM		= 0xf5,
	TAO_MODE_IM		= 0xf6,
	TAO_MODE_ID		= 0xfe,
	TAO_MODE_DATA	= 0xfb,
	TAO_ENDOFDATA	= 0xf7,

	TAO_CMD_GAP		= 0x4e,
	TAO_CMD_SYNC	= 0x00,
	TAO_CMD_IM_IN	= 0xf6,
	TAO_CMD_IM		= 0xfc,
	TAO_CMD_AM_IN	= 0xf5,
	TAO_CMD_IAM		= 0xfe,
	TAO_CMD_DAM		= 0xfb,
	TAO_CMD_DDAM	= 0xf8,
	TAO_CMD_CRC		= 0xf7
};


static REG8 wrtrkstart(FDC *f) {

	FDDFILE	fdd;

	fdd = fddfile + f->s.drv;
	if ((fdd->type == DISKTYPE_NOTREADY) || (fdd->protect)) {
		return(0);
	}
	f->s.bufdir = FDCDIR_TAO;
	f->s.bufpos = 0;
	f->s.bufmedia = f->s.media;
	f->s.bufunit = f->s.drv;
	f->s.buftrack = (f->s.c << 1) + f->s.h;

	f->s.wt_mode = TAO_ENDOFDATA;
	f->s.wt_sectors = 0;
	f->s.wt_ptr = 0;
	f->s.wt_datpos = 0;
	f->s.wt_datsize = 0;
	ZeroMemory(f->s.buffer, sizeof(f->s.buffer));

	return(0);
}

static void wrtrkdata(FDC *f, REG8 data) {

	TAOSEC	*t;
	REG8	n;
	UINT	datsize;
	FDDFILE	fdd;

	switch(f->s.wt_mode) {
		case TAO_ENDOFDATA:
			if (data == TAO_MODE_GAP) {
				f->s.wt_mode = TAO_MODE_GAP;
				f->s.wt_ptr = 0;
			}
			break;

		case TAO_MODE_GAP:
			if (data == TAO_MODE_GAP) {
				f->s.wt_ptr++;
				if (f->s.wt_ptr >= 256) {
					goto wtd_done;
				}
			}
			else if (data == TAO_CMD_SYNC) {
				f->s.wt_mode = TAO_MODE_SYNC;
			}
			else if (data == 0xf4) {
				goto wtd_done;
			}
			else {
				goto wtd_err;
			}
			break;

		case TAO_MODE_SYNC:
			if (data == TAO_CMD_AM_IN) {
				f->s.wt_mode = TAO_MODE_AM;
			}
			else if (data == TAO_CMD_IM_IN) {
				f->s.wt_mode = TAO_MODE_IM;
			}
			else if (data) {
				goto wtd_err;
			}
			break;

		case TAO_MODE_IM:
			if (data == TAO_CMD_IM) {
				f->s.wt_mode = TAO_ENDOFDATA;
			}
			else if (data != TAO_CMD_IM_IN) {
				goto wtd_err;
			}
			break;

		case TAO_MODE_AM:
			if (data == TAO_CMD_IAM) {
				f->s.wt_mode = TAO_MODE_ID;
				f->s.wt_ptr = 0;
			}
			else if ((data == TAO_CMD_DAM) || (data == TAO_CMD_DDAM)) {
				f->s.wt_mode = TAO_MODE_DATA;
				f->s.wt_ptr = 0;
				if (f->s.wt_datsize) {
					t = (TAOSEC *)(f->s.buffer + f->s.wt_datpos);
					t[-1].flag = (UINT8)(data == TAO_CMD_DDAM);
				}
			}
			break;

		case TAO_MODE_ID:
			if ((f->s.wt_ptr == 0) && (data == TAO_CMD_IAM)) {
				break;
			}
			else if (f->s.wt_ptr < 4) {
				f->s.buffer[f->s.bufpos + f->s.wt_ptr] = data;
				f->s.wt_ptr++;
			}
			else if (data == TAO_CMD_CRC) {
				f->s.wt_mode = TAO_ENDOFDATA;
				n = f->s.buffer[f->s.bufpos + 3];
				if (n > 3) {
					n = 3;
				}
				datsize = 128 << n;
				if ((f->s.bufpos + (sizeof(TAOSEC) * 2) + datsize)
													<= sizeof(f->s.buffer)) {
					t = (TAOSEC *)(f->s.buffer + f->s.bufpos);
					STOREINTELWORD(t->size, datsize);
					f->s.wt_datpos = f->s.bufpos + sizeof(TAOSEC);
					f->s.wt_datsize = datsize;
					f->s.bufpos = f->s.wt_datpos + datsize;
					f->s.wt_sectors += 1;
				}
				else {
					goto wtd_err;
				}
			}
			break;

		case TAO_MODE_DATA:						/* DATA WRITE */
			if ((f->s.wt_ptr == 0) &&
				((data == TAO_CMD_DAM) || (data == TAO_CMD_DDAM))) {
				break;
			}
			else if (f->s.wt_ptr < f->s.wt_datsize) {
				f->s.buffer[f->s.wt_datpos + f->s.wt_ptr] = data;
				f->s.wt_ptr++;
			}
			else if (data == TAO_CMD_CRC) {
				f->s.wt_mode = TAO_ENDOFDATA;
				f->s.wt_datsize = 0;
			}
			break;
	}
	return;

wtd_done:
	fdd = fddfile + f->s.bufunit;
	TRACEOUT(("write! %d %dbytes", f->s.wt_sectors, f->s.bufpos));
	f->s.stat = (*fdd->wrtrk)(fdd, f->s.bufmedia, f->s.buftrack,
								f->s.wt_sectors, f->s.buffer, f->s.bufpos);
	f->s.bufdir = FDCDIR_NONE;
	dmac_sendready(FALSE);
	return;

wtd_err:
	f->s.stat = FDDSTAT_LOSTDATA;
	f->s.bufdir = FDCDIR_NONE;
	dmac_sendready(FALSE);
}
#endif


/* ---- */

void neitem_fdcbusy(UINT id) {

	fdc.s.busy = 0;
	if (fdc.s.bufdir) {
		/* TRACEOUT(("dma ready!")); */
		dmac_sendready(TRUE);
	}
	(void)id;
}

static void setbusy(FDC *f, SINT32 clock) {

	if (clock > 0) {
		f->s.busy = FDDSTAT_BUSY;
		nevent_set(NEVENT_FDC, clock, neitem_fdcbusy, NEVENT_ABSOLUTE);
	}
	else {
		f->s.busy = 0;
		nevent_reset(NEVENT_FDC);
	}
}

#if defined(SUPPORT_MOTORRISEUP)
static void setmotor(FDC *f, REG8 drvcmd) {

	UINT	drv;
	SINT32	clock;

	drv = drvcmd & 3;
	clock = CPU_CLOCK + CPU_BASECLOCK - CPU_REMCLOCK;
	if (drvcmd & 0x80) {
		if (f->s.motorevent[drv] == FDCMOTOR_STOP) {
			f->s.motorevent[drv] = FDCMOTOR_STARTING;
			f->s.motorclock[drv] = clock;
		}
		else if (f->s.motorevent[drv] == FDCMOTOR_STOPING) {
			f->s.motorevent[drv] = FDCMOTOR_READY;
		}
	}
	else {
		if ((f->s.motorevent[drv] == FDCMOTOR_STARTING) ||
			(f->s.motorevent[drv] == FDCMOTOR_READY)) {
			f->s.motorevent[drv] = FDCMOTOR_STOPING;
			f->s.motorclock[drv] = clock;
		}
	}
}

void fdc_callback(void) {

	SINT32	clock;
	UINT	i;

	clock = CPU_CLOCK + CPU_BASECLOCK - CPU_REMCLOCK;
	for (i=0; i<4; i++) {
		if (fdc.s.motorevent[i] == FDCMOTOR_STARTING) {
			if ((clock - fdc.s.motorclock[i]) >= (SINT32)pccore.realclock) {
				fdc.s.motorevent[i] = FDCMOTOR_READY;
			}
		}
		else if (fdc.s.motorevent[i] == FDCMOTOR_STOPING) {
			if ((clock - fdc.s.motorclock[i]) >= (SINT32)pccore.realclock) {
				fdc.s.motorevent[i] = FDCMOTOR_STOP;
			}
		}
	}
}

static SINT32 motorwait(const FDC *f) {

	SINT32	curclock;
	SINT32	nextclock;

	if (f->s.motorevent[f->s.drv] == FDCMOTOR_STARTING) {
		curclock = CPU_CLOCK + CPU_BASECLOCK - CPU_REMCLOCK;
		curclock -= f->s.motorclock[f->s.drv];
		if (curclock < (SINT32)pccore.realclock) {
			nextclock = pccore.realclock - curclock;
			/* TRACEOUT(("motor starting busy %d", nextclock)); */
			return(nextclock);
		}
	}
	return(0);
}
#endif


static REG8 getstat(FDC *f) {

	FDDFILE	fdd;
	REG8	ctype;
	REG8	ret;

	fdd = fddfile + f->s.drv;
	ctype = f->s.ctype;
	if (fdd->type == DISKTYPE_NOTREADY) {
		ret = FDDSTAT_NOTREADY;
	}
	else {
		ret = f->s.stat;
	}
	if ((ctype & FDCCTYPE_CMD1) && (f->s.c == 0)) {
		ret |= FDDSTAT_TRACK00;
	}
	if (!(ctype & FDCCTYPE_RO)) {
		if (fdd->protect) {
			ret |= FDDSTAT_WRITEP;
		}
	}
	if (ctype & (FDCCTYPE_CMD1 | FDCCTYPE_CMD4)) {
		f->s.hole++;
		if (f->s.hole < 8) {
			ret |= FDDSTAT_INDEX;
		}
	}
	else if (!(ret & 0xf0)) {
		if (fddmtr_isbusy()) {
			ret |= FDDSTAT_BUSY;
		}
		if (f->s.bufdir) {
			ret |= FDDSTAT_DRQ | FDDSTAT_BUSY;
		}
	}
	return(ret);
}

static void seekcmd(FDC *f) {

	FDDFILE	fdd;
	UINT	track;

	f->s.crcnum = 0;
	f->s.creg = f->s.c;
	fdd = fddfile + f->s.drv;
	track = (f->s.c << 1) + f->s.h;
	f->s.stat = fdd->seek(fdd, f->s.media, track) | FDDSTAT_HEADENG;
	fddmtr_motormove();
}

static REG8 type2cmd(FDC *f, REG8 sc) {

	REG8	dir;
	UINT	track;
	UINT8	*p;
	FDDFILE	fdd;
	UINT	size;
	REG8	stat;
	SINT32	clock;
#if defined(SUPPORT_DISKEXT)
	SINT32	curclock;
	SINT32	nextclock;
	UINT32	secinfo;
#endif

	track = (f->s.c << 1) + f->s.h;
	fdd = fddfile + f->s.drv;
#if !defined(CONST_DISKIMAGE)
	if (!(f->s.cmd & 0x20)) {
		p = f->s.buffer;
		dir = FDCDIR_IN;
	}
	else {
		p = NULL;
		dir = FDCDIR_OUT;
	}
	size = sizeof(f->s.buffer);
	stat = fdd->read(fdd, f->s.media, track, sc, p, &size);
	if (stat & FDDSTAT_RECNFND) {
		size = 0;
		dir = FDCDIR_NONE;
	}
	else if (dir == FDCDIR_OUT) {
		if (size) {
			ZeroMemory(f->s.buffer, size);
		}
		stat = stat & (~FDDSTAT_RECTYPE);
	}
#else
	size = 0;
	dir = FDCDIR_NONE;
	if (!(f->s.cmd & 0x20)) {
		stat = fdd->readp(fdd, f->s.media, track, sc, (void **)&p, &size);
		if (!(stat & FDDSTAT_RECNFND)) {
			f->e.buffer = p;
			dir = FDCDIR_IN;
		}
	}
	else {
		stat = FDDSTAT_RECNFND | FDDSTAT_WRITEFAULT;
	}
#endif
	f->s.bufmedia = f->s.media;
	f->s.bufunit = f->s.drv;
	f->s.buftrack = track;
	f->s.bufsc = sc;
	f->s.bufwrite = FALSE;
	f->s.bufdir = dir;
	f->s.bufmark = f->s.cmd & 1;
	f->s.bufpos = 0;
	f->s.bufsize = size;
	f->s.curtime = 0;

	clock = 0;
#if defined(SUPPORT_MOTORRISEUP)
	clock += motorwait(f);
#endif
#if defined(SUPPORT_DISKEXT)
	secinfo = fdd->sec(fdd, f->s.media, track, sc);
	if (secinfo) {
		nextclock = LOW16(secinfo);
		nextclock *= f->s.loopclock;
		nextclock /= LOW16(secinfo >> 16);
		curclock = nevent_getwork(NEVENT_RTC);
		nextclock -= curclock;
		if (nextclock < 0) {
			nextclock += f->s.loopclock;
		}
		/* TRACEOUT(("wait clock -> %d [%d/%d]", nextclock,
									LOW16(secinfo), LOW16(secinfo >> 16))); */
		clock += nextclock;
	}
#endif
	setbusy(f, max(clock, 500));
	return(stat);
}

static REG8 type2flash(FDC *f) {

#if !defined(CONST_DISKIMAGE)
	FDDFILE	fdd;

	f->s.bufwrite = FALSE;
	fdd = fddfile + f->s.bufunit;
	if (fdd->protect) {
		return(FDDSTAT_WRITEFAULT);
	}
	return(fdd->write(fdd, f->s.bufmedia, f->s.buftrack,
									f->s.bufsc, f->s.buffer, f->s.bufpos));
#else
	(void)f;
	return(FDDSTAT_RECNFND | FDDSTAT_WRITEFAULT);
#endif
}

static REG8 crccmd(FDC *f) {

	UINT8	*crcbuf;
	UINT	track;
	FDDFILE	fdd;
	REG8	stat;

#if !defined(CONST_DISKIMAGE)
	crcbuf = f->s.buffer;
#else
	crcbuf = f->s.crcbuf;
	f->e.buffer = crcbuf;
#endif

	track = (f->s.c << 1) + f->s.h;
	fdd = fddfile + f->s.drv;
	/* TRACEOUT(("fdd->crc %d %d %d", f->s.drv, track, f->s.crcnum)); */
	stat = fdd->crc(fdd, f->s.media, track, f->s.crcnum, crcbuf);
	if (stat & FDDSTAT_RECNFND) {
		f->s.crcnum = 0;
		stat = fdd->crc(fdd, f->s.media, track, 0, crcbuf);
	}
	if (!(stat & FDDSTAT_RECNFND)) {
		f->s.bufdir = FDCDIR_IN;
		f->s.bufsize = 6;
		f->s.rreg = crcbuf[0];
		f->s.crcnum++;
	}
	else {
		f->s.bufdir = FDCDIR_NONE;
		f->s.bufsize = 0;
	}
	f->s.bufwrite = FALSE;
	f->s.curtime = 0;
	return(stat);
}

static void fdcenddata(FDC *f) {

	BRESULT	r;
	REG8	stat;

	r = FALSE;
	if (f->s.ctype & FDCCTYPE_CMD2) {
		stat = 0;
		if (f->s.cmd & 0x10) {
			r = TRUE;
		}
		if ((f->s.cmd & 0x20) && (f->s.bufwrite)) {
			stat = type2flash(f);
			if (stat & (FDDSTAT_RECNFND | FDDSTAT_WRITEFAULT)) {
				r = FALSE;
			}
			f->s.stat = stat;
		}
	}
	f->s.bufdir = FDCDIR_NONE;
	dmac_sendready(FALSE);
	if (r) {
		f->s.rreg = f->s.r + 1;
		stat = type2cmd(f, f->s.rreg);
		if (!(stat & FDDSTAT_RECNFND)) {
			f->s.r = f->s.r + 1;
			f->s.stat = stat;
		}
	}
}



/* IO-Sub */

static void IOOUTCALL fdc_o0ff8(FDC *f, REG8 value) {

	REG8	cmd;

	/* コマンド */
	if (f->s.bufwrite) {
		f->s.stat = type2flash(f);
	}
	if (f->s.bufdir != FDCDIR_NONE) {
		f->s.bufdir = FDCDIR_NONE;
		dmac_sendready(FALSE);
	}

	f->s.cmd = value;
	cmd = (REG8)(value >> 4);
	f->s.ctype = fdctype[cmd];
	/* TRACEOUT(("fdc cmd: %.2x", value)); */
	/* リストアコマンドにおいて
	 * 　マリオは コマンド発行後にbusyを見張る
	 * 　逆にソーサリアンとかは busyだとエラーになる…
	 * 条件は何？
	 */
	setbusy(f, 20);
	switch(cmd) {
		case 0x00:								/* リストア */
			f->s.motor = 0x80;					/* モーターOn? */
			f->s.c = 0;
			f->s.step = 1;
			f->s.r = 0;							/* デゼニワールド */
			seekcmd(f);
			f->s.rreg = 0;
			break;

		case 0x01:								/* シーク */
			f->s.motor = 0x80;					/* モーターOn */
			f->s.step = (SINT8)((f->s.c<=f->s.data)?1:-1);
			f->s.c = f->s.data;
			seekcmd(f);
			break;

		case 0x02:								/* ステップ */
		case 0x03:
		case 0x04:								/* ステップイン */
		case 0x05:
		case 0x06:								/* ステップアウト */
		case 0x07:
			f->s.stat = FDDSTAT_HEADENG;
			if (f->s.motor) {
				if (cmd & 0x04) {
					f->s.step = (cmd & 0x02)?-1:1;
				}
				f->s.c += f->s.step;
				if (cmd & 1) {
					seekcmd(f);
				}
			}
			break;

		case 0x08:								/* リードデータ */
		case 0x09:
		case 0x0a:								/* ライトデータ */
		case 0x0b:
			f->s.stat = type2cmd(f, f->s.r);
			break;

		case 0xc:								/* リードアドレス */
			setbusy(f, 200);
			f->s.stat = crccmd(f);
			break;

		case 0x0d:								/* フォースインタラプト */
			setbusy(f, 0);						/* 必要ない？ */
			/* f->s.skip = 0; */				/* 000330 */
			f->s.stat = 0;
			dmac_sendready(FALSE);
			break;

		case 0x0e:								/* リードトラック */
#if !defined(CONST_DISKIMAGE)
			setbusy(f, 200);
			ZeroMemory(f->s.buffer, 0x1a00);
			f->s.bufpos = 0;
			f->s.bufsize = 0x1a00;
			f->s.bufdir = FDCDIR_IN;
			f->s.stat = 0;
#else
			f->s.stat = FDDSTAT_SEEKERR;
#endif
			break;

		case 0x0f:								/* ライトトラック */
#if !defined(CONST_DISKIMAGE)
			setbusy(f, 200);
			f->s.stat = wrtrkstart(f);
#else
			f->s.stat = FDDSTAT_LOSTDATA;
#endif
			break;
	}
}

static void IOOUTCALL fdc_o0ff9(FDC *f, REG8 value) {

	/* トラック */
	f->s.creg = value;
}

static void IOOUTCALL fdc_o0ffa(FDC *f, REG8 value) {

	/* セクタ */
	fddmtr_waitsec(value);
	f->s.r = value;
	f->s.rreg = value;
}

static void IOOUTCALL fdc_o0ffb(FDC *f, REG8 value) {

	/* データ */
	f->s.data = value;
#if !defined(CONST_DISKIMAGE)
	if (f->s.motor) {
		if (f->s.bufdir == FDCDIR_OUT) {
			f->s.bufwrite = TRUE;
			f->s.curtime = 0;
			f->s.buffer[f->s.bufpos] = value;
			if (!f->s.busy) {
				f->s.bufpos++;
				if (f->s.bufpos >= f->s.bufsize) {
					fdcenddata(f);
				}
			}
		}
		else if (f->s.bufdir == FDCDIR_TAO) {
			wrtrkdata(f, value);
		}
	}
#endif
}

static void IOOUTCALL fdc_o0ffc(FDC *f, REG8 value) {

	/* ドライブ・サイド */
	f->s.ctbl[f->s.drv] = f->s.c;
	f->s.c = f->s.ctbl[value & 0x03];
	f->s.motor = (UINT8)(value & 0x80);
	f->s.drv = (UINT8)(value & 0x03);
	f->s.h = (UINT8)((value >> 4) & 1);
	f->s.cmd = 0;							/* T&E SORCERIAN */
	f->s.ctype = 0;
	f->s.stat = 0;

	fddmtr_drvset();
	if (!f->s.motor) {
		f->s.r = 0;						/* SACOM TELENET */
		f->s.rreg = 0;
	}
#if defined(SUPPORT_MOTORRISEUP)
	setmotor(f, value);
#endif
}

static void IOOUTCALL fdc_o0(FDC *f, REG8 value) {
}

static REG8 IOINPCALL fdc_i0ff8(FDC *f) {

	REG8	ret;

	/* ステータス */
	ret = f->s.busy;
	if (ret) {
		return(ret);
	}
#if 1
	if (f->s.bufdir >= FDCDIR_IN) {		/* YsII */
		f->s.curtime++;
		if (f->s.curtime >= 8) {
			f->s.curtime = 0;
			f->s.stat |= FDDSTAT_LOSTDATA;
			f->s.bufpos++;
			if (f->s.bufpos >= f->s.bufsize) {
				fdcenddata(f);
			}
		}
	}
#endif
	ret = getstat(f);
#if 1
	if (!(ret & 0x02)) {
		dmac_sendready(FALSE);
	}
#endif
	/* TRACEOUT(("ret->%.2x", ret)); */
	return(ret);
}

static REG8 IOINPCALL fdc_i0ff9(FDC *f) {

	/* トラック */
	TRACEOUT(("fdc inp %.4x,%.2x", 0x0ff9, f->s.creg));
	return(f->s.creg);
}

static REG8 IOINPCALL fdc_i0ffa(FDC *f) {

	/* セクタ */
	TRACEOUT(("fdc inp %.4x,%.2x", 0x0ffa, f->s.rreg));
	return(f->s.rreg);
}

static REG8 IOINPCALL fdc_i0ffb(FDC *f) {

	/* データ */
	if (f->s.motor) {
		if (f->s.bufdir == FDCDIR_IN) {
			f->s.curtime = 0;
#if !defined(CONST_DISKIMAGE)
			f->s.data = f->s.buffer[f->s.bufpos];
#else
			f->s.data = f->e.buffer[f->s.bufpos];
#endif
			if (!f->s.busy) {
				f->s.bufpos++;
				if (f->s.bufpos >= f->s.bufsize) {
					fdcenddata(f);
				}
			}
			/* TRACEOUT(("read %.2x - %.2x [%.4x]", f->s.bufpos, f->s.data, Z80_PC)); */
		}
	}
	return(f->s.data);
}

static REG8 IOINPCALL fdc_i0ffc(FDC *f) {

	/* FM */
	return(0x00);
}

static REG8 IOINPCALL fdc_i0ffd(FDC *f) {

	/* MFM */
	return(0x00);
}

static REG8 IOINPCALL fdc_i0ffe(FDC *f) {

	/* 1.6M */
	f->s.media = DISKTYPE_2HD;
	return(0xff);
}

static REG8 IOINPCALL fdc_i0fff(FDC *f) {

	/* 500K/1M */
	f->s.media = DISKTYPE_2D;
	return(0xff);
}


/* IO */

typedef void (IOINPCALL * FNFDCOUT)(FDC *f, REG8 value);
static const FNFDCOUT s_fnOut[] =
{
	fdc_o0ff8,	fdc_o0ff9,	fdc_o0ffa,	fdc_o0ffb,
	fdc_o0ffc,	fdc_o0,		fdc_o0,		fdc_o0,
};

typedef REG8 (IOINPCALL * FNFDCINP)(FDC *f);
static const FNFDCINP s_fnInp[] =
{
	fdc_i0ff8,	fdc_i0ff9,	fdc_i0ffa,	fdc_i0ffb,
	fdc_i0ffc,	fdc_i0ffd,	fdc_i0ffe,	fdc_i0fff,
};

void IOINPCALL fdc_o(UINT port, REG8 value)
{
	if ((port & (~7)) != 0x0ff8)
	{
		return;
	}

	/* TRACEOUT(("fdc out %.4x,%.2x", port, value)); */
	(s_fnOut[port & 7])(&fdc, value);
}

REG8 IOINPCALL fdc_i(UINT uPort)
{
	if ((uPort & (~7)) != 0x0ff8)
	{
		return 0xff;
	}

	/* TRACEOUT(("fdc inp %.4x", port)); */
	return (s_fnInp[uPort & 7])(&fdc);
}


/* reset */

void fdc_reset(void) {

	fddmtr_initialize();
	ZeroMemory(&fdc, sizeof(fdc));
	fdc.s.step = 1;
	fdc.s.equip = xmilcfg.fddequip;
#if defined(FIX_Z80A)
	fdc.s.loopclock = 2000000 * 2 / 5;
#else
	fdc.s.loopclock = pccore.realclock / 5;
#endif
}

