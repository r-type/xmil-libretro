/**
 * @file	advpsg.c
 * @brief	Implementation of the PSG generator
 */

#include "compiler.h"
#include "advpsg.h"
#include "z80core.h"
#include "pccore.h"

// #define	PSG0_DISABLE
// #define	PSG1_DISABLE
// #define	PSG2_DISABLE

// エンベロープパターン
static const UINT8 psgenv_pat[16] = {
					PSGENV_ONESHOT,
					PSGENV_ONESHOT,
					PSGENV_ONESHOT,
					PSGENV_ONESHOT,
					PSGENV_ONESHOT | PSGENV_INC,
					PSGENV_ONESHOT | PSGENV_INC,
					PSGENV_ONESHOT | PSGENV_INC,
					PSGENV_ONESHOT | PSGENV_INC,
					PSGENV_ONECYCLE,
					PSGENV_ONESHOT,
					0,
					PSGENV_ONESHOT | PSGENV_LASTON,
					PSGENV_ONECYCLE | PSGENV_INC,
					PSGENV_ONESHOT | PSGENV_INC | PSGENV_LASTON,
					PSGENV_INC,
					PSGENV_ONESHOT | PSGENV_INC};

#if 0
static const UINT32 advwavepat[16] = {
		0x00000000, 0x00001111, 0x00002222, 0x00003333,
		0x00004444, 0x00005555, 0x00006666, 0x00007777,
		0x00008888, 0x00009999, 0x0000aaaa, 0x0000bbbb,
		0x0000cccc, 0x0000dddd, 0x0000eeee, 0x0000ffff};
#else
static const UINT32 advwavepat[16] = {
		0x00000000, 0x11111111, 0x22222222, 0x33333333,
		0x44444444, 0x55555555, 0x66666666, 0x77777777,
		0x88888888, 0x99999999, 0xaaaaaaaa, 0xbbbbbbbb,
		0xcccccccc, 0xdddddddd, 0xeeeeeeee, 0xffffffff};
#endif


// PSG 2MHz / (16 * x)
// -> GB (4,194,304Hz / 16 / x) >> y
// **** if x=0 assume x=0.5 instead

#define	NOISEDAT(x, y)	((x) + ((y) << 4))

static const UINT8 noisepat[32] = {
		NOISEDAT(1,0),	NOISEDAT(1,1),	NOISEDAT(1,2),	NOISEDAT(3,1),
		NOISEDAT(1,3),	NOISEDAT(5,1),	NOISEDAT(3,2),	NOISEDAT(7,1),
		NOISEDAT(1,4),	NOISEDAT(5,2),	NOISEDAT(5,2),	NOISEDAT(3,3),
		NOISEDAT(3,3),	NOISEDAT(7,2),	NOISEDAT(7,2),	NOISEDAT(1,5),
		NOISEDAT(1,5),	NOISEDAT(5,3),	NOISEDAT(5,3),	NOISEDAT(5,3),
		NOISEDAT(5,3),	NOISEDAT(5,3),	NOISEDAT(3,4),	NOISEDAT(3,4),
		NOISEDAT(3,4),	NOISEDAT(7,3),	NOISEDAT(7,3),	NOISEDAT(7,3),
		NOISEDAT(7,3),	NOISEDAT(1,6),	NOISEDAT(1,6),	NOISEDAT(1,6)};


