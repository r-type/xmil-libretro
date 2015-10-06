#include	"compiler.h"
#if !defined(DISABLE_SOUND)

#include	"soundmng.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"fdd_mtr.h"


enum {
	MOVE1TCK_MS		= 30,
	MOVEMOTOR1_MS	= 30,
	SEKSEC_MS		= 20
};

	_FDDMTR		fddmtr;


static void fddmtr_event(void) {

	switch(fddmtr.curevent) {
		case 100:
			soundmng_pcmstop(SOUND_PCMSEEK);
			fddmtr.nextevent += MOVEMOTOR1_MS;
			fddmtr.curevent = 1;
			break;

		default:
			fddmtr.curevent = 0;
			break;
	}
}


/* ---- */

void fddmtr_initialize(void) {

	soundmng_pcmstop(SOUND_PCMSEEK);
	ZeroMemory(&fddmtr, sizeof(fddmtr));
	FillMemory(fddmtr.head, sizeof(fddmtr.head), 24);
}

void fddmtr_callback(UINT32 time) {

	if ((fddmtr.curevent) && (time >= fddmtr.nextevent)) {
		fddmtr_event();
	}
}

void fddmtr_drvset(void) {

	fddmtr.curdrv = fdc.s.drv;
	if ((!fddmtr.eventtime[fddmtr.curdrv]) && (!fdc.s.motor)) {
		fddmtr.eventtime[fddmtr.curdrv] = GETTICK() + 5000;
	}
	else if ((fddmtr.eventtime[fddmtr.curdrv]) && (fdc.s.motor)) {
		if (fddmtr.eventtime[fddmtr.curdrv] < GETTICK()) {
			fddmtr.head[fddmtr.curdrv] = 24;
		}
		fddmtr.eventtime[fddmtr.curdrv] = 0;
	}
}

void fddmtr_motormove(void) {

	int		regmove;

	regmove = fddmtr.head[fddmtr.curdrv] - fdc.s.c;
	fddmtr.head[fddmtr.curdrv] = fdc.s.c;
	if (!xmilcfg.MOTOR) {
		return;
	}
	if (regmove < 0) {
		regmove *= (-1);
	}
	if (regmove == 1) {
		if (fddmtr.curevent < 80) {
			fddmtr_event();
			soundmng_pcmplay(SOUND_PCMSEEK1, FALSE);
			fddmtr.curevent = 80;
			fddmtr.nextevent = GETTICK() + MOVEMOTOR1_MS;
		}
	}
	else if (regmove) {
		if (fddmtr.curevent < 100) {
			fddmtr_event();
			soundmng_pcmplay(SOUND_PCMSEEK, TRUE);
			fddmtr.curevent = 100;
			fddmtr.nextevent = GETTICK() + regmove * MOVE1TCK_MS;
		}
	}
}

void fddmtr_waitsec(REG8 value) {

	if (!xmilcfg.MOTOR) {
		return;
	}
	if ((fdc.s.r != value) && (fddmtr.curevent < 1)) {
		fddmtr_event();
		fddmtr.curevent = 1;
		fddmtr.nextevent = GETTICK() + SEKSEC_MS;
	}
}

#endif	/* !defined(DISABLE_SOUND) */

