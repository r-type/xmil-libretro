#include	"compiler.h"
#include	"soundmng.h"
#include	"pccore.h"
#include	"sound.h"
#include	"sndctrl.h"


	UINT		pcmbufsize[300];
	UINT		framesoundcnt;
	OPMCH		opmch[OPMCH_MAX];
	_OPMGEN		opmgen;
	_PSGGEN		psggen;


void sndctrl_initialize(void) {

	UINT	rate;
	UINT	lastcnt;
	UINT	i;
	UINT	cnt;

	rate = xmilcfg.samplingrate;

	sound_create(rate, xmilcfg.delayms);
	opmgen_initialize(rate);
	psggen_initialize(rate);
	psggen_setvol(64);

	framesoundcnt = rate / 60;
	lastcnt = 0;
	for (i=0; i<266; i++) {
		cnt = (rate * (i+1)) / (60 * 266);
		pcmbufsize[i] = cnt - lastcnt;
		lastcnt = cnt;
	}
}

void sndctrl_deinitialize(void) {

	soundmng_stop();
	sound_destroy();
}

void sndctrl_reset(void) {

	opmgen_reset();
	psggen_reset(&psggen);

	sound_reset();
	sound_streamregist(&opmgen, (SOUNDCB)opmgen_getpcm);
	sound_streamregist(&psggen, (SOUNDCB)psggen_getpcm);
}

