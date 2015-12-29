/**
 * @file	softkbd.c
 * @brief	
 */

#include "compiler.h"

#if defined(SUPPORT_SOFTKBD)

#include "bmpdata.h"
#include "softkbd.h"
#include "keystat.h"

#define SOFTKEY_NC		0xff

#include "softkbd.res"

struct tagSoftKbd
{
	UINT8	key;
	UINT8	key2;
	UINT8	flag;
	void	*ptr;
	CMNBMP	bmp;
	CMNPAL	pal[16];
};
typedef struct tagSoftKbd	SOFTKBD;
typedef struct tagSoftKbd	*PSOFTKBD;

static SOFTKBD s_softkbd;

static void loadbmp(PSOFTKBD softkbd)
{
	void *ptr;

	softkbd->ptr = NULL;
	ptr = (void *)bmpdata_solvedata(skbd_bmp);
	if (ptr != NULL)
	{
		if (cmndraw_bmp4inf(&softkbd->bmp, ptr) == SUCCESS)
		{
			softkbd->ptr = ptr;
		}
		else
		{
			_MFREE(ptr);
		}
	}
	softkbd->flag |= SOFTKEY_FLAGREDRAW;
}

void softkbd_initialize(void)
{
	PSOFTKBD softkbd = &s_softkbd;

	memset(softkbd, 0, sizeof(*softkbd));
	softkbd->key = SOFTKEY_NC;
	loadbmp(softkbd);
}

void softkbd_deinitialize(void)
{
	PSOFTKBD softkbd = &s_softkbd;
	void *ptr;

	ptr = softkbd->ptr;
	softkbd->ptr = NULL;
	if (ptr)
	{
		_MFREE(ptr);
	}
}

BOOL softkbd_getsize(int *width, int *height)
{
	PSOFTKBD softkbd = &s_softkbd;

	if (softkbd->ptr == NULL)
	{
		return FALSE;
	}
	if (width)
	{
		*width = softkbd->bmp.width;
	}
	if (height)
	{
		*height = softkbd->bmp.height;
	}
	return TRUE;
}

REG8 softkbd_process(void)
{
	PSOFTKBD softkbd = &s_softkbd;

	return softkbd->flag;
}

BOOL softkbd_paint(CMNVRAM *vram, CMNPALCNV cnv, BOOL redraw)
{
	PSOFTKBD softkbd = &s_softkbd;
	UINT8 flag;
	BOOL ret;

	flag = softkbd->flag;
	softkbd->flag = 0;
	if (redraw)
	{
		flag = SOFTKEY_FLAGREDRAW | SOFTKEY_FLAGDRAW;
	}
	ret = FALSE;
	if ((flag & SOFTKEY_FLAGREDRAW) && (vram) && (cnv))
	{
		(*cnv)(softkbd->pal, softkbd->bmp.paltbl, softkbd->bmp.pals, vram->bpp);
		cmndraw_bmp16(vram, softkbd->ptr, cnv, CMNBMP_LEFT | CMNBMP_TOP);
		ret = TRUE;
	}
	if (flag & SOFTKEY_FLAGDRAW)
	{
		TRACEOUT(("softkbd_paint"));
		ret = TRUE;
	}
	return ret;
}

BOOL softkbd_down(int x, int y)
{
	PSOFTKBD softkbd = &s_softkbd;
	UINT8 key;

	softkbd_up();
	key = getsoftkbd(x, y);
	if (key != SOFTKEY_NC)
	{
		keystat_keydown(key);
		softkbd->key = key;
		return TRUE;
	}
	return FALSE;
}

void softkbd_up(void)
{
	PSOFTKBD softkbd = &s_softkbd;

	if (softkbd->key != SOFTKEY_NC)
	{
		keystat_keyup(softkbd->key);
		softkbd->key = SOFTKEY_NC;
	}
}
#endif	/* defined(SUPPORT_SOFTKBD) */
