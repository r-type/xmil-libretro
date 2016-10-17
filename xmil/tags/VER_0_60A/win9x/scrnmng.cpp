/**
 * @file	scrnmng.cpp
 * @brief	Screen Manager (DirectDraw2)
 *
 * @author	$Author: yui $
 * @date	$Date: 2011/03/07 09:54:11 $
 */

#include "compiler.h"
#include <ddraw.h>
#ifndef __GNUC__
#include <winnls32.h>
#endif
#include "resource.h"
#include "xmil.h"
#include "mousemng.h"
#include "scrnmng.h"
// #include "sysmng.h"
#include "extclass.h"
#include "pccore.h"
#include "scrndraw.h"
#include "palettes.h"
#include "makescrn.h"

#if defined(SUPPORT_DCLOCK)
#include "subwnd/dclock.h"
#endif	// defined(SUPPORT_DCLOCK)

#if !defined(__GNUC__)
#pragma comment(lib, "ddraw.lib")
#pragma comment(lib, "dxguid.lib")
#endif	// !defined(__GNUC__)

typedef struct {
	LPDIRECTDRAW		ddraw1;
	LPDIRECTDRAW2		ddraw2;
	LPDIRECTDRAWSURFACE	primsurf;
	LPDIRECTDRAWSURFACE	backsurf;
#if defined(SUPPORT_DCLOCK)
	LPDIRECTDRAWSURFACE	clocksurf;
#endif	// defined(SUPPORT_DCLOCK)
	LPDIRECTDRAWCLIPPER	clipper;
	LPDIRECTDRAWPALETTE	palette;
	UINT				scrnmode;
	int					width;
	int					height;
//	int					extend;
	int					cliping;
	RGB32				pal16mask;
	UINT8				r16b;
	UINT8				l16r;
	UINT8				l16g;
	BOOL				menudisp;
	int					menusize;
	RECT				scrn;
	RECT				rect;
	RECT				scrnclip;
	RECT				rectclip;
	PALETTEENTRY		pal[256];
} DDRAW;

typedef struct {
	int		width;
	int		height;
//	int		extend;
	int		multiple;
} SCRNSTAT;

static	DDRAW		ddraw;
		SCRNMNG		scrnmng;
static	SCRNSTAT	scrnstat;
static	SCRNSURF	scrnsurf;

static void setwindowsize(HWND hWnd, int width, int height)
{
	RECT workrc;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &workrc, 0);
	const int scx = GetSystemMetrics(SM_CXSCREEN);
	const int scy = GetSystemMetrics(SM_CYSCREEN);

	UINT nCount = 2;
	do
	{
		RECT rectwindow;
		GetWindowRect(hWnd, &rectwindow);

		RECT rectclient;
		GetClientRect(hWnd, &rectclient);

		int winx = (xmiloscfg.winx != CW_USEDEFAULT) ? xmiloscfg.winx : rectwindow.left;
		int winy = (xmiloscfg.winy != CW_USEDEFAULT) ? xmiloscfg.winy : rectwindow.top;
		int cx = width;
		cx += rectwindow.right - rectwindow.left;
		cx -= rectclient.right - rectclient.left;
		int cy = height;
		cy += rectwindow.bottom - rectwindow.top;
		cy -= rectclient.bottom - rectclient.top;

		if (scx < cx)
		{
			winx = (scx - cx) / 2;
		}
		else
		{
			if ((winx + cx) > workrc.right)
			{
				winx = workrc.right - cx;
			}
			if (winx < workrc.left)
			{
				winx = workrc.left;
			}
		}
		if (scy < cy)
		{
			winy = (scy - cy) / 2;
		}
		else
		{
			if ((winy + cy) > workrc.bottom)
			{
				winy = workrc.bottom - cy;
			}
			if (winy < workrc.top)
			{
				winy = workrc.top;
			}
		}
		MoveWindow(hWnd, winx, winy, cx, cy, TRUE);
	} while (--nCount);
}

