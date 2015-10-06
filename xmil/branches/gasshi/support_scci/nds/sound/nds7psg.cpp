#include "compiler.h"
#include "libnds.h"
#include "pccore.h"
#include "ipcxfer.h"
#include "nds7psg.h"

// #define PSG1_DISABLE
// #define PSG2_DISABLE
// #define PSG3_DISABLE
// #define PSGNOISE_DISABLE


enum
{
	PSGENV_INC			= 15,
	PSGENV_ONESHOT		= 16,
	PSGENV_LASTON		= 32,
	PSGENV_ONECYCLE		= 64
};


// エンベロープパターン
static const UINT8 s_psgenv_pat[16] =
{
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
	PSGENV_ONESHOT | PSGENV_INC
};





// ----

static void updatevolume(PNDS7PSG pPsg, REG8 cBitmap)
{

	// with SCHANNEL_ENABLE | (3 << 24) | SOUND_PAN(64) | SOUND_FORMAT_PSG
	static const UINT32 s_dwVolReg[16] =
	{
#if 1
		0x00000000, 0xe340030c, 0xe3400310, 0xe3400318,
		0xe3400321, 0xe3400330, 0xe3400343, 0xe3400360,
		0xe3400221, 0xe3400230, 0xe3400243, 0xe3400260,
		0xe3400143, 0xe3400160, 0xe3400043, 0xe3400060,
#else
		0x00000000, 0xe3400308, 0xe340030b, 0xe3400310,
		0xe3400316, 0xe3400320, 0xe340032d, 0xe3400340,
		0xe340035a, 0xe3400220, 0xe340022d, 0xe3400240,
		0xe340025a, 0xe3400140, 0xe340015a, 0xe3400040,
#endif
	};

	const REG8 cMix = pPsg->reg.r.cMixer;

#if !defined(PSG1_DISABLE)
	if (cBitmap & 0x01)
	{
		REG8 cVol = 0;
		if (!(cMix & 0x01))
		{
			cVol = pPsg->reg.r.cVol[0];
			if (cVol & 0x10)
			{
				cVol = pPsg->cEnvVol;
			}
			else
			{
				cVol = cVol & 0x0f;
			}
		}
		if (pPsg->cVol[0] != cVol)
		{
			pPsg->cVol[0] = cVol;
			SCHANNEL_CR(8 + 0) = s_dwVolReg[cVol];
		}
	}
#endif	// !defined(PSG1_DISABLE)

#if	!defined(PSG2_DISABLE)
	if (cBitmap & 0x02)
	{
		REG8 cVol = 0;
		if (!(cMix & 0x02))
		{
			cVol = pPsg->reg.r.cVol[1];
			if (cVol & 0x10)
			{
				cVol = pPsg->cEnvVol;
			}
			else
			{
				cVol = cVol & 0x0f;
			}
		}
		if (pPsg->cVol[1] != cVol)
		{
			pPsg->cVol[1] = cVol;
			SCHANNEL_CR(8 + 1) = s_dwVolReg[cVol];
		}
	}
#endif	// !defined(PSG2_DISABLE)

#if !defined(PSG3_DISABLE)
	if (cBitmap & 0x04)
	{
		REG8 cVol = 0;
		if (!(cMix & 0x04))
		{
			cVol = pPsg->reg.r.cVol[2];
			if (cVol & 0x10)
			{
				cVol = pPsg->cEnvVol;
			}
			else
			{
				cVol = cVol & 0x0f;
			}
		}
		if (pPsg->cVol[2] != cVol)
		{
			pPsg->cVol[2] = cVol;
			SCHANNEL_CR(8 + 2) = s_dwVolReg[cVol];
		}
	}
#endif	// !defined(PSG3_DISABLE)

#if !defined(PSGNOISE_DISABLE)
	if (cBitmap & 0x38)
	{
		REG8 cNoise = 0;
		if (!(cMix & 0x08))
		{
			REG8 cVol = pPsg->reg.r.cVol[0];
			if (cVol & 0x10)
			{
				cVol = pPsg->cEnvVol;
			}
			else
			{
				cVol = cVol & 0x0f;
			}
			cNoise = max(cNoise, cVol);
		}
		if (!(cMix & 0x10))
		{
			REG8 cVol = pPsg->reg.r.cVol[1];
			if (cVol & 0x10)
			{
				cVol = pPsg->cEnvVol;
			}
			else
			{
				cVol = cVol & 0x0f;
			}
			cNoise = max(cNoise, cVol);
		}
		if (!(cMix & 0x20))
		{
			REG8 cVol = pPsg->reg.r.cVol[2];
			if (cVol & 0x10)
			{
				cVol = pPsg->cEnvVol;
			}
			else
			{
				cVol = cVol & 0x0f;
			}
			cNoise = max(cNoise, cVol);
		}
		if (pPsg->cVol[3] != cNoise)
		{
			pPsg->cVol[3] = cNoise;
			SCHANNEL_CR(14) = s_dwVolReg[cNoise];
		}
	}
#endif	// !defined(PSGNOISE_DISABLE)
}

