#include	"compiler.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"vram.h"
#include	"makescrn.h"


/* text */

void IOOUTCALL tram_atr_o(UINT port, REG8 value) {

	UINT	addr;

	addr = LOW11(port);
	if (TRAM_ATR(addr) == value) {
		return;
	}
	if ((TRAM_ATR(addr) ^ value) & (TRAMATR_Yx2 | TRAMATR_Xx2)) {
		crtc.e.remakeattr = 1;
	}
	if (value & TRAMATR_BLINK) {
		crtc.e.existblink = 1;
	}
	TRAM_ATR(addr) = value;
	crtc.e.scrnflash = 1;
	if (TRAM_ATR(addr) & TRAMATR_Xx2) {
		TRAMUPDATE(LOW11(addr + 1)) |= UPDATE_TRAM;
	}
	TRAMUPDATE(addr) |= UPDATE_TRAM;
}

void IOOUTCALL tram_ank_o(UINT port, REG8 value) {

	UINT	addr;

	addr = LOW11(port);
	if (TRAM_ANK(addr) == value) {
		return;
	}
	TRAM_ANK(addr) = value;
	crtc.e.scrnflash = 1;
	if (TRAM_ATR(addr) & TRAMATR_Xx2) {
		TRAMUPDATE(LOW11(addr + 1)) |= UPDATE_TRAM;
	}
	TRAMUPDATE(addr) |= UPDATE_TRAM;
}

void IOOUTCALL tram_knj_o(UINT port, REG8 value) {

	UINT	addr;

	addr = LOW11(port);
	if (TRAM_KNJ(addr) == value) {
		return;
	}
	TRAM_KNJ(addr) = value;
	crtc.e.scrnflash = 1;
	if (TRAM_ATR(addr) & TRAMATR_Xx2) {
		TRAMUPDATE(LOW11(addr + 1)) |= UPDATE_TRAM;
	}
	TRAMUPDATE(addr) |= UPDATE_TRAM;
}

REG8 IOINPCALL tram_atr_i(UINT port) {

	return(TRAM_ATR(LOW11(port)));
}

REG8 IOINPCALL tram_ank_i(UINT port) {

	return(TRAM_ANK(LOW11(port)));
}

REG8 IOINPCALL tram_knj_i(UINT port) {

	return(TRAM_KNJ(LOW11(port)));
}


/* grph */

void IOOUTCALL gram_o(UINT port, REG8 value) {

	UINT8	*p;

	p = crtc.e.gramacc + PORT2GRAM(port);
	if (*p == value) {
		return;
	}
	*p = value;
	TRAMUPDATE(port & crtc.e.updatemask) |= crtc.e.updatebit;
	crtc.e.scrnflash = 1;
}

REG8 IOINPCALL gram_i(UINT port) {

	return(crtc.e.gramacc[PORT2GRAM(port)]);
}

void IOOUTCALL gram2_o(UINT port, REG8 value) {

	UINT8	*p;

	p = crtc.e.gramacc + PORT2GRAM2(port);
	switch((port >> 14) & 3) {
		case 0:
			p[GRAM_B] = value;
			p[GRAM_R] = value;
			p[GRAM_G] = value;
			break;

		case 1:
			p[GRAM_R] = value;
			p[GRAM_G] = value;
			break;

		case 2:
			p[GRAM_B] = value;
			p[GRAM_G] = value;
			break;

		case 3:
			p[GRAM_B] = value;
			p[GRAM_R] = value;
			break;
	}
	TRAMUPDATE(port & crtc.e.updatemask) |= crtc.e.updatebit;
	crtc.e.scrnflash = 1;
}


/* reset */

void vramio_reset(void) {

	UINT	i;

	ZeroMemory(gram, GRAM_SIZE);
	for (i=0; i<0x800; i++) {
		TRAM_ATR(i) = 0x07;
		TRAM_ANK(i) = 0x20;
		TRAM_KNJ(i) = 0x00;
		TRAMUPDATE(i) = 0;
	}
}

