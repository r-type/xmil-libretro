/**
 * @file	opm.cpp
 * @brief	Implementation of OPM
 */

#include "compiler.h"
#include "sound/opm.h"
#include "sound/sound.h"
#include "sound/x1f.h"
#include "nbv4manager.h"
#include "generic/keydisp.h"

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
	IExternalChip* pExt = reinterpret_cast<IExternalChip*>(opm->userdata);
	CNbv4Manager::GetInstance()->Release(pExt);
	opm->userdata = 0;
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
	memset(&opm->s.reg[0x20], 0xc0, 0x08);
	memset(&opm->s.reg[0x40], 0xff, 0xc0);

	opmgen_reset(&opm->opmgen);

	if (cCaps == 0)
	{
		IExternalChip* pExt = reinterpret_cast<IExternalChip*>(opm->userdata);
		if (pExt)
		{
			CNbv4Manager::GetInstance()->Release(pExt);
			opm->userdata = 0;
		}
	}
}

/**
 * Restore
 * @param[in] opm The instance
 */
void opm_restore(POPM opm)
{
	UINT i;

	// FM
	for (i = 0; i < 0x100; i++)
	{
		if (i == 8)
		{
			continue;
		}
		writeRegister(opm, (REG8)i, opm->s.reg[i]);
	}
}

/**
 * Bind
 * @param[in] opm The instance
 */
void opm_bind(POPM opm)
{
	UINT8 cCaps = opm->s.cCaps;

	if (cCaps & OPM_HAS_OPM)
	{
		keydisp_bindopm(opm->s.reg, 4000000);
	}

	IExternalChip* pExt = reinterpret_cast<IExternalChip*>(opm->userdata);
	if (pExt == NULL)
	{
		pExt = static_cast<IExternalChip*>(CNbv4Manager::GetInstance()->GetInterface(IExternalChip::kYM2151, 4000000));
		opm->userdata = reinterpret_cast<INTPTR>(pExt);
	}
	if (pExt)
	{
		pExt->Reset();
	}
	opm_restore(opm);

	if (pExt)
	{
		cCaps &= ~OPM_HAS_OPM;
	}
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
		if ((nAddress < 0x20) && ((0x0b178102 & ((1 << nAddress))) == 0))
		{
			return;
		}

		if (nAddress == 8)
		{
			keydisp_opmkeyon(opm->s.reg, cData);
		}

		IExternalChip* pExt = reinterpret_cast<IExternalChip*>(opm->userdata);
		if (!pExt)
		{
			opmgen_setreg(&opm->opmgen, nAddress, cData);
		}
		else
		{
			pExt->WriteRegister(nAddress, cData);
		}
	}
}