static void renewalclientsize(BOOL winloc) {

	int		width;
	int		height;
	int		multiple;
	int		scrnwidth;
	int		scrnheight;

	width = min(scrnstat.width, ddraw.width);
	height = min(scrnstat.height, ddraw.height);

	// �`��͈́`
	if (ddraw.scrnmode & SCRNMODE_FULLSCREEN) {
		ddraw.rect.right = width;
		ddraw.rect.bottom = height;
		scrnwidth = width;
		scrnheight = height;
		const UINT8 fscrnmod = xmiloscfg.fscrnmod & FSCRNMOD_ASPECTMASK;
		switch (fscrnmod)
		{
			default:
			case FSCRNMOD_NORESIZE:
				break;

			case FSCRNMOD_ASPECTFIX8:
				scrnwidth = (ddraw.width << 3) / width;
				scrnheight = (ddraw.height << 3) / height;
				multiple = min(scrnwidth, scrnheight);
				scrnwidth = (width * multiple) >> 3;
				scrnheight = (height * multiple) >> 3;
				break;

			case FSCRNMOD_ASPECTFIX:
				scrnwidth = ddraw.width;
				scrnheight = (scrnwidth * height) / width;
				if (scrnheight >= ddraw.height) {
					scrnheight = ddraw.height;
					scrnwidth = (scrnheight * width) / height;
				}
				break;

			case FSCRNMOD_LARGE:
				scrnwidth = ddraw.width;
				scrnheight = ddraw.height;
				break;
		}
		ddraw.scrn.left = (ddraw.width - scrnwidth) / 2;
		ddraw.scrn.top = (ddraw.height - scrnheight) / 2;
		ddraw.scrn.right = ddraw.scrn.left + scrnwidth;
		ddraw.scrn.bottom = ddraw.scrn.top + scrnheight;

		// ���j���[�\�����̕`��̈�
		ddraw.rectclip = ddraw.rect;
		ddraw.scrnclip = ddraw.scrn;
		if (ddraw.scrnclip.top < ddraw.menusize) {
			ddraw.scrnclip.top = ddraw.menusize;
			int tmpcy = ddraw.height - ddraw.menusize;
			if (scrnheight > tmpcy) {
				switch(fscrnmod) {
					default:
					case FSCRNMOD_NORESIZE:
						tmpcy = min(tmpcy, height);
						ddraw.rectclip.bottom = tmpcy;
						break;

					case FSCRNMOD_ASPECTFIX8:
					case FSCRNMOD_ASPECTFIX:
						ddraw.rectclip.bottom = (tmpcy * height) / scrnheight;
						break;

					case FSCRNMOD_LARGE:
						break;
				}
			}
			ddraw.scrnclip.bottom = ddraw.menusize + tmpcy;
		}
	}
	else {
		multiple = scrnstat.multiple;
		scrnwidth = (width * multiple) >> 3;
		scrnheight = (height * multiple) >> 3;
		ddraw.rect.right = width;
		ddraw.rect.bottom = height;
		ddraw.scrn.left = 0;				// + np2oscfg.paddingx
		ddraw.scrn.top = 0;					// + np2oscfg.paddingy
		ddraw.scrn.right = scrnwidth;		// + np2oscfg.paddingx
		ddraw.scrn.bottom = scrnheight;		// + np2oscfg.paddingy
		setwindowsize(hWndMain, scrnwidth, scrnheight);
	}
	scrnsurf.width = width;
	scrnsurf.height = height;
//	scrnsurf.extend = extend;
}

static void clearoutofrect(const RECT *target, const RECT *base) {

	LPDIRECTDRAWSURFACE	primsurf;
	DDBLTFX				ddbf;
	RECT				rect;

	primsurf = ddraw.primsurf;
	if (primsurf == NULL) {
		return;
	}
	ZeroMemory(&ddbf, sizeof(ddbf));
	ddbf.dwSize = sizeof(ddbf);
	ddbf.dwFillColor = 0;

	rect.left = base->left;
	rect.right = base->right;
	rect.top = base->top;
	rect.bottom = target->top;
	if (rect.top < rect.bottom) {
		primsurf->Blt(&rect, NULL, NULL, DDBLT_COLORFILL, &ddbf);
	}
	rect.top = target->bottom;
	rect.bottom = base->bottom;
	if (rect.top < rect.bottom) {
		primsurf->Blt(&rect, NULL, NULL, DDBLT_COLORFILL, &ddbf);
	}

	rect.top = max(base->top, target->top);
	rect.bottom = min(base->bottom, target->bottom);
	if (rect.top < rect.bottom) {
		rect.left = base->left;
		rect.right = target->left;
		if (rect.left < rect.right) {
			primsurf->Blt(&rect, NULL, NULL, DDBLT_COLORFILL, &ddbf);
		}
		rect.left = target->right;
		rect.right = base->right;
		if (rect.left < rect.right) {
			primsurf->Blt(&rect, NULL, NULL, DDBLT_COLORFILL, &ddbf);
		}
	}
}

