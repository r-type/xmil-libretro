#include	"compiler.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"x1_io.h"
#include	"x1_crtc.h"
#include	"vram.h"
#include	"draw.h"


		BYTE	*curvram;
		BYTE	curupdt;
		DWORD	updatemsk = 0x7ff;


// ---- text

void IOOUTCALL tram_o(UINT port, REG8 value) {

	UINT	addr;

	addr = LOW11(port);
	if (port < 0x3000) {
		if (tram[TRAM_ATR + addr] == value) {
			return;
		}
		if ((tram[TRAM_ATR + addr] ^ value) & (X1ATR_Yx2 | X1ATR_Xx2)) {
			doubleatrchange = 1;
		}
		if (value & X1ATR_BLINK) {
			blinkflag = 1;
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
	scrnflash = 1;
	if (tram[TRAM_ATR + addr] & X1ATR_Xx2) {
		updatetmp[addr + 1] |= UPDATE_TVRAM;
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

	UINT	addr;

	addr = (LOW11(port) << 5) + (port >> 11);
	if (curvram[addr] == value) {
		return;
	}
	curvram[addr] = value;
	updatetmp[port & updatemsk] |= curupdt;
	scrnflash = 1;
}

REG8 IOINPCALL gram_i(UINT port) {

	return(curvram[(LOW11(port) << 5) + (port >> 11)]);
}

void IOOUTCALL gram2_o(UINT port, REG8 value) {

	UINT8	*p;

	p = curvram + (((port << 5) | (port >> 11)) & 0xffe7);
	switch(port & 0xc000) {
		case 0x0000:
			p[PLANE_B] = value;
			p[PLANE_R] = value;
			p[PLANE_G] = value;
			break;

		case 0x4000:
			p[PLANE_R] = value;
			p[PLANE_G] = value;
			break;

		case 0x8000:
			p[PLANE_B] = value;
			p[PLANE_G] = value;
			break;

		case 0xc000:
			p[PLANE_B] = value;
			p[PLANE_R] = value;
			break;
	}

	updatetmp[port & updatemsk] |= curupdt;
	scrnflash = 1;
}


// ----

void vramio_reset(void) {

	ZeroMemory(GRP_RAM, 0x20000);
	ZeroMemory(tram, 0x01800);
	memset(tram + TRAM_ATR, 0x07, 0x800);
	memset(tram + TRAM_ANK, 0x20, 0x800);
	ZeroMemory(updatetmp, sizeof(updatetmp));
	curvram = &GRP_RAM[GRAM_BANK0];
	curupdt = UPDATE_VRAM0;
}

