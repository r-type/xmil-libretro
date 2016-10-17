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
#include "generic/keydisp.h"

/* OPM */

#if defined(SUPPORT_TURBOZ) || defined(SUPPORT_OPM)

#if defined(SUPPORT_OPMx2)
	OPM g_opm[2];
#else	/* defined(SUPPORT_OPMx2) */
	OPM g_opm[1];
#endif	/* defined(SUPPORT_OPMx2) */

void IOOUTCALL opm_o(UINT port, REG8 dat)
{
	REG8 lsb;

	lsb = (UINT8)port;
	if (lsb == 0x00)					/* 0700 */
	{
		g_opm[0].s.addr = (UINT8)dat;
	}
	else if (lsb == 0x01)				/* 0701 */
	{
		opm_writeRegister(&g_opm[0], g_opm[0].s.addr, dat);
	}
	else if ((lsb & (~3)) == 0x04)		/* 0704-0707 */
	{
		ctc_o(port, dat);
	}
#if defined(SUPPORT_OPMx2)
	else if (((lsb & (~7)) == 8) && (pccore.SOUND_SW >= 2))
	{
		if (lsb == 0x08)					/* 0708 */
		{
			g_opm[1].s.addr = (UINT8)dat;
		}
		else if (lsb == 0x09)				/* 0709 */
		{
			opm_writeRegister(&g_opm[1], g_opm[1].s.addr, dat);
		}
		else if ((lsb & (~3)) == 0x0c)		/* 070C-070F */
		{
			ctc_o(port - 8, dat);
		}
	}
#endif	/* defined(SUPPORT_OPMx2) */
}

REG8 IOINPCALL opm_i(UINT port)
{
	REG8 lsb;

	lsb = (UINT8)port;
	if ((lsb & (~1)) == 0x00)			/* 0700/0701 */
	{
		return 0x00;											/* ƒ[ƒŠƒA[ƒh */
	}
	else if ((lsb & (~3)) == 0x04)		/* 0704-0707 */
	{
		return ctc_i(port);
	}
#if defined(SUPPORT_OPMx2)
	else if (((lsb & (~7)) == 0x08) && (pccore.SOUND_SW >= 2))
	{
		if ((lsb & (~1)) == 0x08)
		{
			return 0x00;
		}
		else if ((lsb & (~3)) == 0x0c)
		{
			return ctc_i(port - 8);
		}
	}
#endif	/* defined(SUPPORT_OPMx2) */
	return 0xff;
}
#endif



/* PSG */

static UINT8 s_rapids;
	PSG g_psg;

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

/**
 * Initialize
 */
void sndboard_initialize(void)
{
#if defined(SUPPORT_TURBOZ) || defined(SUPPORT_OPM)
	UINT i;

	for (i = 0; i < NELEMENTS(g_opm); i++)
	{
		opm_construct(&g_opm[i]);
	}
#endif	/* defined(SUPPORT_TURBOZ) || defined(SUPPORT_OPM) */
	psg_construct(&g_psg);
}

void sndboard_deinitialize(void)
{
#if defined(SUPPORT_TURBOZ) || defined(SUPPORT_OPM)
	UINT i;

	for (i = 0; i < NELEMENTS(g_opm); i++)
	{
		opm_destruct(&g_opm[i]);
	}
#endif
	psg_destruct(&g_psg);
}

/**
 * reset
 */
void sndboard_reset(void)
{
#if defined(SUPPORT_TURBOZ) || defined(SUPPORT_OPM)
	UINT8 cCaps;
#endif	/* defined(SUPPORT_TURBOZ) || defined(SUPPORT_OPM) */

	keydisp_reset();

#if defined(SUPPORT_TURBOZ) || defined(SUPPORT_OPM)
	cCaps = (pccore.SOUND_SW) ? (OPM_HAS_OPM | OPM_X1F) : 0;
	opm_reset(&g_opm[0], cCaps);
	opm_bind(&g_opm[0]);

#if defined(SUPPORT_OPMx2)
	cCaps = (pccore.SOUND_SW >= 2) ? OPM_HAS_OPM : 0;
	opm_reset(&g_opm[1], cCaps);
	opm_bind(&g_opm[1]);
#endif	/* defined(SUPPORT_OPMx2) */
#endif	/* defined(SUPPORT_TURBOZ) || defined(SUPPORT_OPM) */

	psg_reset(&g_psg, PSG_HAS_PSG | PSG_X1F);
	psg_bind(&g_psg);
}

void sndboard_update(void)
{
#if defined(SUPPORT_TURBOZ) || defined(SUPPORT_OPM)
	UINT i;

	for (i = 0; i < NELEMENTS(g_opm); i++)
	{
		opm_restore(&g_opm[i]);
	}
#endif
	psg_restore(&g_psg);
}