static void clearoutscreen(void) {

	RECT	base;
	POINT	clipt;
	RECT	target;

	GetClientRect(hWndMain, &base);
	clipt.x = 0;
	clipt.y = 0;
	ClientToScreen(hWndMain, &clipt);
	base.left += clipt.x;
	base.top += clipt.y;
	base.right += clipt.x;
	base.bottom += clipt.y;
	target.left = base.left + ddraw.scrn.left;
	target.top = base.top + ddraw.scrn.top;
	target.right = base.left + ddraw.scrn.right;
	target.bottom = base.top + ddraw.scrn.bottom;
	clearoutofrect(&target, &base);
}

static void clearoutfullscreen(void) {

	RECT	base;
const RECT	*scrn;

	base.left = 0;
	base.top = 0;
	base.right = ddraw.width;
	base.bottom = ddraw.height;
	if (GetWindowLong(hWndMain, EXTGWL_HMENU)) {
		scrn = &ddraw.scrn;
		base.top = 0;
	}
	else {
		scrn = &ddraw.scrnclip;
		base.top = ddraw.menusize;
	}
	clearoutofrect(scrn, &base);
#if defined(SUPPORT_DCLOCK)
	DispClock::GetInstance()->Redraw();
#endif	// defined(SUPPORT_DCLOCK)
}

static void paletteinit()
{
	HDC hdc = GetDC(hWndMain);
	GetSystemPaletteEntries(hdc, 0, 256, ddraw.pal);
	ReleaseDC(hWndMain, hdc);
#if defined(SUPPORT_DCLOCK)
	const RGB32* pal32 = DispClock::GetInstance()->GetPalettes();
	for (UINT i = 0; i < 4; i++)
	{
		ddraw.pal[i + START_PALORG].peBlue = pal32[i].p.b;
		ddraw.pal[i + START_PALORG].peRed = pal32[i].p.r;
		ddraw.pal[i + START_PALORG].peGreen = pal32[i].p.g;
		ddraw.pal[i + START_PALORG].peFlags = PC_RESERVED | PC_NOCOLLAPSE;
	}
#endif	// defined(SUPPORT_DCLOCK)
	for (UINT i = 0; i < XMILPAL_USE; i++)
	{
		ddraw.pal[i + START_PAL].peFlags = PC_RESERVED | PC_NOCOLLAPSE;
	}
	pal_reset();
	ddraw.ddraw2->CreatePalette(DDPCAPS_8BIT, ddraw.pal, &ddraw.palette, 0);
	ddraw.primsurf->SetPalette(ddraw.palette);
	scrnmng.palchanged = 0;
}

static void paletteset()
{
	if ((ddraw.palette != NULL) && (xmil_palettes))
	{
		for (UINT i = 0; i < xmil_palettes; i++)
		{
			ddraw.pal[i + START_PAL].peRed = xmil_pal32[i].p.r;
			ddraw.pal[i + START_PAL].peBlue = xmil_pal32[i].p.b;
			ddraw.pal[i + START_PAL].peGreen = xmil_pal32[i].p.g;
		}
		ddraw.palette->SetEntries(0, START_PAL, xmil_palettes, &ddraw.pal[START_PAL]);
	}
}

