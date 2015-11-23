/**
 * @file	sndctrl.h
 * @brief	Interface of the sound
 */

#pragma once

#include "opm.h"
#include "psg.h"

#if !defined(DISABLE_SOUND)

#ifdef __cplusplus
extern "C"
{
#endif

#if defined(SUPPORT_TURBOZ) || defined(SUPPORT_OPM)
extern	OPM		g_opm;
#endif
extern	PSG		g_psg;

void sndctrl_initialize(void);
void sndctrl_deinitialize(void);
void sndctrl_reset(void);

#ifdef __cplusplus
}
#endif

#else

#define	sndctrl_initialize()
#define sndctrl_deinitialize()
#define sndctrl_reset()

#endif

