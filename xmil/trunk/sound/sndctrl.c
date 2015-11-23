/**
 * @file	sndctrl.c
 * @brief	Implementation of the sound
 */

#include "compiler.h"
#include "sndctrl.h"
#if !defined(DISABLE_SOUND)
#include "soundmng.h"
#include "pccore.h"
#include "sound.h"

#if defined(SUPPORT_TURBOZ) || defined(SUPPORT_OPM)
	OPM g_opm;
#endif
	PSG g_psg;

void sndctrl_initialize(void)
{
	UINT	rate;

	rate = xmilcfg.samplingrate;
	sound_create(rate, xmilcfg.delayms);
#if defined(SUPPORT_TURBOZ) || defined(SUPPORT_OPM)
	opmgen_initialize(rate);
	opmgen_setvol(xmilcfg.vol_fm);
#endif
	psggen_initialize(rate);
	psggen_setvol(xmilcfg.vol_ssg);
}

void sndctrl_deinitialize(void)
{
	soundmng_stop();
	sound_destroy();
}

void sndctrl_reset(void)
{
}
#endif