static void make16mask(DWORD bmask, DWORD rmask, DWORD gmask) {

	UINT8	sft;

	sft = 0;
	while((!(bmask & 0x80)) && (sft < 32)) {
		bmask <<= 1;
		sft++;
	}
	ddraw.pal16mask.p.b = (UINT8)bmask;
	ddraw.r16b = sft;

	sft = 0;
	while((rmask & 0xffffff00) && (sft < 32)) {
		rmask >>= 1;
		sft++;
	}
	ddraw.pal16mask.p.r = (UINT8)rmask;
	ddraw.l16r = sft;

	sft = 0;
	while((gmask & 0xffffff00) && (sft < 32)) {
		gmask >>= 1;
		sft++;
	}
	ddraw.pal16mask.p.g = (UINT8)gmask;
	ddraw.l16g = sft;
}


// ----

void scrnmng_initialize(void) {

	scrnstat.width = 640;
	scrnstat.height = 400;
//	scrnstat.extend = 1;
	scrnstat.multiple = 8;
	setwindowsize(hWndMain, 640, 400);
}

BRESULT scrnmng_create(UINT8 scrnmode) {

	DWORD			winstyle;
	DWORD			winstyleex;
	LPDIRECTDRAW2	ddraw2;
	DDSURFACEDESC	ddsd;
	DDPIXELFORMAT	ddpf;
	int				width;
	int				height;
	UINT			bitcolor;

	ZeroMemory(&scrnmng, sizeof(scrnmng));
	winstyle = GetWindowLong(hWndMain, GWL_STYLE);
	winstyleex = GetWindowLong(hWndMain, GWL_EXSTYLE);
	if (scrnmode & SCRNMODE_FULLSCREEN) {
		scrnmng.flag = SCRNFLAG_FULLSCREEN;
		winstyle &= ~(WS_CAPTION | WS_SYSMENU | WS_THICKFRAME);
		winstyle |= WS_POPUP;
		winstyleex |= WS_EX_TOPMOST;
		ddraw.menudisp = FALSE;
		ddraw.menusize = GetSystemMetrics(SM_CYMENU);
		extclass_enablemenu(hWndMain, FALSE);
	}
	else {
//		scrnmng.flag = 0;
		winstyle |= WS_SYSMENU;
		if (xmiloscfg.thickframe) {
			winstyle |= WS_THICKFRAME;
		}
		winstyle |= WS_CAPTION;
		winstyle &= ~WS_POPUP;
		winstyleex &= ~WS_EX_TOPMOST;
	}
	SetWindowLong(hWndMain, GWL_STYLE, winstyle);
	SetWindowLong(hWndMain, GWL_EXSTYLE, winstyleex);

	if (DirectDrawCreate(NULL, &ddraw.ddraw1, NULL) != DD_OK) {
		goto scre_err;
	}
	ddraw.ddraw1->QueryInterface(IID_IDirectDraw2, (void **)&ddraw2);
	ddraw.ddraw2 = ddraw2;

	if (scrnmode & SCRNMODE_FULLSCREEN) {
#if defined(SUPPORT_DCLOCK)
		DispClock::GetInstance()->Initialize();
#endif	// defined(SUPPORT_DCLOCK)
		ddraw2->SetCooperativeLevel(hWndMain,
										DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN);
		width = SURFACE_WIDTH;
		height = 480;
		bitcolor = xmiloscfg.fscrnbpp;
		const UINT8 fscrnmod = xmiloscfg.fscrnmod;
		if (fscrnmod & (FSCRNMOD_SAMERES | FSCRNMOD_SAMEBPP))
		{
			DEVMODE devmode;
			if (EnumDisplaySettings(NULL, ENUM_REGISTRY_SETTINGS, &devmode))
			{
				if (fscrnmod & FSCRNMOD_SAMERES)
				{
					width = devmode.dmPelsWidth;
					height = devmode.dmPelsHeight;
				}
				if (fscrnmod & FSCRNMOD_SAMEBPP)
				{
					bitcolor = devmode.dmBitsPerPel;
				}
			}
		}
		if (bitcolor == 0) {
			bitcolor = (scrnmode & (SCRNMODE_SYSHIGHCOLOR | SCRNMODE_COREHIGHCOLOR)) ? 16 : 8;
		}
		if (ddraw2->SetDisplayMode(width, height, bitcolor, 0, 0) != DD_OK) {
			goto scre_err;
		}
		ddraw2->CreateClipper(0, &ddraw.clipper, NULL);
		ddraw.clipper->SetHWnd(0, hWndMain);

		ZeroMemory(&ddsd, sizeof(ddsd));
		ddsd.dwSize = sizeof(ddsd);
		ddsd.dwFlags = DDSD_CAPS;
		ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
		if (ddraw2->CreateSurface(&ddsd, &ddraw.primsurf, NULL) != DD_OK) {
			goto scre_err;
		}
//		fullscrn_clearblank();

		ZeroMemory(&ddpf, sizeof(ddpf));
		ddpf.dwSize = sizeof(DDPIXELFORMAT);
		if (ddraw.primsurf->GetPixelFormat(&ddpf) != DD_OK) {
			goto scre_err;
		}

		ZeroMemory(&ddsd, sizeof(ddsd));
		ddsd.dwSize = sizeof(ddsd);
		ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;
		ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
		ddsd.dwWidth = SURFACE_WIDTH;
		ddsd.dwHeight = SURFACE_HEIGHT;
		if (ddraw2->CreateSurface(&ddsd, &ddraw.backsurf, NULL) != DD_OK) {
			goto scre_err;
		}
		if (bitcolor == 8) {
			paletteinit();
		}
		else if (bitcolor == 16) {
			make16mask(ddpf.dwBBitMask, ddpf.dwRBitMask, ddpf.dwGBitMask);
		}
		else if (bitcolor == 24) {
		}
		else if (bitcolor == 32) {
		}
		else {
			goto scre_err;
		}
#if defined(SUPPORT_DCLOCK)
		DispClock::GetInstance()->SetPalettes(bitcolor);
		ZeroMemory(&ddsd, sizeof(ddsd));
		ddsd.dwSize = sizeof(ddsd);
		ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;
		ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
		ddsd.dwWidth = DCLOCK_WIDTH;
		ddsd.dwHeight = DCLOCK_HEIGHT;
		if (ddraw2->CreateSurface(&ddsd, &ddraw.clocksurf, NULL) != DD_OK) {
			ddraw.clocksurf = NULL;
		}
		DispClock::GetInstance()->Reset();
#endif	// defined(SUPPORT_DCLOCK)
	}
	else {
		ddraw2->SetCooperativeLevel(hWndMain, DDSCL_NORMAL);

		ZeroMemory(&ddsd, sizeof(ddsd));
		ddsd.dwSize = sizeof(ddsd);
		ddsd.dwFlags = DDSD_CAPS;
		ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
		if (ddraw2->CreateSurface(&ddsd, &ddraw.primsurf, NULL) != DD_OK) {
			goto scre_err;
		}

		ddraw2->CreateClipper(0, &ddraw.clipper, NULL);
		ddraw.clipper->SetHWnd(0, hWndMain);
		ddraw.primsurf->SetClipper(ddraw.clipper);

		ZeroMemory(&ddpf, sizeof(ddpf));
		ddpf.dwSize = sizeof(DDPIXELFORMAT);
		if (ddraw.primsurf->GetPixelFormat(&ddpf) != DD_OK) {
			goto scre_err;
		}

		ZeroMemory(&ddsd, sizeof(ddsd));
		ddsd.dwSize = sizeof(ddsd);
		ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;
		ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
		ddsd.dwWidth = SURFACE_WIDTH;
		ddsd.dwHeight = SURFACE_HEIGHT;
		width = SURFACE_WIDTH;
		height = SURFACE_HEIGHT;

		if (ddraw2->CreateSurface(&ddsd, &ddraw.backsurf, NULL) != DD_OK) {
			goto scre_err;
		}
		bitcolor = ddpf.dwRGBBitCount;
		if (bitcolor == 8) {
			paletteinit();
		}
		else if (bitcolor == 16) {
			make16mask(ddpf.dwBBitMask, ddpf.dwRBitMask, ddpf.dwGBitMask);
		}
		else if (bitcolor == 24) {
		}
		else if (bitcolor == 32) {
		}
		else {
			goto scre_err;
		}
	}
	scrnmng.bpp = (UINT8)bitcolor;
	scrnsurf.bpp = bitcolor;
	ddraw.scrnmode = scrnmode;
	ddraw.width = width;
	ddraw.height = height;
	ddraw.cliping = 0;
	renewalclientsize(FALSE);
	return(SUCCESS);

scre_err:
	scrnmng_destroy();
	return(FAILURE);
}

