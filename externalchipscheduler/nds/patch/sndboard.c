#include "compiler.h"
#include "pccore.h"
#include "iocore.h"
#include "z80core.h"
#include "sound/nds9psg.h"
#if defined(SUPPORT_ROMEO2)
#include <nds/registers_alt.h>
#include "romeo2.h"
#include "romeo2_x1.h"
#endif	// defined(SUPPORT_ROMEO2)


// --- psg

#if !defined(USE_ARMROUTINE)
void IOOUTCALL sndboard_psgreg(UINT uPort, REG8 cValue)
{
	nds9psg.cAddr = cValue;
	(void)uPort;
}

void IOOUTCALL sndboard_psgdat(UINT uPort, REG8 cValue)
{
	nds9psg_setreg(&nds9psg, nds9psg.cAddr, cValue);
	(void)uPort;
}

REG8 IOINPCALL sndboard_psgsta(UINT uPort)
{
	if (nds9psg.cAddr < 0x10)
	{
		// ’¼Ú‚æ‚Þ‚æ‚Þ
		return nds9psg.reg.b[nds9psg.cAddr];
	}
	else
	{
		(void)uPort;
		return 0xff;
	}
}
#endif	// !defined(USE_ARMROUTINE)



// ---- romeo2

#if defined(SUPPORT_ROMEO2)

#if defined(ROMEO2_WFIFO)
static SINT32 s_nFifoLastClock;
#else	// defined(ROMEO2_WFIFO)
static unsigned char s_cRomeo2Addr;
#endif	// defined(ROMEO2_WFIFO)

static void IOOUTCALL romeo2_psgdat(UINT uPort, REG8 cValue)
{
	const REG8 cReg = nds9psg.cAddr;
	if (cReg >= 14)
	{
		return;
	}

#if defined(ROMEO2_WFIFO)
	// sync sound write
	const SINT32 nCurClock = CPU_CLOCKCOUNT - iocore.e.framestartclock;
	const SINT32 nPast = nCurClock - s_nFifoLastClock;
	if (nPast > 0)
	{
		s_nFifoLastClock = nCurClock;
		x1sound_sw_sync(nPast);
	}
	x1sound_ssg_write(0, cReg);   // 1cxx
	x1sound_ssg_write(1, cValue); // 1bxx
#else	// defined(ROMEO2_WFIFO)
	ROMEO2SsgWriteReg(0, cReg, cValue);
#endif	// defined(ROMEO2_WFIFO)
	(void)uPort;
}

#if defined(SUPPORT_OPM)
void IOOUTCALL opm_o(UINT uPort, REG8 cDat)
{
	UINT uLsb = uPort & 0xff;
	if (uLsb == 0x00)		/* 0700 */
	{
#if defined(ROMEO2_WFIFO)
		x1sound_fm_write(0, cDat);
#else	// defined(ROMEO2_WFIFO)
		s_cRomeo2Addr = cDat;
#endif	// defined(ROMEO2_WFIFO)
	}
	else if (uLsb == 0x01)	/* 0701 */
	{
#if defined(ROMEO2_WFIFO)
		// sync sound write
		const SINT32 nCurClock = CPU_CLOCKCOUNT - iocore.e.framestartclock;
		const SINT32 nPast = nCurClock - s_nFifoLastClock;
		if (nPast > 0)
		{
			s_nFifoLastClock = nCurClock;
			x1sound_sw_sync(nPast);
		}
		x1sound_fm_write(1, cDat);
#else	// defined(ROMEO2_WFIFO)
		ROMEO2FmmWriteReg(0, s_cRomeo2Addr, cDat);
#endif	// defined(ROMEO2_WFIFO)

	}
	else if ((uLsb & (~3)) == 0x04)
	{
		/* 0704-0707 */
		ctc_o(uPort, cDat);
	}
}

REG8 IOINPCALL opm_i(UINT uPort)
{
	UINT uLsb = uPort & 0xff;
	if (uLsb == 0x00)					/* 0700 */
	{
		return 0x00;
	}
	else if (uLsb == 0x01)				/* 0701 */
	{
		return 0x00;
	}
	else if ((uLsb & (~3)) == 0x04)		/* 0704-0707 */
	{
		return ctc_i(uPort);
	}
	else
	{
		return 0xff;
	}
}
#endif	// defined(SUPPORT_OPM)
#endif	// defined(SUPPORT_ROMEO2)


// ----

void sndboard_initialize(void)
{
}

void sndboard_deinitialize(void)
{
}

void sndboard_reset()
{
#if defined(SUPPORT_ROMEO2)
	if (xmilcfg.SOUND_SW)
	{
		romeo2_x1_setup();

		iocore.f.outfn[0x1b] = romeo2_psgdat;
	}
#endif	// defined(SUPPORT_ROMEO2)

	nds9psg_reset(&nds9psg);
}