static void updatevolume(ADVPSG psg, REG8 bitmap) {

	REG8		mix;
	REG8		vol;
	REG8		bank;
volatile UINT32	*r;
	UINT32		data;
	REG8		noise;

	mix = psg->reg.mixer;
#if !defined(PSG0_DISABLE)
	if (bitmap & 0x01) {
		vol = 0;
		if (!(mix & 0x01)) {
			vol = psg->reg.vol[0];
			if (vol & 0x10) {
				vol = psg->envvol;
			}
			else {
				vol = vol & 0x0f;
			}
		}
		if (psg->vol[0] != vol) {
			psg->vol[0] = vol;
			*REG_SOUND1CNT_H = (UINT16)((vol << ADVSNDBIT_ENV)
												+ (2 << ADVSNDBIT_DUTYCYCLE));
			*REG_SOUND1CNT_X = psg->freq[0];
		}
	}
#endif
#if !defined(PSG1_DISABLE)
	if (bitmap & 0x02) {
		vol = 0;
		if (!(mix & 0x02)) {
			vol = psg->reg.vol[1];
			if (vol & 0x10) {
				vol = psg->envvol;
			}
			else {
				vol = vol & 0x0f;
			}
		}
		if (psg->vol[1] != vol) {
			psg->vol[1] = vol;
			*REG_SOUND2CNT_L = (UINT16)((vol << ADVSNDBIT_ENV)
												+ (2 << ADVSNDBIT_DUTYCYCLE));
			*REG_SOUND2CNT_H = psg->freq[1];
		}
	}
#endif
#if !defined(PSG2_DISABLE)
	if (bitmap & 0x04) {
		vol = 0;
		if (!(mix & 0x04)) {
			vol = psg->reg.vol[2];
			if (vol & 0x10) {
				vol = psg->envvol;
			}
			else {
				vol = vol & 0x0f;
			}
		}
		if (psg->vol[2] != vol) {
			psg->vol[2] = vol;
			if (vol == 0) {
				*REG_SOUND3CNT_L = 0;
				psg->ch3bank = 0;
			}
			else {
				bank = psg->ch3bank ^ 1;
				psg->ch3bank = bank;
				data = advwavepat[vol];
				r = REG_WAVE_RAM0;
				r[0] = data;
				r[1] = 0; // data;
				r[2] = data;
				r[3] = 0; // data;
				*REG_SOUND3CNT_L =
						(1 << ADVSNDBIT_OUTPUT) + (bank << ADVSNDBIT_BANKSEL);
				*REG_SOUND3CNT_X = psg->freq[2];
			}
		}
	}
#endif
	if (bitmap & 0x38) {
		noise = 0;
		if (!(mix & 0x08)) {
			vol = psg->reg.vol[0];
			if (vol & 0x10) {
				vol = psg->envvol;
			}
			else {
				vol = vol & 0x0f;
			}
			noise = max(noise, vol);
		}
		if (!(mix & 0x10)) {
			vol = psg->reg.vol[1];
			if (vol & 0x10) {
				vol = psg->envvol;
			}
			else {
				vol = vol & 0x0f;
			}
			noise = max(noise, vol);
		}
		if (!(mix & 0x20)) {
			vol = psg->reg.vol[2];
			if (vol & 0x10) {
				vol = psg->envvol;
			}
			else {
				vol = vol & 0x0f;
			}
			noise = max(noise, vol);
		}
		if (psg->vol[3] != noise) {
			psg->vol[3] = noise;
			*REG_SOUND4CNT_L = (UINT16)(noise << ADVSNDBIT_ENV);
			*REG_SOUND4CNT_H = psg->freq[3];
		}
	}
}

void advpsg_sync(ADVPSG psg) {

	UINT32	clock;
	int		step;

	if (psg->envvolcnt < 0) {
		return;
	}
	clock = CPU_CLOCK + CPU_BASECLOCK - CPU_REMCLOCK;
	clock -= psg->lastclock;
	if (clock < psg->envmax) {
		return;
	}
	step = clock / psg->envmax;
	psg->lastclock += step * psg->envmax;
	if (psg->envvolcnt >= step) {
		psg->envvolcnt -= step;
		psg->envvol = (psg->envvolcnt ^ psg->envmode) & 0x0f;
	}
	else {
		if (psg->envmode & PSGENV_ONESHOT) {
			psg->envvolcnt = -1;
			psg->envvol = (psg->envmode & PSGENV_LASTON)?15:0;
		}
		else {
			psg->envvolcnt -= (step & 0x1f);
			while(psg->envvolcnt < 0) {
				psg->envvolcnt += 16;
				if (!(psg->envmode & PSGENV_ONECYCLE)) {
					psg->envmode ^= PSGENV_INC;
				}
			}
			psg->envvol = (psg->envvolcnt ^ psg->envmode) & 0x0f;
		}
	}
	updatevolume(psg, 0x3f);
}

