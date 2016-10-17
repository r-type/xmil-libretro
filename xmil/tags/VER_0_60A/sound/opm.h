/**
 * @file	opm.h
 * @brief	Interface of OPM
 */

#pragma once

#include "opmgen.h"

/**
 * Chips flags
 */
enum
{
	OPM_HAS_OPM			= 0x01,		/*!< Has OPM */
	OPM_X1F				= 0x02,		/*!< Supports S98 */
};

/**
 * @brief opm
 */
struct tagOpmState
{
	UINT8	addr;
	UINT8	cCaps;
	UINT8	reg[0x100];
};

/**
 * @brief opm
 */
struct tagOpm
{
	struct tagOpmState s;
	INTPTR userdata;
	_OPMGEN opmgen;
};

typedef struct tagOpm OPM;
typedef struct tagOpm* POPM;

#ifdef __cplusplus
extern "C"
{
#endif

void opm_construct(POPM opm);
void opm_destruct(POPM opm);
void opm_reset(POPM opm, REG8 cCaps);
void opm_bind(POPM opm);
void opm_restore(POPM opm);

void opm_writeRegister(POPM opm, REG8 nAddress, REG8 cData);

#ifdef __cplusplus
}
#endif