void scrnmng_destroy(void) {

	if (scrnmng.flag & SCRNFLAG_FULLSCREEN) {
		extclass_enablemenu(hWndMain, TRUE);
	}
#if defined(SUPPORT_DCLOCK)
	if (ddraw.clocksurf) {
		ddraw.clocksurf->Release();
		ddraw.clocksurf = NULL;
	}
#endif	// defined(SUPPORT_DCLOCK)
	if (ddraw.backsurf) {
		ddraw.backsurf->Release();
		ddraw.backsurf = NULL;
	}
	if (ddraw.palette) {
		ddraw.palette->Release();
		ddraw.palette = NULL;
	}
	if (ddraw.clipper) {
		ddraw.clipper->Release();
		ddraw.clipper = NULL;
	}
	if (ddraw.primsurf) {
		ddraw.primsurf->Release();
		ddraw.primsurf = NULL;
	}
	if (ddraw.ddraw2) {
		if (ddraw.scrnmode & SCRNMODE_FULLSCREEN) {
			ddraw.ddraw2->SetCooperativeLevel(hWndMain, DDSCL_NORMAL);
		}
		ddraw.ddraw2->Release();
		ddraw.ddraw2 = NULL;
	}
	if (ddraw.ddraw1) {
		ddraw.ddraw1->Release();
		ddraw.ddraw1 = NULL;
	}
	ZeroMemory(&ddraw, sizeof(ddraw));
}