static void sync(PNDS7PSG pPsg)
{
	if (pPsg->cEnvVolCount < 0)
	{
		return;
	}
	UINT uClock = pPsg->uClock;
	uClock -= pPsg->uLastClock;
	if (uClock < pPsg->uEnvMax)
	{
		return;
	}

	int nStep = uClock / pPsg->uEnvMax;
	pPsg->uLastClock += nStep * pPsg->uEnvMax;
	if (pPsg->cEnvVolCount >= nStep)
	{
		pPsg->cEnvVolCount -= nStep;
		pPsg->cEnvVol = (pPsg->cEnvVolCount ^ pPsg->cEnvMode) & 0x0f;
	}
	else
	{
		if (pPsg->cEnvMode & PSGENV_ONESHOT)
		{
			pPsg->cEnvVolCount = -1;
			pPsg->cEnvVol = (pPsg->cEnvMode & PSGENV_LASTON) ? 15 : 0;
		}
		else
		{
			pPsg->cEnvVolCount -= (nStep & 0x1f);
			while(pPsg->cEnvVolCount < 0)
			{
				pPsg->cEnvVolCount += 16;
				if (!(pPsg->cEnvMode & PSGENV_ONECYCLE))
				{
					pPsg->cEnvMode ^= PSGENV_INC;
				}
			}
			pPsg->cEnvVol = (pPsg->cEnvVolCount ^ pPsg->cEnvMode) & 0x0f;
		}
	}
	updatevolume(pPsg, 0x3f);
}

