/**
 * @file	psg.h
 * @brief	Interface of PSG
 */

#pragma once

#if !defined(DISABLE_SOUND)
#include "psggen.h"
#endif	/* !defined(DISABLE_SOUND) */

/**
 * Chips flags
 */
enum
{
	PSG_HAS_PSG		= 0x01,		/*!< Has PSG */
	PSG_X1F			= 0x02,		/*!< Supports S98 */
};

/**
 * @brief psg
 */
struct tagPsgState
{
	UINT8	addr;
	UINT8	cCaps;
	UINT8	reg[16];
};

/**
 * @brief psg
 */
struct tagPsg
{
	struct tagPsgState s;
#if !defined(DISABLE_SOUND)
	INTPTR userdata;
	_PSGGEN psg;
#endif	/* !defined(DISABLE_SOUND) */
};

typedef struct tagPsg PSG;
typedef struct tagPsg* PPSG;

#ifdef __cplusplus
extern "C"
{
#endif

void psg_construct(PPSG psg);
void psg_destruct(PPSG psg);
void psg_reset(PPSG psg, REG8 cCaps);
void psg_restore(PPSG psg);
void psg_bind(PPSG psg);

REG8 psg_readStatus(PPSG psg);
REG8 psg_readRegister(PPSG psg, REG8 nAddress);
void psg_writeRegister(PPSG psg, REG8 nAddress, REG8 cData);

#ifdef __cplusplus
}
#endif
