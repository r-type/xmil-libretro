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


void nvitem_fdcbusy(UINT id) {

	fdc.s.busy = FALSE;
	if (fdc.s.bufdir) {
		TRACEOUT(("dma ready!"));
		dmac_sendready(TRUE);
	}
	(void)id;
}

static void setbusy(UINT clock) {

	fdc.s.busy = TRUE;
	nevent_set(NEVENT_FDC, clock, nvitem_fdcbusy, NEVENT_ABSOLUTE);
}

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
	TRACEOUT(("read %.2x %d %d", fdc.s.drv, track, sc));
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
	stat = fdd->crc(fdd, fdc.s.media, track, fdc.s.crcnum, fdc.s.buffer);
	if (stat & FDDSTAT_RECNFND) {
		fdc.s.crcnum = 0;
		stat = fdd->crc(fdd, fdc.s.media, track, 0, fdc.s.buffer);
	}
	if (!(stat & FDDSTAT_RECNFND)) {
		fdc.s.bufdir = FDCDIR_IN;
		fdc.s.bufsize = 6;
		fdc.s.rreg = fdc.s.buffer[0];
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
			if (r) {
				fdc.s.rreg = fdc.s.r + 1;
				stat = type2cmd(fdc.s.rreg);
				if (!(stat & FDDSTAT_RECNFND)) {
					fdc.s.r = fdc.s.r + 1;
					fdc.s.stat = stat;
				}
				else {
					r = FALSE;
				}
			}
		}
		if (!r) {
			fdc.s.bufdir = FDCDIR_NONE;
			dmac_sendready(FALSE);
		}
	}
}

void IOOUTCALL fdc_o(UINT port, REG8 value) {

	REG8	cmd;

	if ((port & (~7)) != 0x0ff8) {
		return;
	}
//	TRACEOUT(("fdc %.4x,%.2x [%.4x]", port, value, Z80_PC));
	switch(port & 7) {
		case 0:									// コマンド
			fdc.s.cmd = value;
			cmd = (REG8)(value >> 4);
			fdc.s.type = fdctype[cmd];
			TRACEOUT(("fdc cmd: %.2x", value));
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
				//	if (value & 8) {					// LAYDOCK
				//		setbusy(0);
				//	}
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
					setbusy(500);
					fdc.s.stat = type2cmd(fdc.s.r);
					break;

				case 0xc:								// リードアドレス
					setbusy(200);
					fdc.s.stat = crccmd();
					break;

				case 0x0d:								// フォースインタラプト
					setbusy(0);							// 必要ない？
//					fdc.s.skip = 0;						// 000330
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
			break;
	}
}

REG8 IOINPCALL fdc_i(UINT port) {

// static	BYTE	timeoutwait;
// static	BYTE	last_r;
// static	short	last_off;
	REG8	ret;

	if ((port & (~7)) != 0x0ff8) {
		return(0xff);
	}
#if 0
	if ((port &= 0xf) != 8) {
		last_r = -1;
		last_off = -1;
		timeoutwait = 4;
	}
#endif
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
			return(ret);

		case 1:									// トラック
			return(fdc.s.creg);

		case 2:									// セクタ
			return(fdc.s.rreg);

		case 3:									// データ
			if (fdc.s.motor) {
				if (fdc.s.bufdir == FDCDIR_IN) {
					fdc.s.data = fdc.s.buffer[fdc.s.bufpos];
TRACEOUT(("read %.2x %.2x [%.4x]", fdc.s.data, fdc.s.bufpos, Z80_PC));
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
}

