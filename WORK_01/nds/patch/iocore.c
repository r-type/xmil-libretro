#include	"compiler.h"
#include	"pccore.h"
#include	"iocore.h"


	CMT			cmt;
	SIO			sio;


// ----

#if !defined(USE_ARMROUTINE)
static void IOOUTCALL dummy_out(UINT uPort, REG8 cValue)
{
	(void)uPort;
	(void)cValue;
}

static REG8 IOINPCALL dummy_inp(UINT uPort)
{
	(void)uPort;
	return 0xff;
}
#else	// !defined(USE_ARMROUTINE)
void IOOUTCALL dummy_out(UINT uPort, REG8 cValue);
REG8 IOINPCALL dummy_inp(UINT uPort);
#endif	// !defined(USE_ARMROUTINE)


// ----

static void IOOUTCALL port1fxx_o(UINT uPort, REG8 cValue)
{
	REG8 lsb;
	REG8 msb6;

	lsb = (UINT8)uPort;
	if (lsb == 0xd0)
	{
		scrn_o(uPort, cValue);
		return;
	}
	if (lsb < 0x80)
	{
		return;
	}
	msb6 = lsb & (~3);
	if (lsb < 0x90)
	{
		dmac_o(uPort, cValue);
		return;
	}
	if (msb6 == 0x90)
	{
		sio_o(uPort, cValue);
		return;
	}
	if ((msb6 == 0xa0) || (msb6 == 0xa8))
	{
		ctc_o(uPort, cValue);
		return;
	}
	if (lsb == 0xe0)
	{
		blackctrl_o(uPort, cValue);
		return;
	}
#if defined(SUPPORT_TURBOZ)
	if (pccore.ROM_TYPE >= 3)
	{
		if (lsb == 0xb0)
		{
			extpal_o(uPort, cValue);
			return;
		}
		if (lsb == 0xc0)
		{
			exttextdisp_o(uPort, cValue);
			return;
		}
		if (lsb == 0xc5)
		{
			extgrphpal_o(uPort, cValue);
			return;
		}
		if ((lsb >= 0xb9) && (lsb < 0xc0))
		{
			exttextpal_o(uPort, cValue);
			return;
		}
	}
#endif	// defined(SUPPORT_TURBOZ)
}