void scrnmng_querypalette(void) {

	if (ddraw.palette) {
		ddraw.primsurf->SetPalette(ddraw.palette);
	}
}

RGB16 scrnmng_makepal16(RGB32 pal32) {

	RGB32	pal;

	pal.d = pal32.d & ddraw.pal16mask.d;
	return((RGB16)((pal.p.g << ddraw.l16g) +
						(pal.p.r << ddraw.l16r) + (pal.p.b >> ddraw.r16b)));
}

void scrnmng_fullscrnmenu(int y) {

	BOOL menudisp;

	if (scrnmng.flag & SCRNFLAG_FULLSCREEN) {
		menudisp = ((y >= 0) && (y < ddraw.menusize)) ? TRUE : FALSE;
		if (ddraw.menudisp != menudisp) {
			ddraw.menudisp = menudisp;
			if (menudisp) {
				extclass_enablemenu(hWndMain, TRUE);
			}
			else {
				extclass_enablemenu(hWndMain, FALSE);
				clearoutfullscreen();
			}
		}
	}
}

void scrnmng_topwinui(void) {

	mousemng_disable(MOUSEPROC_WINUI);
	if (!ddraw.cliping++) {											// ver0.28
		if (scrnmng.flag & SCRNFLAG_FULLSCREEN) {
			ddraw.primsurf->SetClipper(ddraw.clipper);
		}
#ifndef __GNUC__
		WINNLSEnableIME(hWndMain, TRUE);
#endif
	}
}

void scrnmng_clearwinui(void) {

	if ((ddraw.cliping > 0) && (!(--ddraw.cliping))) {
#ifndef __GNUC__
		WINNLSEnableIME(hWndMain, FALSE);
#endif
		if (scrnmng.flag & SCRNFLAG_FULLSCREEN) {
			ddraw.primsurf->SetClipper(0);
		}
	}
	if (scrnmng.flag & SCRNFLAG_FULLSCREEN) {
		extclass_enablemenu(hWndMain, FALSE);
		clearoutfullscreen();
		ddraw.menudisp = FALSE;
	}
	mousemng_enable(MOUSEPROC_WINUI);
}


// ----

void scrnmng_setheight(int posy, int height) {

	scrnstat.height = height;
	renewalclientsize(TRUE);
}

BRESULT scrnmng_setcolormode(BOOL fullcolor) {

	REG8	newmode;

	newmode = g_scrnmode & (~(SCRNMODE_COREHIGHCOLOR));
	if (fullcolor) {
		newmode |= SCRNMODE_COREHIGHCOLOR;
	}
	return xmil_changescreen(newmode);
}

