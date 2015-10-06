#include	"compiler.h"
#include	"mousemng.h"
#include	"z80core.h"
#include	"pccore.h"
#include	"iocore.h"


/* sw ... 割り込み 1=かける 0=かけない */
static void fifo_w(SIOFIFO *sf, REG8 data, REG8 sw) {

	UINT	off;

	off = sf->top + sf->cnt;
	if (off >= SIOFIFO_BUFMAX) {
		off -= SIOFIFO_BUFMAX;
	}
	sf->cnt++;
	if (sf->cnt >= SIOFIFO_BUFMAX) {
		sf->cnt = 0;
	}
	sf->buf[off] = (UINT8)data;
	sf->flg[off] = (UINT8)sw;
}

static REG8 fifo_r(SIOFIFO *sf) {

	REG8	data;

	if (!sf->cnt) {
		return(0);
	}
	sf->cnt--;
	data = sf->buf[sf->top++];
	if (sf->top >= SIOFIFO_BUFMAX) {
		sf->top = 0;
	}
	return(data);
}

#if 0
static REG8 fifo_chk(SIOFIFO *sf) {

	REG8	ret;

	if (sf->top != sf->fno) {
		return(0);
	}
	ret = sf->flg[sf->fno++];
	if (sf->fno >= SIOFIFO_BUFMAX) {
		sf->fno = 0;
	}
	return(ret);
}

static void x1_sio0(SIOCH *ch) {

	if ((!(ch->wr[1] & 0x18)) || (!(ch->wr[3] & 0x01)) || (!ch->RxD.cnt)) {
		return;
	}
	if (fifo_chk(&ch->RxD)) {
		ch->rx_int = 1;
	}
}

static BRESULT x1_sio1(SIOCH *ch) {

	REG8	vct;

	vct = (REG8)(ch->wr[2] & 0xf1);
	if ((ch->tx_int) && (!Z80_ABLEINTERRUPT())) {
		ch->tx_int = 0;
		Z80_INT(vct);
		return(TRUE);
	}
	if ((ch->rx_int) && (!Z80_ABLEINTERRUPT())) {
		ch->rx_int = 0;
		Z80_INT((REG8)(vct + 0x04));
		return(TRUE);
	}
	return(FALSE);
}

void sio_int(void) {

	x1_sio0(sio.ch + SIOCH_A);
	x1_sio0(sio.ch + SIOCH_B);
	if (x1_sio1(sio.ch + SIOCH_A)) return;
	if (x1_sio1(sio.ch + SIOCH_B)) return;
}
#endif

static void mouse_read(SIOCH *ch) {

	SINT16	mx;
	SINT16	my;
	UINT8	dat0, dat1, dat2;

	dat0 = mousemng_getstat(&mx, &my, TRUE) & 3;

	if (mx > 127) {
		dat1 = 0x7f;
		dat0 |= 0x10;
	}
	else if (mx < -127) {
		dat1 = 0x80;
		dat0 |= 0x20;
	}
	else {
		dat1 = (UINT8)mx;
	}
	if (my > 127) {
		dat2 = 0x7f;
		dat0 |= 0x40;
	}
	else if (my < -127) {
		dat2 = 0x80;
		dat0 |= 0x80;
	}
	else {
		dat2 = (UINT8)my;
	}
	if ((ch->wr[1] & 0x18) == 0x08) {
		fifo_w(&ch->RxD, dat0, 1);		/* 最初の１バイトだけ受信割り込み */
		fifo_w(&ch->RxD, dat1, 0);
		fifo_w(&ch->RxD, dat2, 0);
	}
	else {
		fifo_w(&ch->RxD, dat0, 1);		/* １バイト毎に割り込み */
		fifo_w(&ch->RxD, dat1, 1);
		fifo_w(&ch->RxD, dat2, 1);
	}
}


/* ---- */

void IOOUTCALL sio_o(UINT port, REG8 value) {

	switch(port) {
		case 0x1f90:				/* CH-A ﾃﾞｰﾀ */
			if (sio.ch[SIOCH_A].wr[3] & 0x08) {
				fifo_w(&sio.ch[SIOCH_A].TxD, value, 0);
			}
			break;

		case 0x1f91:				/* CH-A ｺﾝﾄﾛｰﾙ */
			sio.ch[SIOCH_A].wr[sio.ch[SIOCH_A].num] = value;
			if (sio.ch[SIOCH_A].num) {
				sio.ch[SIOCH_A].num = 0;
			}
			else if (value & 0xf1) {
				sio.ch[SIOCH_A].num = (value & 7);
			}
			break;
#if 0
		case 0x1f92:				/* CH-B ﾃﾞｰﾀ   (Mouse) */
			if (CH_B.wr[3] & 0x08) {
				fifo_w(&CH_B.T_D, value, 0);
			}
			break;
#endif
		case 0x1f93:				/* CH-B ｺﾝﾄﾛｰﾙ (Mouse) */
			if (sio.ch[SIOCH_B].num == 5) {
				/* RTS(WR5:BIT1)を０から１へ変化させると */
				/* マウスから３バイトのデータが送られてくる */
				if (!(sio.ch[SIOCH_B].wr[5] & 2) && (value & 2)) {
					mouse_read(sio.ch + SIOCH_B);
				}
			}
			sio.ch[SIOCH_B].wr[sio.ch[SIOCH_B].num] = value;
			if (sio.ch[SIOCH_B].num) {
				sio.ch[SIOCH_B].num = 0;
			}
			else {
				sio.ch[SIOCH_B].num = value & 7;
			}
			break;
	}
}

REG8 IOINPCALL sio_i(UINT port) {

	REG8	ret;

	ret = 0;
	switch(port) {
		case 0x1f90:				/* CH-A ﾃﾞｰﾀ */
			if (sio.ch[SIOCH_A].RxD.cnt) {
				return(fifo_r(&sio.ch[SIOCH_A].RxD));
			}
			break;

		case 0x1f91:				/* CH-A ｺﾝﾄﾛｰﾙ */
			if (sio.ch[SIOCH_A].num == 0) {
				if (sio.ch[SIOCH_A].RxD.cnt != 0) {
					ret |= 1;
				}
				if (sio.ch[SIOCH_A].TxD.cnt == 0) {
					ret |= 4;
				}
			}
			break;

		case 0x1f92:				/* CH-B ﾃﾞｰﾀ   (Mouse) */
			if (sio.ch[SIOCH_B].RxD.cnt) {
				return(fifo_r(&sio.ch[SIOCH_B].RxD));
			}
			break;

		case 0x1f93:				/* CH-B ｺﾝﾄﾛｰﾙ (Mouse) */
			if (sio.ch[SIOCH_B].num == 0) {		/* ?? */
				if (sio.ch[SIOCH_B].RxD.cnt != 0) {
					ret |= 1;
				}
				if (sio.ch[SIOCH_B].TxD.cnt == 0) {
					ret |= 4;
				}
			}
			break;
	}
	return(ret);
}


/* reset */

void sio_reset(void) {

	ZeroMemory(&sio, sizeof(sio));
}

