/**
 * @file	sndctrl.h
 * @brief	Interface of the sound
 */

#pragma once

#if !defined(DISABLE_SOUND)

#ifdef __cplusplus
extern "C"
{
#endif

void sndctrl_initialize(void);
void sndctrl_deinitialize(void);

#ifdef __cplusplus
}
#endif

#else

#define	sndctrl_initialize()
#define sndctrl_deinitialize()

#endif