const SCRNSURF *scrnmng_surflock(void) {

	DDSURFACEDESC	destscrn;
	HRESULT			r;

	ZeroMemory(&destscrn, sizeof(destscrn));
	destscrn.dwSize = sizeof(destscrn);
	if (ddraw.backsurf == NULL) {
		return(NULL);
	}
	r = ddraw.backsurf->Lock(NULL, &destscrn, DDLOCK_WAIT, NULL);
	if (r == DDERR_SURFACELOST) {
		ddraw.backsurf->Restore();
		r = ddraw.backsurf->Lock(NULL, &destscrn, DDLOCK_WAIT, NULL);
	}
	if (r != DD_OK) {
		return(NULL);
	}
	scrnsurf.ptr = (UINT8 *)destscrn.lpSurface;
	scrnsurf.xalign = scrnmng.bpp >> 3;
	scrnsurf.yalign = destscrn.lPitch;
//	scrnsurf.width = 640;
//	scrnsurf.height = 400;
//	scrnsurf.bpp = scrnmng.bpp;
	return(&scrnsurf);
}

void scrnmng_surfunlock(const SCRNSURF *surf) {

	ddraw.backsurf->Unlock(NULL);
	scrnmng_update();
}

void scrnmng_update(void) {

	POINT	clip;
	RECT	dst;
	RECT	*rect;
	RECT	*scrn;
	HRESULT	r;

	if (scrnmng.palchanged) {
		scrnmng.palchanged = FALSE;
		paletteset();
	}
	if (ddraw.backsurf != NULL) {
		if (ddraw.scrnmode & SCRNMODE_FULLSCREEN) {
			if (scrnmng.allflash) {
				scrnmng.allflash = 0;
				clearoutfullscreen();
			}
			if (GetWindowLong(hWndMain, EXTGWL_HMENU)) {
				rect = &ddraw.rect;
				scrn = &ddraw.scrn;
			}
			else {
				rect = &ddraw.rectclip;
				scrn = &ddraw.scrnclip;
			}
			r = ddraw.primsurf->Blt(scrn, ddraw.backsurf, rect,
															DDBLT_WAIT, NULL);
			if (r == DDERR_SURFACELOST) {
				ddraw.backsurf->Restore();
				ddraw.primsurf->Restore();
				ddraw.primsurf->Blt(scrn, ddraw.backsurf, rect,
															DDBLT_WAIT, NULL);
			}
		}
		else {
			if (scrnmng.allflash) {
				scrnmng.allflash = 0;
				clearoutscreen();
			}
			clip.x = 0;
			clip.y = 0;
			ClientToScreen(hWndMain, &clip);
			dst.left = clip.x + ddraw.scrn.left;
			dst.top = clip.y + ddraw.scrn.top;
			dst.right = clip.x + ddraw.scrn.right;
			dst.bottom = clip.y + ddraw.scrn.bottom;
			r = ddraw.primsurf->Blt(&dst, ddraw.backsurf, &ddraw.rect,
									DDBLT_WAIT, NULL);
			if (r == DDERR_SURFACELOST) {
				ddraw.backsurf->Restore();
				ddraw.primsurf->Restore();
				ddraw.primsurf->Blt(&dst, ddraw.backsurf, &ddraw.rect,
														DDBLT_WAIT, NULL);
			}
		}
	}
}


// ----

void scrnmng_setmultiple(int multiple)
{
	if (scrnstat.multiple != multiple)
	{
		scrnstat.multiple = multiple;
		renewalclientsize(TRUE);
	}
}

int scrnmng_getmultiple(void)
{
	return scrnstat.multiple;
}


// ----

#if defined(SUPPORT_DCLOCK)
static const RECT rectclk = {0, 0, DCLOCK_WIDTH, DCLOCK_HEIGHT};

