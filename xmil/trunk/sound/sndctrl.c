#include	"compiler.h"
#include	"soundmng.h"
#include	"pccore.h"
#include	"sound.h"
#include	"sndctrl.h"


	OPMCH		opmch[OPMCH_MAX];
	_OPMGEN		opmgen;
	_PSGGEN		psggen;


void sndctrl_initialize(void) {

	UINT	rate;

	rate = xmilcfg.samplingrate;
	sound_create(rate, xmilcfg.delayms);
	opmgen_initialize(rate);
	psggen_initialize(rate);
	opmgen_setvol(xmilcfg.vol_fm);
	psggen_setvol(xmilcfg.vol_ssg);
}

void sndctrl_deinitialize(void) {

	soundmng_stop();
	sound_destroy();
}

void sndctrl_reset(void) {

	opmgen_reset();
	psggen_reset(&psggen);

	sound_streamregist(&opmgen, (SOUNDCB)opmgen_getpcm);
	sound_streamregist(&psggen, (SOUNDCB)psggen_getpcm);
}