static void setreg(PNDS7PSG pPsg, REG8 cReg, REG8 cValue)
{
	const REG8 cModify = pPsg->reg.b[cReg] ^ cValue;
	if ((!cModify) && (cReg != 13))
	{
		return;
	}
	pPsg->reg.b[cReg] = cValue;

	switch(cReg)
	{
#if !defined(PSG1_DISABLE) || !defined(PSG2_DISABLE) || !defined(PSG3_DISABLE)
#if !defined(PSG1_DISABLE)
		case 0:
		case 1:
#endif
#if !defined(PSG2_DISABLE)
		case 2:
		case 3:
#endif
#if !defined(PSG3_DISABLE)
		case 4:
		case 5:
#endif
			{
				const UINT uCh = cReg >> 1;
				UINT uFreq = pPsg->reg.r.cTune[uCh][0] + ((pPsg->reg.r.cTune[uCh][1] & 0x0f) << 8);
				// Rate = (4MHz / 72) / Tune
				// uFreq = (33513982 / 2) * uFreq / 125000 / 8; (16.756991)
#if 1
				uFreq = (uFreq * 17159 /*.08864*/) >> 10;
#else
				uFreq = (uFreq * 0x10 * 9 / 8);
#endif
				uFreq = 0 - uFreq;
				SCHANNEL_TIMER(uCh + 8) = uFreq;
			}
			break;
#endif	// !defined(PSG1_DISABLE) || !defined(PSG2_DISABLE) || !defined(PSG3_DISABLE)

#if !defined(PSGNOISE_DISABLE)
		case 6:
			{
				UINT uFreq = cValue & 0x1f;
				// Rate = 2MHz / (16 * Noise)
				// Freq = 0x1000000 * (16 * Noise) / 2MHz
				// Freq = 0x10 * 16 * Noise / 2Hz
				if (uFreq)
				{
					uFreq = (0x10 * uFreq);
				}
				else
				{
					uFreq = (0x10 / 2);
				}
				uFreq = 0 - uFreq;
//				pPsg->uFreq[3] = uFreq;
				SCHANNEL_TIMER(14) = uFreq;
			}
			break;
#endif	// !defined(PSGNOISE_DISABLE)

		case 7:
			if (cModify & 0x3f)
			{
				updatevolume(pPsg, cModify);
			}
			break;

#if !defined(PSG1_DISABLE) || !defined(PSG2_DISABLE) || !defined(PSG3_DISABLE)
#if !defined(PSG1_DISABLE)
		case 8:
#endif
#if !defined(PSG2_DISABLE)
		case 9:
#endif
#if !defined(PSG3_DISABLE)
		case 10:
#endif
			updatevolume(pPsg, (REG8)(0x09 << (cReg - 8)));
			break;
#endif	// !defined(PSG1_DISABLE) || !defined(PSG2_DISABLE) || !defined(PSG3_DISABLE)

		case 11:
		case 12:
			{
				UINT uFreq = LOADINTELWORD(pPsg->reg.r.cEnvTime);
				if (uFreq == 0)
				{
					uFreq = 1 << 16;
				}
#if defined(FIX_Z80A)
				uFreq = uFreq * 16 * 2;
#else	// defined(FIX_Z80A)
				uFreq = uFreq * 16 * pccore.multiple;
#endif	// defined(FIX_Z80A)
				pPsg->uEnvMax = uFreq;
			}
			break;

		case 13:
			pPsg->cEnvMode = s_psgenv_pat[cValue & 0x0f];
			pPsg->cEnvVolCount = 15;
			pPsg->cEnvVol = (~pPsg->cEnvMode) & 15;
			pPsg->uLastClock = pPsg->uClock;
			updatevolume(pPsg, 0x3f);
			break;
	}
}

// ----

void nds7psg_reset(PNDS7PSG pPsg)
{
	ZeroMemory(pPsg, sizeof(*pPsg));
	pPsg->reg.r.cMixer = 0x3f;

	pPsg->cEnvMode = PSGENV_ONESHOT;
	pPsg->cEnvVolCount = 15;
	pPsg->cEnvVol = 15;
	pPsg->uLastClock = 0;
	pPsg->uEnvMax = (1 << 16) * 32;

	SOUND_CR = SOUND_ENABLE | SOUND_VOL(0x7f);
//	*REG_SOUNDBIAS = 0x200;		// 504

	SCHANNEL_CR(8) = 0;
	SCHANNEL_CR(9) = 0;
	SCHANNEL_CR(10) = 0;
	SCHANNEL_CR(14) = 0;
}

void nds7psg_sync(PNDS7PSG pPsg)
{
	PNDSPSGXFER pXfer = IPCPSGXFER;
	const UINT uClock = pXfer->uClock;
	if (uClock)
	{
		pPsg->uClock += uClock;
		if (pXfer->cUpdateReg)
		{
			for (REG8 i=0; i<=12; i++)
			{
				setreg(pPsg, i, pXfer->reg.b[i]);
			}
		}
		if (pXfer->cUpdateEnv)
		{
			setreg(pPsg, 13, pXfer->reg.b[13]);
		}
		sync(pPsg);
		pXfer->uClock = 0;
	}
}