void scrnmng_dispclock(void)
{
	if (!ddraw.clocksurf)
	{
		return;
	}
	if (!DispClock::GetInstance()->IsDisplayed())
	{
		return;
	}

	if ((ddraw.scrn.bottom + DCLOCK_HEIGHT) > ddraw.height)
	{
		return;
	}
	DispClock::GetInstance()->Make();

	DDSURFACEDESC dest;
	ZeroMemory(&dest, sizeof(dest));
	dest.dwSize = sizeof(dest);
	if (ddraw.clocksurf->Lock(NULL, &dest, DDLOCK_WAIT, NULL) == DD_OK)
	{
		DispClock::GetInstance()->Draw(scrnmng.bpp, dest.lpSurface, dest.lPitch);
		ddraw.clocksurf->Unlock(NULL);
	}
	if (ddraw.primsurf->BltFast(ddraw.width - DCLOCK_WIDTH - 4,
									ddraw.height - DCLOCK_HEIGHT,
									ddraw.clocksurf, const_cast<RECT*>(&rectclk),
									DDBLTFAST_WAIT) == DDERR_SURFACELOST)
	{
		ddraw.primsurf->Restore();
		ddraw.clocksurf->Restore();
	}
	DispClock::GetInstance()->CountDown(xmiloscfg.DRAW_SKIP);
}
#endif	// defined(SUPPORT_DCLOCK)


// ----

typedef struct {
	int		bx;
	int		by;
	int		cx;
	int		cy;
	int		mul;
} SCRNSIZING;

static	SCRNSIZING	scrnsizing;

enum {
	SIZING_ADJUST	= 12
};

void scrnmng_entersizing(void) {

	RECT	rectwindow;
	RECT	rectclient;
	int		cx;
	int		cy;

	GetWindowRect(hWndMain, &rectwindow);
	GetClientRect(hWndMain, &rectclient);
	scrnsizing.bx = (rectwindow.right - rectwindow.left) -
					(rectclient.right - rectclient.left);
	scrnsizing.by = (rectwindow.bottom - rectwindow.top) -
					(rectclient.bottom - rectclient.top);
	cx = min(scrnstat.width, ddraw.width);
	cx = (cx + 7) >> 3;
	cy = min(scrnstat.height, ddraw.height);
	cy = (cy + 7) >> 3;
	scrnsizing.cx = cx;
	scrnsizing.cy = cy;
	scrnsizing.mul = scrnstat.multiple;
}

void scrnmng_sizing(UINT side, RECT *rect) {

	int		width;
	int		height;
	int		mul;

	if ((side != WMSZ_TOP) && (side != WMSZ_BOTTOM)) {
		width = rect->right - rect->left - scrnsizing.bx + SIZING_ADJUST;
		width /= scrnsizing.cx;
	}
	else {
		width = 16;
	}
	if ((side != WMSZ_LEFT) && (side != WMSZ_RIGHT)) {
		height = rect->bottom - rect->top - scrnsizing.by + SIZING_ADJUST;
		height /= scrnsizing.cy;
	}
	else {
		height = 16;
	}
	mul = min(width, height);
	if (mul <= 0) {
		mul = 1;
	}
	else if (mul > 16) {
		mul = 16;
	}
	width = scrnsizing.bx + (scrnsizing.cx * mul);
	height = scrnsizing.by + (scrnsizing.cy * mul);
	switch(side) {
		case WMSZ_LEFT:
		case WMSZ_TOPLEFT:
		case WMSZ_BOTTOMLEFT:
			rect->left = rect->right - width;
			break;

		case WMSZ_RIGHT:
		case WMSZ_TOP:
		case WMSZ_TOPRIGHT:
		case WMSZ_BOTTOM:
		case WMSZ_BOTTOMRIGHT:
		default:
			rect->right = rect->left + width;
			break;
	}

	switch(side) {
		case WMSZ_TOP:
		case WMSZ_TOPLEFT:
		case WMSZ_TOPRIGHT:
			rect->top = rect->bottom - height;
			break;

		case WMSZ_LEFT:
		case WMSZ_RIGHT:
		case WMSZ_BOTTOM:
		case WMSZ_BOTTOMLEFT:
		case WMSZ_BOTTOMRIGHT:
		default:
			rect->bottom = rect->top + height;
			break;
	}
	scrnsizing.mul = mul;
}

void scrnmng_exitsizing(void)
{
	scrnmng_setmultiple(scrnsizing.mul);
	InvalidateRect(hWndMain, NULL, TRUE);		// ugh
}