void advpsg_setreg(ADVPSG psg, REG8 reg, REG8 value) {

	REG8	modify;
	REG8	ch;
	UINT32	freq;

	if (reg >= 14) {
		return;
	}
	modify = ((UINT8 *)&psg->reg)[reg] ^ value;
	if ((!modify) && (reg != 13)) {
		return;
	}
	advpsg_sync(psg);
	((UINT8 *)&psg->reg)[reg] = value;
	switch(reg) {
#if !defined(PSG0_DISABLE)
		case 0:
		case 1:
#endif
#if !defined(PSG1_DISABLE)
		case 2:
		case 3:
#endif
#if !defined(PSG2_DISABLE)
		case 4:
		case 5:
#endif
			ch = reg >> 1;
			freq = psg->reg.tune[ch][0] +
										((psg->reg.tune[ch][1] & 0x0f) << 8);
			freq = (freq * 68719) >> 16;
#if 0
			if (ch == 2) {
				freq <<= 1;
			}
#endif
			if (freq == 0) {
				freq = 1;
			}
			else if (freq > 2048) {
				freq = 2048;
			}
			freq = (1 << ADVSNDBIT_RESET) + 0x800 - freq;
			psg->freq[ch] = freq;
			REG_SOUND1CNT_X[ch * 4] = freq;
			break;

		case 6:
			freq = (1 << ADVSNDBIT_RESET) + noisepat[value & 0x1f];
			psg->freq[3] = freq;
			*REG_SOUND4CNT_H = freq;
			break;

		case 7:
			if (modify & 0x3f) {
				updatevolume(psg, modify);
			}
			break;

#if !defined(PSG0_DISABLE)
		case 8:
#endif
#if !defined(PSG1_DISABLE)
		case 9:
#endif
#if !defined(PSG2_DISABLE)
		case 10:
#endif
			updatevolume(psg, (REG8)(0x09 << (reg - 8)));
			break;

		case 11:
		case 12:
			freq = LOADINTELWORD(psg->reg.envtime);
			if (freq == 0) {
				freq = 1 << 16;
			}
#if defined(FIX_Z80A)
			freq = freq * 16 * 2;
#else
			freq = freq * 16 * pccore.multiple;
#endif
			psg->envmax = freq;
			break;

		case 13:
			psg->envmode = psgenv_pat[value & 0x0f];
			psg->envvolcnt = 15;
			psg->envvol = (~psg->envmode) & 15;
			psg->lastclock = CPU_CLOCK + CPU_BASECLOCK - CPU_REMCLOCK;
			updatevolume(psg, 0x3f);
			break;
	}
}



// ----

void advpsg_reset(ADVPSG psg) {

	ZeroMemory(psg, sizeof(_ADVPSG));
	psg->reg.mixer = 0x3f;
	psg->reg.io1 = 0xff;
	psg->reg.io2 = 0xff;

	psg->envmode = PSGENV_ONESHOT;
	psg->envvolcnt = 15;
	psg->envvol = 15;
	psg->lastclock = CPU_CLOCK;
	psg->envmax = (1 << 16) * 32;

	*REG_SOUNDCNT_X	= 0x80;
	*REG_SOUNDCNT_L	= 0xff66;
	*REG_SOUNDCNT_H	= 2;

	*REG_SOUND1CNT_L = 0;
	*REG_SOUND1CNT_H = (2 << ADVSNDBIT_DUTYCYCLE);
	*REG_SOUND1CNT_X = (1 << ADVSNDBIT_RESET);
	*REG_SOUND2CNT_L = (2 << ADVSNDBIT_DUTYCYCLE);
	*REG_SOUND2CNT_H = (1 << ADVSNDBIT_RESET);

	*REG_SOUND3CNT_L = 0;
	*REG_SOUND3CNT_H = (1 << ADVSNDBIT_VOLRATIO);
	*REG_SOUND3CNT_X = (1 << ADVSNDBIT_RESET);

	*REG_SOUND4CNT_L = 0;
	*REG_SOUND4CNT_H = 0;

	*REG_WAVE_RAM0 = 0;
	*REG_WAVE_RAM1 = 0;
	*REG_WAVE_RAM2 = 0;
	*REG_WAVE_RAM3 = 0;

	*REG_SOUNDBIAS = 0x200 + (3 << 14);
}

