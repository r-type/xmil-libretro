/**
 * @file	sndboard.h
 * @brief	Implementation of the sound boards
 */

#include "compiler.h"
#include "joymng.h"
#include "pccore.h"
#include "iocore.h"
#include "keystat.h"
#include "sound.h"
#include "sndctrl.h"

static UINT8 s_rapids;

#if defined(SUPPORT_TURBOZ) || defined(SUPPORT_OPM)
void IOOUTCALL opm_o(UINT port, REG8 dat)
{
	REG8 lsb;

	lsb = (UINT8)port;
	if (lsb == 0x00)					/* 0700 */
	{
		g_opm.s.addr = (UINT8)dat;
	}
	else if (lsb == 0x01)				/* 0701 */
	{
		opm_writeRegister(&g_opm, g_opm.s.addr, dat);
	}
	else if ((lsb & (~3)) == 0x04)		/* 0704-0707 */
	{
		ctc_o(port, dat);
	}
}

REG8 IOINPCALL opm_i(UINT port)
{
	REG8 lsb;

	lsb = (UINT8)port;
	if ((lsb & (~1)) == 0x00)			/* 0700/0701 */
	{
		return 0x00;											/* É[ÉäÉAÅ[Éh */
	}
	else if ((lsb & (~3)) == 0x04)		/* 0704-0707 */
	{
		return ctc_i(port);
	}
	else
	{
		return 0xff;
	}
}
#endif


void IOOUTCALL sndboard_psgreg(UINT port, REG8 dat)
{
	g_psg.s.addr = dat;
	(void)port;
}

void IOOUTCALL sndboard_psgdat(UINT port, REG8 dat)
{
	psg_writeRegister(&g_psg, g_psg.s.addr, dat);
	(void)port;
}

REG8 IOINPCALL sndboard_psgsta(UINT port)
{
	REG8 nAddress;
	REG8 ret;

	nAddress = g_psg.s.addr;
	if (nAddress < 0x0e)
	{
		return psg_readRegister(&g_psg, nAddress);
	}
	else if (nAddress < 0x10)
	{
		ret = 0xff;
		if (nAddress == (xmilcfg.KEY_MODE + 0x0d))
		{
			ret &= keystat_getjoy();
		}
		if (nAddress == 0x0e)
		{
			ret &= joymng_getstat();
		}
		if (xmilcfg.BTN_RAPID)
		{
			s_rapids ^= 0x60;
			ret |= s_rapids;
		}
		if (xmilcfg.BTN_MODE)
		{
			ret = (ret & (~0x60)) | ((ret & 0x40) >> 1) | ((ret & 0x20) << 1);
		}
		return ret;
	}
	else
	{
		(void)port;
		return 0xff;
	}
}

/* reset */

void sndboard_reset(void)
{
#if defined(SUPPORT_TURBOZ) || defined(SUPPORT_OPM)
	UINT8 cCaps;

	cCaps = (pccore.SOUND_SW) ? (OPM_HAS_OPM | OPM_X1F) : 0;
	opm_reset(&g_opm, cCaps);
	opm_bind(&g_opm);
#endif
	psg_reset(&g_psg, PSG_HAS_PSG | PSG_X1F);
	psg_bind(&g_psg);
}

#if !defined(DISABLE_SOUND)
void sndboard_update(void)
{
#if defined(SUPPORT_TURBOZ) || defined(SUPPORT_OPM)
	opm_restore(&g_opm);
#endif
	psg_restore(&g_psg);
}
#endif
