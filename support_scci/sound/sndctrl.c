#include	"compiler.h"
#if !defined(DISABLE_SOUND)
#include	"soundmng.h"
#include	"pccore.h"
#include	"sound.h"
#include	"sndctrl.h"


#if defined(SUPPORT_TURBOZ) || defined(SUPPORT_OPM)
	OPMCH		opmch[OPMCH_MAX];
	_OPMGEN		opmgen;
#endif
	_PSGGEN		psggen;


void sndctrl_initialize(void) {

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

void sndctrl_deinitialize(void) {

	soundmng_stop();
	sound_destroy();
}

void sndctrl_reset(void) {

#if defined(SUPPORT_TURBOZ) || defined(SUPPORT_OPM)
	opmgen_reset();
	sound_streamregist(&opmgen, (SOUNDCB)opmgen_getpcm);
#endif
	psggen_reset(&psggen);
	sound_streamregist(&psggen, (SOUNDCB)psggen_getpcm);
}

#endif

