#include	"compiler.h"
#include	"soundmng.h"
#include	"pccore.h"
#include	"x1_io.h"
#include	"x1_fdc.h"


		BYTE		curevent = 0;
		DWORD		nextevent = 0;
		BYTE		FDC_HEAD[4] = {0, 0, 0, 0};
		DWORD		FDC_TIME[4] = {0, 0, 0, 0};
		short		curdrv = 0;

#define		MOVE1TCK_MS		30 
#define		MOVEMOTOR1_MS	30
#define		SEKSEC_MS		20

void fddmtr_event(void) {

	switch(curevent) {
		case 100:
			soundmng_pcmstop(SOUND_PCMSEEK);
			nextevent += MOVEMOTOR1_MS;
			curevent = 1;
			break;
		case 80:
			curevent = 0;
			break;
		default:
			curevent = 0;
			break;
	}
}



void fddmtr_init(void) {

	fddmtr_event();
	memset(FDC_HEAD, 24, sizeof(FDC_HEAD));
	ZeroMemory(FDC_TIME, sizeof(FDC_TIME));
}

void fddmtr_callback(DWORD time) {

	if ((curevent) && (time >= nextevent)) {
		fddmtr_event();
	}
}

void fddmtr_drvset(void) {

	curdrv = fdc.drv;
	if ((!FDC_TIME[curdrv]) && (!fdc.motor)) {
		FDC_TIME[curdrv] = GetTickCount() + 5000;
	}
	else if ((FDC_TIME[curdrv]) && (fdc.motor)) {
		if (FDC_TIME[curdrv] < GetTickCount()) {
			FDC_HEAD[curdrv] = 24;
		}
		FDC_TIME[curdrv] = 0;
	}
}

void fddmtr_motormove(void) {

	int		regmove;

	regmove = FDC_HEAD[curdrv] - fdc.c;
	FDC_HEAD[curdrv] = fdc.c;
	if ((!xmilcfg.MOTOR) || (xmilcfg.NOWAIT)) {
		return;
	}
	if (regmove < 0) {
		regmove *= (-1);
	}
	if (regmove == 1) {
		if (curevent < 80) {
			fddmtr_event();
			soundmng_pcmplay(SOUND_PCMSEEK1, FALSE);
			curevent = 80;
			nextevent = GetTickCount() + MOVEMOTOR1_MS;
		}
	}
	else if (regmove) {
		if (curevent < 100) {
			fddmtr_event();
			soundmng_pcmplay(SOUND_PCMSEEK, TRUE);
			curevent = 100;
			nextevent = GetTickCount() + regmove * MOVE1TCK_MS;
		}
	}
}

void fddmtr_waitsec(BYTE value) {

	if ((!xmilcfg.MOTOR) || (xmilcfg.NOWAIT)) {
		return;
	}
	if ((fdc.r != value) && (curevent < 1)) {
		fddmtr_event();
		curevent = 1;
		nextevent = GetTickCount() + SEKSEC_MS;
	}
}

