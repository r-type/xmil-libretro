#include	"compiler.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"vram.h"
#include	"makescrn.h"


// ---- text

void IOOUTCALL tram_o(UINT port, REG8 value) {

	UINT	addr;

	addr = LOW11(port);
	if (port < 0x3000) {
		if (tram[TRAM_ATR + addr] == value) {
			return;
		}
		if ((tram[TRAM_ATR + addr] ^ value) & (TRAMATR_Yx2 | TRAMATR_Xx2)) {
			makescrn.remakeattr = 1;
		}
		if (value & TRAMATR_BLINK) {
			makescrn.existblink = 1;
		}
		tram[TRAM_ATR + addr] = value;
	}
	else if ((port >= 0x3800) && (pccore.ROM_TYPE >= 2)) {
		if (tram[TRAM_KNJ + addr] == value) {
			return;
		}
		tram[TRAM_KNJ + addr] = value;
	}
	else {
		if (tram[TRAM_ANK + addr] == value) {
			return;
		}
		tram[TRAM_ANK + addr] = value;
	}
	makescrn.scrnflash = 1;
	if (tram[TRAM_ATR + addr] & TRAMATR_Xx2) {
		updatetmp[LOW11(addr + 1)] |= UPDATE_TVRAM;
	}
	updatetmp[addr] |= UPDATE_TVRAM;
}

REG8 IOINPCALL tram_i(UINT port) {

	UINT	addr;

	addr = LOW11(port);
	if (port < 0x3000) {
		return(tram[TRAM_ATR + addr]);
	}
	else if ((port >= 0x3800) && (pccore.ROM_TYPE >= 2)) {
		return(tram[TRAM_KNJ + addr]);
	}
	return(tram[TRAM_ANK + addr]);
}


// ---------------------------------------------------------------------- grph

void IOOUTCALL gram_o(UINT port, REG8 value) {

	UINT8	*p;

	p = crtc.e.gram + (LOW11(port) << 5) + (port >> 11);
	if (*p == value) {
		return;
	}
	*p = value;
	updatetmp[port & crtc.e.updatemask] |= crtc.e.updatebit;
	makescrn.scrnflash = 1;
}

REG8 IOINPCALL gram_i(UINT port) {

	return(crtc.e.gram[(LOW11(port) << 5) + (port >> 11)]);
}

void IOOUTCALL gram2_o(UINT port, REG8 value) {

	UINT8	*p;

	p = crtc.e.gram + (((port << 5) + (port >> 11)) & 0xffe7);
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
	updatetmp[port & crtc.e.updatemask] |= crtc.e.updatebit;
	makescrn.scrnflash = 1;
}


// ----

void vramio_reset(void) {

	ZeroMemory(gram, sizeof(gram));
	FillMemory(tram + TRAM_ATR, 0x800, 0x07);
	FillMemory(tram + TRAM_ANK, 0x800, 0x20);
	ZeroMemory(tram + TRAM_KNJ, 0x800);
	ZeroMemory(updatetmp, sizeof(updatetmp));
}

