#include	"compiler.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"makescrn.h"


// ---- 8255 PPI～

static REG8 getportb(void) {

	REG8	ret;
	UINT	v;

	ret = cmt_test(); // | cmt_read();		// THUNDER BALL

	v = pccore_getraster(NULL);
	if (v < crtc.e.dl) {
		ret |= 0x80;						// 1:DISP
	}
	if (subcpu.IBF) {
		subcpu.IBF = 0;
		ret |= 0x40;						// 1:SUB-CPU BUSY
	}
	if (subcpu.OBF) {
		ret |= 0x20;						// 1:SUB-CPU Data empty
	}
	if (memio.ram) {
		ret |= 0x10;						// 1:RAM
	}
#if 1
	if (!(v < crtc.e.vs)) {
		ret |= 0x04;						// V-SYNC
	}
#else								// ラプラステスト…VYSNCが長すぎるらしい
	if (v_cnt == crtc.e.vs) {
		ret |= 0x04;
	}
#endif
	return(ret);
}

static void setportc(REG8 dat) {

	REG8	oldc;
	UINT8	xl;

	oldc = ppi.portc;
	if (crtc.s.TXT_XL == 40) {
		oldc |= 0x40;
	}
	else {
		oldc &= ~0x40;
	}
	ppi.portc = dat;

//	cmt_write((REG8)(dat & 1));
	if ((oldc & 0x20) && (!(dat & 0x20))) {
		iocore.s.mode = 1;
	}
	xl = ((dat & 0x40)?40:80);
	if (crtc.s.TXT_XL != xl) {
		crtc.s.TXT_XL = (UINT8)xl;
//		crtc.s.GRP_XL = xl << 3;
		vrambank_patch();
		scrnallflash = 1;
	}
}


// ----

void IOOUTCALL ppi_o(UINT port, REG8 value) {

	REG8	bit;

	switch(port & 0x0f) {
		case 0:
			ppi.porta = value;
			return;

		case 1:
			ppi.portb = value;
			return;

		case 2:
			setportc(value);
			break;

		case 3:
			if (value & 0x80) {
				ppi.mode = value;
				return;
			}
			else {
				bit = 1 << ((value >> 1) & 7);
				if (value & 0x01) {
					setportc((REG8)(ppi.portc | bit));
				}
				else {
					setportc((REG8)(ppi.portc & (~bit)));
				}
			}
			break;
	}
}

REG8 IOINPCALL ppi_i(UINT port) {

	switch(port & 0x0f) {
		case 0:
		//	if (!(ppi.mode & 0x10)) {
		//		return(ppi.porta);
		//	}
			return(ppi.porta);

		case 1:
			if (!(ppi.mode & 0x02)) {
				return(ppi.portb);
			}
			return(getportb());

		case 2:
#if 1
			if (crtc.s.TXT_XL == 40) {
				ppi.portc |= 0x40;
			}
			else {
				ppi.portc &= ~0x40;
			}
#endif
			return(ppi.portc);

		case 3:
			return(ppi.mode);
	}
	return(0xff);
}


// ----

void ppi_initialize(void) {

	ppi.porta = 0x00;
	ppi.portb = 0xff;
	ppi.portc = 0xff;
	ppi.mode = 0x9b;
}

void ppi_reset(void) {

	ppi.porta = 0x00;
	ppi.portc |= 0x40;
	ppi.mode = 0x82;
}

