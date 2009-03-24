#include	"compiler.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"vram.h"
#include	"makescrn.h"


// ---- text

void IOOUTCALL tram_atr_o(UINT uPort, REG8 cValue)
{
	UINT uAddr;

	uAddr = LOW11(uPort);
	if (TRAM_ATR(uAddr) == cValue)
	{
		return;
	}
	if ((TRAM_ATR(uAddr) ^ cValue) & (TRAMATR_Yx2 | TRAMATR_Xx2))
	{
		crtc.e.remakeattr = 1;
	}
	if (cValue & TRAMATR_BLINK)
	{
		crtc.e.existblink = 1;
	}
	TRAM_ATR(uAddr) = cValue;
	crtc.e.scrnflash = 1;
	if (TRAM_ATR(uAddr) & TRAMATR_Xx2)
	{
		TRAMUPDATE(LOW11(uAddr + 1)) |= UPDATE_TRAM;
	}
	TRAMUPDATE(uAddr) |= UPDATE_TRAM;
}

#if !defined(USE_ARMROUTINE)
void IOOUTCALL tram_ank_o(UINT uPort, REG8 cValue)
{
	UINT uAddr;

	uAddr = LOW11(uPort);
	if (TRAM_ANK(uAddr) == cValue)
	{
		return;
	}
	TRAM_ANK(uAddr) = cValue;
	crtc.e.scrnflash = 1;
	if (TRAM_ATR(uAddr) & TRAMATR_Xx2)
	{
		TRAMUPDATE(LOW11(uAddr + 1)) |= UPDATE_TRAM;
	}
	TRAMUPDATE(uAddr) |= UPDATE_TRAM;
}
#endif	// !defined(USE_ARMROUTINE)

void IOOUTCALL tram_knj_o(UINT uPort, REG8 cValue)
{
	UINT uAddr;

	uAddr = LOW11(uPort);
	if (TRAM_KNJ(uAddr) == cValue)
	{
		return;
	}
	TRAM_KNJ(uAddr) = cValue;
	crtc.e.scrnflash = 1;
	if (TRAM_ATR(uAddr) & TRAMATR_Xx2)
	{
		TRAMUPDATE(LOW11(uAddr + 1)) |= UPDATE_TRAM;
	}
	TRAMUPDATE(uAddr) |= UPDATE_TRAM;
}

REG8 IOINPCALL tram_atr_i(UINT uPort)
{
	return TRAM_ATR(LOW11(uPort));
}

REG8 IOINPCALL tram_ank_i(UINT uPort)
{
	return TRAM_ANK(LOW11(uPort));
}

REG8 IOINPCALL tram_knj_i(UINT uPort)
{
	return TRAM_KNJ(LOW11(uPort));
}


// ---------------------------------------------------------------------- grph

#if !defined(USE_ARMROUTINE)
void IOOUTCALL gram_o(UINT uPort, REG8 cValue)
{
	UINT8 *p;

	p = crtc.e.gramacc + PORT2GRAM(uPort);
	if (*p == cValue)
	{
		return;
	}
	*p = cValue;
	TRAMUPDATE(uPort & crtc.e.updatemask) |= crtc.e.updatebit;
	crtc.e.scrnflash = 1;
}

REG8 IOINPCALL gram_i(UINT uPort)
{
	return crtc.e.gramacc[PORT2GRAM(uPort)];
}

void IOOUTCALL gram2_o(UINT uPort, REG8 cValue)
{
	UINT8 *p;

	p = crtc.e.gramacc + PORT2GRAM2(uPort);
	switch((uPort >> 14) & 3)
	{
		case 0:
			p[GRAM_B] = cValue;
			p[GRAM_R] = cValue;
			p[GRAM_G] = cValue;
			break;

		case 1:
			p[GRAM_R] = cValue;
			p[GRAM_G] = cValue;
			break;

		case 2:
			p[GRAM_B] = cValue;
			p[GRAM_G] = cValue;
			break;

		case 3:
			p[GRAM_B] = cValue;
			p[GRAM_R] = cValue;
			break;
	}
	TRAMUPDATE(uPort & crtc.e.updatemask) |= crtc.e.updatebit;
	crtc.e.scrnflash = 1;
}
#endif	// !defined(USE_ARMROUTINE)


// ----

void vramio_reset(void)
{
	UINT i;

	ZeroMemory(gram + GRAM_BANK0, 0xc000);
	ZeroMemory(gram + GRAM_BANK1, 0xc000);
	for (i=0; i<0x800; i++)
	{
		TRAM_ATR(i) = 0x07;
		TRAM_ANK(i) = 0x20;
		TRAM_KNJ(i) = 0x00;
		TRAMUPDATE(i) = 0;
	}
}

