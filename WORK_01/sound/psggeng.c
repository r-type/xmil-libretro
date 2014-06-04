#include	"compiler.h"
#include	"parts.h"
#include	"sound.h"
#include	"sndctrl.h"


extern	PSGGENCFG	psggencfg;


void SOUNDCALL psggen_getpcm(PSGGEN psg, SINT32 *pcm, UINT count) {

	UINT	r;
	SINT32	*p;
	SINT32	noisevol;
	UINT8	mixer;
	UINT	noisetbl = 0;
	PSGTONE	*tone;
	PSGTONE	*toneterm;
	SINT32	samp;
	SINT32	vol;
	UINT	i;
	UINT	noise;

	while(count) {
		if (psg->envvolcnt >= 0) {
			r = min(count, psg->envcnt);
			psg->envcnt -= r;
		}
		else {
			r = count;
		}
		p = pcm;
		pcm += r * 2;
		count -= r;

		if ((psg->mixer & 0x3f) == 0) {
			r = min(r, psg->puchicount);
			psg->puchicount -= r;
		}
		while(r) {
			r--;
			noisevol = 0;
			mixer = psg->mixer;
			if (mixer & 0x38) {
				for (i=0; i<(1 << PSGADDEDBIT); i++) {
					SINT32 countbak;
					countbak = psg->noise.count;
					psg->noise.count -= psg->noise.freq;
					if (psg->noise.count > countbak) {
						psg->noise.base = rand_get() &
													(1 << (1 << PSGADDEDBIT));
					}
					noisetbl += psg->noise.base;
					noisetbl >>= 1;
				}
			}
			tone = psg->tone;
			toneterm = tone + 3;
			do {
				vol = *(tone->pvol);
				if (vol) {
					samp = 0;
					switch(mixer & 9) {
						case 0:							/* no mix */
							if (tone->puchi) {
								tone->puchi--;
								samp += vol << PSGADDEDBIT;
							}
							break;

						case 1:							/* tone only */
							for (i=0; i<(1 << PSGADDEDBIT); i++) {
								tone->count += tone->freq;
								samp += vol * ((tone->count>=0)?1:-1);
							}
							break;

						case 8:							/* noise only */
							noise = noisetbl;
							for (i=0; i<(1 << PSGADDEDBIT); i++) {
								samp += vol * ((noise & 1)?1:-1);
								noise >>= 1;
							}
							break;

						case 9:
							noise = noisetbl;
							for (i=0; i<(1 << PSGADDEDBIT); i++) {
								tone->count += tone->freq;
								if ((tone->count >= 0) && (noise & 1)) {
									samp += vol;
								}
								else {
									samp -= vol;
								}
								noise >>= 1;
							}
							break;
					}
					if (!(tone->pan & 1)) {
						p[0] += samp;
					}
					if (!(tone->pan & 2)) {
						p[1] += samp;
					}
				}
				mixer >>= 1;
			} while(++tone < toneterm);
			p += 2;
		}

		if ((psg->envvolcnt >= 0) && (psg->envcnt == 0)) {
			psg->envvolcnt--;
			if (psg->envvolcnt < 0) {
				if (psg->envmode & PSGENV_ONESHOT) {
					psg->envvol = (psg->envmode & PSGENV_LASTON)?15:0;
				}
				else {
					psg->envvolcnt = 15;
					if (!(psg->envmode & PSGENV_ONECYCLE)) {
						psg->envmode ^= PSGENV_INC;
					}
					psg->envcnt = psg->envmax;
					psg->envvol = (psg->envvolcnt ^ psg->envmode) & 0x0f;
				}
			}
			else {
				psg->envcnt = psg->envmax;
				psg->envvol = (psg->envvolcnt ^ psg->envmode) & 0x0f;
			}
			psg->puchicount = psggencfg.puchidec;
			if (psg->reg.vol[0] & 0x10) {
				psg->tone[0].puchi = psggencfg.puchidec;
			}
			if (psg->reg.vol[1] & 0x10) {
				psg->tone[1].puchi = psggencfg.puchidec;
			}
			if (psg->reg.vol[2] & 0x10) {
				psg->tone[2].puchi = psggencfg.puchidec;
			}
			psg->evol = psggencfg.volume[psg->envvol];
		}
	}
}

