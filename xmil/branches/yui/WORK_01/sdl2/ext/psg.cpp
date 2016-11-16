/**
 * @file	psg.cpp
 * @brief	Implementation of PSG
 */

#include "compiler.h"
#include "sound/psg.h"
#include "sound/sound.h"
#include "sound/x1f.h"
#include "externalchipmanager.h"
#include "externalpsg.h"
#include "generic/keydisp.h"

static void writeRegister(PPSG psg, REG8 nAddress, REG8 cData);

/**
 * Initialize instance
 * @param[in] psg The instance
 */
void psg_construct(PPSG psg)
{
	memset(psg, 0, sizeof(*psg));
}

/**
 * Deinitialize instance
 * @param[in] psg The instance
 */
void psg_destruct(PPSG psg)
{
	CExternalPsg* pExt = reinterpret_cast<CExternalPsg*>(psg->userdata);
	CExternalChipManager::GetInstance()->Release(pExt);
	psg->userdata = NULL;
}

/**
 * Reset
 * @param[in] psg The instance
 * @param[in] cCaps
 */
void psg_reset(PPSG psg, REG8 cCaps)
{
	memset(&psg->s, 0, sizeof(psg->s));
	psg->s.cCaps = cCaps;
	psg->s.reg[0x07] = 0xbf;
	psg->s.reg[0x0e] = 0xff;
	psg->s.reg[0x0f] = 0xff;

	psggen_reset(&psg->psg);

	if (cCaps == 0)
	{
		CExternalPsg* pExt = reinterpret_cast<CExternalPsg*>(psg->userdata);
		if (pExt)
		{
			CExternalChipManager::GetInstance()->Release(pExt);
			psg->userdata = NULL;
		}
	}
}

/**
 * Restore
 * @param[in] psg The instance
 */
void psg_restore(PPSG psg)
{
	REG8 i;

	// PSG
	for (i = 0; i < 14; i++)
	{
		writeRegister(psg, i, psg->s.reg[i]);
	}
}

/**
 * Bind
 * @param[in] psg The instance
 */
void psg_bind(PPSG psg)
{
	UINT8 cCaps = psg->s.cCaps;

	if (cCaps & PSG_HAS_PSG)
	{
		keydisp_bindpsg(psg->s.reg, 4000000);
	}

	CExternalPsg* pExt = reinterpret_cast<CExternalPsg*>(psg->userdata);
	if (pExt == NULL)
	{
		pExt = static_cast<CExternalPsg*>(CExternalChipManager::GetInstance()->GetInterface(IExternalChip::kAY8910, 2000000));
		psg->userdata = reinterpret_cast<INTPTR>(pExt);
	}
	if (pExt)
	{
		pExt->Reset();
	}
	psg_restore(psg);

	if (pExt)
	{
		cCaps &= ~PSG_HAS_PSG;
	}
	if (cCaps & PSG_HAS_PSG)
	{
		sound_streamregist(&psg->psg, (SOUNDCB)psggen_getpcm);
	}
}

/**
 * Status
 * @param[in] psg The instance
 * @return Status
 */
REG8 psg_readStatus(PPSG psg)
{
	if (psg->s.cCaps & PSG_HAS_PSG)
	{
		return 0;
	}
	return 0xff;
}

/**
 * Writes register
 * @param[in] psg The instance
 * @param[in] nAddress The address
 * @param[in] cData The data
 */
void psg_writeRegister(PPSG psg, REG8 nAddress, REG8 cData)
{
	if (nAddress >= 0x10)
	{
		return;
	}

	psg->s.reg[nAddress] = cData;
	if (psg->s.cCaps & PSG_X1F)
	{
		x1f_psg(nAddress, cData);
	}
	writeRegister(psg, nAddress, cData);
}

/**
 * Writes register (Inner)
 * @param[in] psg The instance
 * @param[in] nAddress The address
 * @param[in] cData The data
 */
static void writeRegister(PPSG psg, REG8 nAddress, REG8 cData)
{
	const UINT8 cCaps = psg->s.cCaps;

	if ((cCaps & PSG_HAS_PSG) && (nAddress < 0x0e))
	{
		keydisp_psg(psg->s.reg, nAddress);

		CExternalPsg* pExt = reinterpret_cast<CExternalPsg*>(psg->userdata);
		if (!pExt)
		{
			psggen_setreg(&psg->psg, nAddress, cData);
		}
		else
		{
			pExt->WriteRegister(nAddress, cData);
		}
	}
}

/**
 * Reads register
 * @param[in] psg The instance
 * @param[in] nAddress The address
 * @return data
 */
REG8 psg_readRegister(PPSG psg, REG8 nAddress)
{
	if (nAddress < 0x10)
	{
		if (psg->s.cCaps & PSG_HAS_PSG)
		{
			return psg->s.reg[nAddress];
		}
	}
	return 0xff;
}
