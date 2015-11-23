/**
 * @file	sndctrl.c
 * @brief	Implementation of the sound
 */

#include "compiler.h"
#include "sndctrl.h"
#if !defined(DISABLE_SOUND)
#include "pccore.h"
#include "opmgen.h"
#include "psggen.h"
#include "soundmng.h"

/**
 * Initialize
 */
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

/**
 * Deinitialize
 */
void sndctrl_deinitialize(void)
{
	soundmng_stop();
	sound_destroy();
}
#endif
