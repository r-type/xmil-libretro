/**
 * @file	opm.c
 * @brief	Implementation of OPM
 */

#include "compiler.h"
#include "opm.h"
#include "sound.h"
#include "x1f.h"

static void writeRegister(POPM opm, REG8 nAddress, REG8 cData);

/**
 * Initialize instance
 * @param[in] opm The instance
 */
void opm_construct(POPM opm)
{
	memset(opm, 0, sizeof(*opm));
}

/**
 * Deinitialize instance
 * @param[in] opm The instance
 */
void opm_destruct(POPM opm)
{
}

/**
 * Reset
 * @param[in] opm The instance
 * @param[in] cCaps
 */
void opm_reset(POPM opm, REG8 cCaps)
{
	memset(&opm->s, 0, sizeof(opm->s));
	opm->s.cCaps = cCaps;

	opmgen_reset(&opm->opmgen);
}

/**
 * Restore
 * @param[in] opm The instance
 */
void opm_restore(POPM opm)
{
	UINT i;

	// FM
	for (i = 0x20; i < 0x100; i++)
	{
		writeRegister(opm, (REG8)i, opm->s.reg[i]);
	}
}

/**
 * Bind
 * @param[in] opm The instance
 */
void opm_bind(POPM opm)
{
	const UINT8 cCaps = opm->s.cCaps;

	opm_restore(opm);

	if (cCaps & OPM_HAS_OPM)
	{
		sound_streamregist(&opm->opmgen, (SOUNDCB)opmgen_getpcm);
	}
}

/**
 * Writes register
 * @param[in] opm The instance
 * @param[in] nAddress The address
 * @param[in] cData The data
 */
void opm_writeRegister(POPM opm, REG8 nAddress, REG8 cData)
{
	opm->s.reg[nAddress] = cData;

	if (opm->s.cCaps & OPM_X1F)
	{
		x1f_opm(nAddress, cData);
	}

	writeRegister(opm, nAddress, cData);
}

/**
 * Writes register (Inner)
 * @param[in] opm The instance
 * @param[in] nAddress The address
 * @param[in] cData The data
 */
static void writeRegister(POPM opm, REG8 nAddress, REG8 cData)
{
	const UINT8 cCaps = opm->s.cCaps;

	if (cCaps & OPM_HAS_OPM)
	{
		opmgen_setreg(&opm->opmgen, nAddress, cData);
	}
}
