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


// write track

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

		case TAO_MODE_DATA:						// DATA WRITE
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


// ----

void neitem_fdcbusy(UINT id) {

	fdc.s.busy = 0;
	if (fdc.s.bufdir) {
//		TRACEOUT(("dma ready!"));
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
//			TRACEOUT(("motor starting busy %d", nextclock));
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
//		TRACEOUT(("wait clock -> %d [%d/%d]", nextclock,
//									LOW16(secinfo), LOW16(secinfo >> 16)));
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
// TRACEOUT(("fdd->crc %d %d %d", f->s.drv, track, f->s.crcnum));
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
			stat = type2flash(&fdc);
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

void IOOUTCALL fdc_o(UINT port, REG8 value) {

	REG8	cmd;

	if ((port & (~7)) != 0x0ff8) {
		return;
	}
	TRACEOUT(("fdc %.4x,%.2x [%.4x]", port, value, Z80_PC));
	switch(port & 7) {
		case 0:									// �R�}���h
			if (fdc.s.bufwrite) {
				fdc.s.stat = type2flash(&fdc);
			}
			if (fdc.s.bufdir != FDCDIR_NONE) {
				fdc.s.bufdir = FDCDIR_NONE;
				dmac_sendready(FALSE);
			}

			fdc.s.cmd = value;
			cmd = (REG8)(value >> 4);
			fdc.s.ctype = fdctype[cmd];
//			TRACEOUT(("fdc cmd: %.2x", value));
			// ���X�g�A�R�}���h�ɂ�����
			// �@�}���I�� �R�}���h���s���busy��������
			// �@�t�Ƀ\�[�T���A���Ƃ��� busy���ƃG���[�ɂȂ�c
			// �����͉��H
			setbusy(&fdc, 20);
			switch(cmd) {
				case 0x00:								// ���X�g�A
					fdc.s.motor = 0x80;					// ���[�^�[On?
					fdc.s.c = 0;
					fdc.s.step = 1;
					fdc.s.r = 0;						// �f�[�j���[���h
					seekcmd(&fdc);
					fdc.s.rreg = 0;
					break;

				case 0x01:								// �V�[�N
					fdc.s.motor = 0x80;					// ���[�^�[On
					fdc.s.step = (SINT8)((fdc.s.c<=fdc.s.data)?1:-1);
					fdc.s.c = fdc.s.data;
					seekcmd(&fdc);
					break;

				case 0x02:								// �X�e�b�v
				case 0x03:
				case 0x04:								// �X�e�b�v�C��
				case 0x05:
				case 0x06:								// �X�e�b�v�A�E�g
				case 0x07:
					fdc.s.stat = FDDSTAT_HEADENG;
					if (fdc.s.motor) {
						if (cmd & 0x04) {
							fdc.s.step = (cmd & 0x02)?-1:1;
						}
						fdc.s.c += fdc.s.step;
						if (cmd & 1) {
							seekcmd(&fdc);
						}
					}
					break;

				case 0x08:								// ���[�h�f�[�^
				case 0x09:
				case 0x0a:								// ���C�g�f�[�^
				case 0x0b:
					fdc.s.stat = type2cmd(&fdc, fdc.s.r);
					break;

				case 0xc:								// ���[�h�A�h���X
					setbusy(&fdc, 200);
					fdc.s.stat = crccmd(&fdc);
					break;

				case 0x0d:								// �t�H�[�X�C���^���v�g
					setbusy(&fdc, 0);					// �K�v�Ȃ��H
//					fdc.s.skip = 0;						// 000330
					fdc.s.stat = 0;
					dmac_sendready(FALSE);
					break;

				case 0x0e:								// ���[�h�g���b�N
#if !defined(CONST_DISKIMAGE)
					setbusy(&fdc, 200);
					ZeroMemory(fdc.s.buffer, 0x1a00);
					fdc.s.bufpos = 0;
					fdc.s.bufsize = 0x1a00;
					fdc.s.bufdir = FDCDIR_IN;
					fdc.s.stat = 0;
#else
					fdc.s.stat = FDDSTAT_SEEKERR;
#endif
					break;

				case 0x0f:								// ���C�g�g���b�N
#if !defined(CONST_DISKIMAGE)
					setbusy(&fdc, 200);
					fdc.s.stat = wrtrkstart(&fdc);
#else
					fdc.s.stat = FDDSTAT_LOSTDATA;
#endif
					break;
			}
			break;

		case 1:									// �g���b�N
			fdc.s.creg = value;
			break;

		case 2:									// �Z�N�^
			fddmtr_waitsec(value);
			fdc.s.r = value;
			fdc.s.rreg = value;
			break;

		case 3:									// �f�[�^
			fdc.s.data = value;
#if !defined(CONST_DISKIMAGE)
			if (fdc.s.motor) {
				if (fdc.s.bufdir == FDCDIR_OUT) {
					fdc.s.bufwrite = TRUE;
					fdc.s.curtime = 0;
					fdc.s.buffer[fdc.s.bufpos] = value;
					if (!fdc.s.busy) {
						fdc.s.bufpos++;
						if (fdc.s.bufpos >= fdc.s.bufsize) {
							fdcenddata(&fdc);
						}
					}
				}
				else if (fdc.s.bufdir == FDCDIR_TAO) {
					wrtrkdata(&fdc, value);
				}
			}
#endif
			break;

		case 4:									// �h���C�u�E�T�C�h
			fdc.s.ctbl[fdc.s.drv] = fdc.s.c;
			fdc.s.c = fdc.s.ctbl[value & 0x03];
			fdc.s.motor = (UINT8)(value & 0x80);
			fdc.s.drv = (UINT8)(value & 0x03);
			fdc.s.h = (UINT8)((value >> 4) & 1);
			fdc.s.cmd = 0;							// T&E SORCERIAN
			fdc.s.ctype = 0;
			fdc.s.stat = 0;

			fddmtr_drvset();
			if (!fdc.s.motor) {
				fdc.s.r = 0;						// SACOM TELENET
				fdc.s.rreg = 0;
			}
#if defined(SUPPORT_MOTORRISEUP)
			setmotor(&fdc, value);
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
		case 0:										// �X�e�[�^�X
			ret = fdc.s.busy;
			if (ret) {
				return(ret);
			}
#if 1
			if (fdc.s.bufdir >= FDCDIR_IN) {		// YsII
				fdc.s.curtime++;
				if (fdc.s.curtime >= 8) {
					fdc.s.curtime = 0;
					fdc.s.stat |= FDDSTAT_LOSTDATA;
					fdc.s.bufpos++;
					if (fdc.s.bufpos >= fdc.s.bufsize) {
						fdcenddata(&fdc);
					}
				}
			}
#endif
			ret = getstat(&fdc);
#if 1
			if (!(ret & 0x02)) {
				dmac_sendready(FALSE);
			}
#endif
//			TRACEOUT(("ret->%.2x", ret));
			return(ret);

		case 1:									// �g���b�N
TRACEOUT(("fdc inp %.4x,%.2x", port, fdc.s.creg));
			return(fdc.s.creg);

		case 2:									// �Z�N�^
TRACEOUT(("fdc inp %.4x,%.2x", port, fdc.s.rreg));
			return(fdc.s.rreg);

		case 3:									// �f�[�^
			if (fdc.s.motor) {
				if (fdc.s.bufdir == FDCDIR_IN) {
					fdc.s.curtime = 0;
#if !defined(CONST_DISKIMAGE)
					fdc.s.data = fdc.s.buffer[fdc.s.bufpos];
#else
					fdc.s.data = fdc.e.buffer[fdc.s.bufpos];
#endif
// TRACEOUT(("read %.2x - %.2x [%.4x]", fdc.s.bufpos, fdc.s.data, Z80_PC));
					if (!fdc.s.busy) {
						fdc.s.bufpos++;
						if (fdc.s.bufpos >= fdc.s.bufsize) {
							fdcenddata(&fdc);
						}
					}
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

