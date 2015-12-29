/**
 * @file	softkbd.h
 * @brief
 */

#pragma once

#if defined(SUPPORT_SOFTKBD)

#include "cmndraw.h"

enum
{
	SOFTKEY_FLAGDRAW		= 0x01,		/*!< Dirty */
	SOFTKEY_FLAGREDRAW		= 0x02		/*!< Redraw */
};


#ifdef __cplusplus
extern "C"
{
#endif

void softkbd_initialize(void);
void softkbd_deinitialize(void);
BOOL softkbd_getsize(int *width, int *height);
REG8 softkbd_process(void);
BOOL softkbd_paint(CMNVRAM *vram, CMNPALCNV cnv, BOOL redraw);
BOOL softkbd_down(int x, int y);
void softkbd_up(void);

#ifdef __cplusplus
}
#endif

#endif
