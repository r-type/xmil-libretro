/**
 * @file	psg.c
 * @brief	Implementation of PSG
 */

#include "compiler.h"
#include "psg.h"
#include "sound.h"
#include "x1f.h"
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
	const UINT8 cCaps = psg->s.cCaps;

	if (cCaps & PSG_HAS_PSG)
	{
		keydisp_bindpsg(psg->s.reg, 4000000);
	}

	psg_restore(psg);

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

	if (nAddress < 0x10)
	{
		if (cCaps & PSG_HAS_PSG)
		{
			keydisp_psg(psg->s.reg, nAddress);
			psggen_setreg(&psg->psg, nAddress, cData);
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
