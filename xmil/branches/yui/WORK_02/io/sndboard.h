/**
 * @file	sndboard.h
 * @brief	Interface of the sound boards
 */

#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#if !defined(DISABLE_SOUND)
void sndboard_update(void);
#else
#define	sndboard_update()
#endif

#if defined(SUPPORT_TURBOZ) || defined(SUPPORT_OPM)
void IOOUTCALL opm_o(UINT port, REG8 dat);
REG8 IOINPCALL opm_i(UINT port);
#endif

void IOOUTCALL sndboard_psgreg(UINT port, REG8 dat);
void IOOUTCALL sndboard_psgdat(UINT port, REG8 dat);
REG8 IOINPCALL sndboard_psgsta(UINT port);

void sndboard_reset(void);

#ifdef __cplusplus
}
#endif

