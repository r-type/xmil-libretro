#include	"compiler.h"
#include	"dosio.h"
#include	"z80core.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"nevent.h"
#include	"fddfile.h"
#include	"fdd_2d.h"
#include	"fdd_d88.h"
#include	"fdd_mtr.h"


static const UINT8 fdctype[] = {1,1,1,1,1,1,1,1,2,2,2,2,3,4,3,3};

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


// ----

void neitem_fdcbusy(UINT id) {

	fdc.s.busy = FALSE;
	if (fdc.s.bufdir) {
//		TRACEOUT(("dma ready!"));
		dmac_sendready(TRUE);
	}
	(void)id;
}

static void setbusy(SINT32 clock) {

	if (clock > 0) {
		fdc.s.busy = TRUE;
		nevent_set(NEVENT_FDC, clock, neitem_fdcbusy, NEVENT_ABSOLUTE);
	}
	else {
		fdc.s.busy = FALSE;
		nevent_reset(NEVENT_FDC);
	}
}

#if defined(SUPPORT_MOTORRISEUP)
static void setmotor(REG8 drvcmd) {

	UINT	drv;
	SINT32	clock;

	drv = drvcmd & 3;
	clock = CPU_CLOCK + CPU_BASECLOCK - CPU_REMCLOCK;
	if (drvcmd & 0x80) {
		if (fdc.s.motorevent[drv] == FDCMOTOR_STOP) {
			fdc.s.motorevent[drv] = FDCMOTOR_STARTING;
			fdc.s.motorclock[drv] = clock;
		}
		else if (fdc.s.motorevent[drv] == FDCMOTOR_STOPING) {
			fdc.s.motorevent[drv] = FDCMOTOR_READY;
		}
	}
	else {
		if ((fdc.s.motorevent[drv] == FDCMOTOR_STARTING) ||
			(fdc.s.motorevent[drv] == FDCMOTOR_READY)) {
			fdc.s.motorevent[drv] = FDCMOTOR_STOPING;
			fdc.s.motorclock[drv] = clock;
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

static SINT32 motorwait(REG8 drv) {

	SINT32	curclock;
	SINT32	nextclock;

	if (fdc.s.motorevent[drv] == FDCMOTOR_STARTING) {
		curclock = CPU_CLOCK + CPU_BASECLOCK - CPU_REMCLOCK;
		curclock -= fdc.s.motorclock[drv];
		if (curclock < (SINT32)pccore.realclock) {
			nextclock = pccore.realclock - curclock;
//			TRACEOUT(("motor starting busy %d", nextclock));
			return(nextclock);
		}
	}
	return(0);
}
#endif


static REG8 getstat(void) {

	FDDFILE	fdd;
	REG8	cmd;
	REG8	type;
	REG8	ret;

	fdd = fddfile + fdc.s.drv;
	cmd = (REG8)(fdc.s.cmd >> 4);
	type = fdc.s.type;
	if (fdd->type == DISKTYPE_NOTREADY) {
		ret = FDDSTAT_NOTREADY;
	}
	else if (type != 0) {
		ret = fdc.s.stat;
	}
	else {
		ret = 0;
	}
	if ((type == 1) && (fdc.s.c == 0)) {
		ret |= FDDSTAT_TRACK00;
	}
	if ((type == 0) || (type == 1) || (type == 4) ||
		(cmd == 0x0a) || (cmd == 0x0b) || (cmd == 0x0f)) {
		if (fdd->protect) {
			ret |= FDDSTAT_WRITEP;
		}
	}
	if ((type == 1) || (type == 4)) {
		fdc.s.hole++;
		if (fdc.s.hole < 8) {
			ret |= FDDSTAT_INDEX;
		}
	}
	else if (!(ret & 0xf0)) {
		if (FDDMTR_BUSY) {
			ret |= FDDSTAT_BUSY;
		}
		if (fdc.s.bufdir) {
			ret |= FDDSTAT_DRQ | FDDSTAT_BUSY;
		}
		else if (cmd == 0x0f) {
			ret |= FDDSTAT_LOSTDATA;
		}
	}
	return(ret);
}

static void seekcmd(void) {

	FDDFILE	fdd;
	UINT	track;

	fdc.s.crcnum = 0;
	fdc.s.creg = fdc.s.c;
	fdd = fddfile + fdc.s.drv;
	track = (fdc.s.c << 1) + fdc.s.h;
	fdc.s.stat = fdd->seek(fdd, fdc.s.media, track) | FDDSTAT_HEADENG;
	FDDMTR_MOVE;
}

static REG8 type2cmd(REG8 sc) {

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

	track = (fdc.s.c << 1) + fdc.s.h;
	if (!(fdc.s.cmd & 0x20)) {
		p = fdc.s.buffer;
		dir = FDCDIR_IN;
	}
	else {
		p = NULL;
		dir = FDCDIR_OUT;
	}
	size = sizeof(fdc.s.buffer);
	fdd = fddfile + fdc.s.drv;
//	TRACEOUT(("read %.2x %d %d", fdc.s.drv, track, sc));
	stat = fdd->read(fdd, fdc.s.media, track, sc, p, &size);
	if (stat & FDDSTAT_RECNFND) {
		size = 0;
		dir = FDCDIR_NONE;
	}
	else if (dir == FDCDIR_OUT) {
		if (size) {
			ZeroMemory(fdc.s.buffer, size);
		}
		stat = stat & (~FDDSTAT_RECTYPE);
	}
	fdc.s.bufmedia = fdc.s.media;
	fdc.s.bufunit = fdc.s.drv;
	fdc.s.buftrack = track;
	fdc.s.bufsc = sc;
	fdc.s.bufwrite = FALSE;
	fdc.s.bufdir = dir;
	fdc.s.bufmark = fdc.s.cmd & 1;
	fdc.s.bufpos = 0;
	fdc.s.bufsize = size;
	fdc.s.curtime = 0;

	clock = 0;
#if defined(SUPPORT_MOTORRISEUP)
	clock += motorwait(fdc.s.drv);
#endif
#if defined(SUPPORT_DISKEXT)
	secinfo = fdd->sec(fdd, fdc.s.media, track, sc);
	if (secinfo) {
		nextclock = LOW16(secinfo);
		nextclock *= fdc.s.loopclock;
		nextclock /= LOW16(secinfo >> 16);
		curclock = nevent_getwork(NEVENT_RTC);
		nextclock -= curclock;
		if (nextclock < 0) {
			nextclock += fdc.s.loopclock;
		}
//		TRACEOUT(("wait clock -> %d [%d/%d]", nextclock,
//									LOW16(secinfo), LOW16(secinfo >> 16)));
		clock += nextclock;
	}
#endif
	setbusy(max(clock, 500));
	return(stat);
}

static REG8 type2flash(void) {

	FDDFILE	fdd;

	fdc.s.bufwrite = FALSE;
	fdd = fddfile + fdc.s.bufunit;
	if (fdd->protect) {
		return(FDDSTAT_WRITEFAULT);
	}
	return(fdd->write(fdd, fdc.s.bufmedia, fdc.s.buftrack,
									fdc.s.bufsc, fdc.s.buffer, fdc.s.bufpos));
}

static REG8 crccmd(void) {

	UINT	track;
	FDDFILE	fdd;
	REG8	stat;

	track = (fdc.s.c << 1) + fdc.s.h;
	fdd = fddfile + fdc.s.drv;
// TRACEOUT(("fdd->crc %d %d %d", fdc.s.drv, track, fdc.s.crcnum));
	stat = fdd->crc(fdd, fdc.s.media, track, fdc.s.crcnum, fdc.s.buffer);
	if (stat & FDDSTAT_RECNFND) {
		fdc.s.crcnum = 0;
		stat = fdd->crc(fdd, fdc.s.media, track, 0, fdc.s.buffer);
	}
	if (!(stat & FDDSTAT_RECNFND)) {
		fdc.s.bufdir = FDCDIR_IN;
		fdc.s.bufsize = 6;
		fdc.s.rreg = fdc.s.buffer[0];
		fdc.s.crcnum++;
	}
	else {
		fdc.s.bufdir = FDCDIR_NONE;
		fdc.s.bufsize = 0;
	}
	fdc.s.bufwrite = FALSE;
	fdc.s.curtime = 0;
	return(stat);
}

static void bufposinc(void) {

	BRESULT	r;
	REG8	stat;

	if (fdc.s.busy) {
		return;
	}
	fdc.s.bufpos++;
	fdc.s.curtime = 0;
	if (fdc.s.bufpos >= fdc.s.bufsize) {
		r = FALSE;
		if (fdc.s.type == 2) {
			stat = 0;
			if (fdc.s.cmd & 0x10) {
				r = TRUE;
			}
			if ((fdc.s.cmd & 0x20) && (fdc.s.bufwrite)) {
				stat = type2flash();
				if (stat & (FDDSTAT_RECNFND | FDDSTAT_WRITEFAULT)) {
					r = FALSE;
				}
				fdc.s.stat = stat;
			}
		}
		fdc.s.bufdir = FDCDIR_NONE;
		dmac_sendready(FALSE);
		if (r) {
			fdc.s.rreg = fdc.s.r + 1;
			stat = type2cmd(fdc.s.rreg);
			if (!(stat & FDDSTAT_RECNFND)) {
				fdc.s.r = fdc.s.r + 1;
				fdc.s.stat = stat;
			}
		}
	}
}

void IOOUTCALL fdc_o(UINT port, REG8 value) {

	REG8	cmd;

	if ((port & (~7)) != 0x0ff8) {
		return;
	}
	TRACEOUT(("fdc %.4x,%.2x [%.4x]", port, value, Z80_PC));
	switch(port & 7) {
		case 0:									// コマンド
			fdc.s.cmd = value;
			cmd = (REG8)(value >> 4);
			fdc.s.type = fdctype[cmd];
//			TRACEOUT(("fdc cmd: %.2x", value));
			if (fdc.s.bufwrite) {
				fdc.s.stat = type2flash();
			}
			fdc.s.bufdir = FDCDIR_NONE;
			// リストアコマンドにおいて
			// 　マリオは コマンド発行後にbusyを見張る
			// 　逆にソーサリアンとかは busyだとエラーになる…
			// 条件は何？
			setbusy(20);
			switch(cmd) {
				case 0x00:								// リストア
					fdc.s.motor = 0x80;					// モーターOn?
					fdc.s.c = 0;
					fdc.s.step = 1;
					fdc.s.r = 0;						// デゼニワールド
					seekcmd();
					fdc.s.rreg = 0;
					break;

				case 0x01:								// シーク
					fdc.s.motor = 0x80;					// モーターOn
					fdc.s.step = (SINT8)((fdc.s.c<=fdc.s.data)?1:-1);
					fdc.s.c = fdc.s.data;
					seekcmd();
					break;

				case 0x02:								// ステップ
				case 0x03:
					if (fdc.s.motor) {
						fdc.s.c += fdc.s.step;
						if (cmd & 1) {
							seekcmd();
						}
					}
					break;

				case 0x04:								// ステップイン
				case 0x05:
					if (fdc.s.motor) {
						fdc.s.step = 1;
						fdc.s.c++;
						if (cmd & 1) {
							seekcmd();
						}
					}
					break;

				case 0x06:								// ステップアウト
				case 0x07:
					if (fdc.s.motor) {
						fdc.s.step = -1;
						fdc.s.c--;
						if (cmd & 1) {
							seekcmd();
						}
					}
					break;

				case 0x08:								// リードデータ
				case 0x09:
				case 0x0a:								// ライトデータ
				case 0x0b:
					fdc.s.stat = type2cmd(fdc.s.r);
					break;

				case 0xc:								// リードアドレス
					setbusy(200);
					fdc.s.stat = crccmd();
					break;

				case 0x0d:								// フォースインタラプト
					setbusy(0);							// 必要ない？
//					fdc.s.skip = 0;						// 000330
					fdc.s.stat = 0;
					dmac_sendready(FALSE);
					break;

				case 0x0e:								// リードトラック
					setbusy(200);
					ZeroMemory(fdc.s.buffer, 0x1a00);
					fdc.s.bufpos = 0;
					fdc.s.bufsize = 0x1a00;
					fdc.s.bufdir = FDCDIR_IN;
					break;

				case 0x0f:								// ライトトラック
					break;
			}
			break;

		case 1:									// トラック
			fdc.s.c = value;
			break;

		case 2:									// セクタ
			FDDMTR_WAITSEC(value);
			fdc.s.r = value;
			fdc.s.rreg = value;
			break;

		case 3:									// データ
			fdc.s.data = value;
			if ((fdc.s.motor) && (fdc.s.bufdir == FDCDIR_OUT)) {
				fdc.s.buffer[fdc.s.bufpos] = value;
				fdc.s.bufwrite = TRUE;
				bufposinc();
			}
			break;

		case 4:									// ドライブ・サイド
			fdc.s.ctbl[fdc.s.drv] = fdc.s.c;
			fdc.s.c = fdc.s.ctbl[value & 0x03];
			fdc.s.motor = (UINT8)(value & 0x80);
			fdc.s.drv = (UINT8)(value & 0x03);
			fdc.s.h = (UINT8)((value >> 4) & 1);
			fdc.s.cmd = 0;							// T&E SORCERIAN
			fdc.s.type = 0;

			FDDMTR_DRVSET;
			if (!fdc.s.motor) {
				fdc.s.r = 0;						// SACOM TELENET
				fdc.s.rreg = 0;
			}
#if defined(SUPPORT_MOTORRISEUP)
			setmotor(value);
#endif
			break;
	}
}

REG8 IOINPCALL fdc_i(UINT port) {

	REG8	ret;

//	TRACEOUT(("fdc inp %.4x", port));

	if ((port & (~7)) != 0x0ff8) {
		return(0xff);
	}
	switch(port & 7) {
		case 0:									// ステータス
			ret = fdc.s.busy;
			if (ret) {
				return(ret);
			}
#if 1
			if (fdc.s.bufdir) {					// YsII
				fdc.s.curtime++;
				if (fdc.s.curtime >= 8) {
					fdc.s.stat |= 0x04;
					bufposinc();
				}
			}
#endif
			ret = getstat();
			if (!(ret & 0x02)) {
				dmac_sendready(FALSE);
			}
//			TRACEOUT(("ret->%.2x", ret));
			return(ret);

		case 1:									// トラック
			return(fdc.s.creg);

		case 2:									// セクタ
			return(fdc.s.rreg);

		case 3:									// データ
			if (fdc.s.motor) {
				if (fdc.s.bufdir == FDCDIR_IN) {
					fdc.s.data = fdc.s.buffer[fdc.s.bufpos];
// TRACEOUT(("read %.2x - %.2x [%.4x]", fdc.s.bufpos, fdc.s.data, Z80_PC));
					bufposinc();
				}
			}
			return(fdc.s.data);

		case 4:									// FM
		case 5:									// MFM
			return(0x00);

		case 6:									// 1.6M
			fdc.s.media = DISKTYPE_2HD;
			break;

		case 7:									// 500K/1M
			fdc.s.media = DISKTYPE_2D;
			break;
	}
	return(0xff);
}


// ----

void fdc_reset(void) {

	FDDMTR_INIT;
	ZeroMemory(&fdc, sizeof(fdc));
	fdc.s.step = 1;
	fdc.s.equip = xmilcfg.fddequip;
	fdc.s.loopclock = pccore.realclock / 5;
}