static REG8 IOINPCALL port1fxx_i(UINT uPort)
{
	REG8 lsb;
	REG8 msb6;

	lsb = (UINT8)uPort;
	if (lsb < 0x80)
	{
		return 0xff;
	}
	msb6 = lsb & (~3);
	if (lsb < 0x90)
	{
		return dmac_i(uPort);
	}
	if (msb6 == 0x90)
	{
		return sio_i(uPort);
	}
	if ((msb6 == 0xa0) || (msb6 == 0xa8))
	{
		return ctc_i(uPort);
	}
	if (lsb == 0xd0)
	{
		return scrn_i(uPort);
	}
	if (lsb >= 0xf0)
	{
		return dipsw_i(uPort);
	}
#if defined(SUPPORT_TURBOZ)
	if (pccore.ROM_TYPE >= 3)
	{
		if (lsb == 0xb0)
		{
			return extpal_i(uPort);
		}
		if (lsb == 0xc0)
		{
			return exttextdisp_i(uPort);
		}
		if (lsb == 0xc5)
		{
			return extgrphpal_i(uPort);
		}
		if ((lsb >= 0xb9) && (lsb < 0xc0))
		{
			return exttextpal_i(uPort);
		}
		if (lsb == 0xe0)
		{
			return(blackctrl_i(uPort);
		}
	}
#endif	// defined(SUPPORT_TURBOZ)

	return 0xff;
}


// ----

static const IOINP s_fnDefInp[0x20] =
{
	dummy_inp,			dummy_inp,
	dummy_inp,			dummy_inp,
	dummy_inp,			dummy_inp,
	dummy_inp,			dummy_inp,

	dummy_inp,			dummy_inp,
	dummy_inp,			dummy_inp,
	dummy_inp,			dummy_inp,
	cgrom_i,			fdc_i,

	dummy_inp,			dummy_inp,
	dummy_inp,			dummy_inp,
	pcg_i,				pcg_i,
	pcg_i,				pcg_i,

	dummy_inp,			subcpu_i,
	ppi_i,				sndboard_psgsta,
	dummy_inp,			dummy_inp,
	dummy_inp,			dummy_inp
};

static const IOOUT s_fnDefOut[0x20] =
{
	dummy_out,			dummy_out,
	dummy_out,			dummy_out,
	dummy_out,			dummy_out,
	dummy_out,			dummy_out,

	dummy_out,			dummy_out,
	dummy_out,			dummy_out,
	dummy_out,			dummy_out,
	cgrom_o,			fdc_o,

	palette_o,			palette_o,
	palette_o,			ply_o,
	dummy_out,			pcg_o,
	pcg_o,				pcg_o,

	crtc_o,				subcpu_o,
	ppi_o,				sndboard_psgdat,
	sndboard_psgreg,	memio_rom,
	memio_ram,			dummy_out
};

typedef void (*INITFN)(void);

static const INITFN s_fnInit[] =
{
	cgrom_reset,	cmt_reset,		crtc_reset,
	ctc_reset,		dmac_reset,		fdc_reset,
	memio_reset,	pcg_reset,		ppi_reset,
	sio_reset,		sndboard_reset,	subcpu_reset,
	vramio_reset
};


// ----

void iocore_reset(void)
{
	UINT i;

	ZeroMemory(&iocore, sizeof(iocore));
	CopyMemory(iocore.f.inpfn, s_fnDefInp, sizeof(s_fnDefInp));
	CopyMemory(iocore.f.outfn, s_fnDefOut, sizeof(s_fnDefOut));
	for (i=0; i<0x10; i++) {
		iocore.f.inpfn[i+0x20] = tram_atr_i;
		iocore.f.inpfn[i+0x30] = tram_ank_i;
		iocore.f.outfn[i+0x20] = tram_atr_o;
		iocore.f.outfn[i+0x30] = tram_ank_o;
	}
	if (pccore.ROM_TYPE >= 2) {
#if defined(SUPPORT_BANKMEM)
		iocore.f.inpfn[0x0b] = memio_bank_i;
		iocore.f.outfn[0x0b] = memio_bank_o;
#endif
		iocore.f.inpfn[0x1f] = port1fxx_i;
		iocore.f.outfn[0x1f] = port1fxx_o;
		for (i=0; i<8; i++) {
			iocore.f.inpfn[i+0x38] = tram_knj_i;
			iocore.f.outfn[i+0x38] = tram_knj_o;
		}
	}
#if defined(SUPPORT_TURBOZ)
	if (pccore.ROM_TYPE >= 3) {
		iocore.f.inpfn[0x10] = palette_i;
		iocore.f.inpfn[0x11] = palette_i;
		iocore.f.inpfn[0x12] = palette_i;
		iocore.f.inpfn[0x13] = ply_i;
	}
#endif	// defined(SUPPORT_TURBOZ)
#if	defined(SUPPORT_ROMEO2)
#if defined(SUPPORT_TURBOZ) || defined(SUPPORT_OPM)
	if (pccore.SOUND_SW) {
		iocore.f.inpfn[0x07] = opm_i;
		iocore.f.outfn[0x07] = opm_o;
	}
#endif	// defined(SUPPORT_TURBOZ) || defined(SUPPORT_OPM)
#endif	// defined(SUPPORT_ROMEO2)
	for (i=0; i<NELEMENTS(s_fnInit); i++)
	{
		(s_fnInit[i])();
	}
}


// ----

#if !defined(USE_ARMROUTINE)
void IOOUTCALL iocore_out(UINT uPort, REG8 cValue)
{
	UINT msb;

	msb = uPort >> 8;
	if (iocore.s.mode)
	{
		gram2_o(uPort, cValue);
	}
	else if (msb >= 0x40)
	{
		gram_o(uPort, cValue);
	}
	else
	{
		(*iocore.f.outfn[msb])(uPort, cValue);
	}
}

REG8 IOINPCALL iocore_inp(UINT uPort)
{
	UINT msb;

	msb = uPort >> 8;
	iocore.s.mode = 0;
	if (msb >= 0x40)
	{
		return gram_i(uPort);
	}
	else
	{
		return (*iocore.f.inpfn[msb])(uPort);
	}
}
#endif	// !defined(USE_ARMROUTINE)

