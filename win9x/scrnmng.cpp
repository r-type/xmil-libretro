#include	"compiler.h"
#include	<ddraw.h>
#ifndef __GNUC__
#include	<winnls32.h>
#endif
#include	"resource.h"
#include	"xmil.h"
#include	"mousemng.h"
#include	"scrnmng.h"
#include	"soundmng.h"
#include	"sysmng.h"
#include	"extclass.h"
#include	"pccore.h"
#include	"scrndraw.h"
#include	"palettes.h"
#include	"makescrn.h"
#if defined(SUPPORT_DCLOCK)
#include "dclock.h"
#endif	// defined(SUPPORT_DCLOCK)

#pragma comment(lib, "ddraw.lib")
#pragma comment(lib, "dxguid.lib")

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
	PALETTEENTRY		pal[256];
} DDRAW;

typedef struct {
	int		width;
	int		height;
//	int		extend;
//	int		multiple;
} SCRNSTAT;

		REG8		scrnmode;
static	DDRAW		ddraw;
		SCRNMNG		scrnmng;
static	SCRNSTAT	scrnstat;
static	SCRNSURF	scrnsurf;


static void setwindowsize(HWND hWnd, int width, int height) {

	int		winx;
	int		winy;
	RECT	workrc;
	int		scx;
	int		scy;
	UINT	cnt;
	RECT	rectwindow;
	RECT	rectclient;
	int		cx;
	int		cy;
	UINT	update;

	winx = xmiloscfg.winx;
	winy = xmiloscfg.winy;

	SystemParametersInfo(SPI_GETWORKAREA, 0, &workrc, 0);
	scx = GetSystemMetrics(SM_CXSCREEN);
	scy = GetSystemMetrics(SM_CYSCREEN);

	cnt = 2;
	do {
		GetWindowRect(hWnd, &rectwindow);
		GetClientRect(hWnd, &rectclient);
		cx = width;
		cx += rectwindow.right - rectwindow.left;
		cx -= rectclient.right - rectclient.left;
		cy = height;
		cy += rectwindow.bottom - rectwindow.top;
		cy -= rectclient.bottom - rectclient.top;

		update = 0;
		if (scx < cx) {
			winx = (scx - cx) / 2;
		}
		else {
			if ((winx + cx) > workrc.right) {
				winx = workrc.right - cx;
			}
			if (winx < workrc.left) {
				winx = workrc.left;
			}
		}
		if (scy < cy) {
			winy = (scy - cy) / 2;
		}
		else {
			if ((winy + cy) > workrc.bottom) {
				winy = workrc.bottom - cy;
			}
			if (winy < workrc.top) {
				winy = workrc.top;
			}
		}
		MoveWindow(hWnd, winx, winy, cx, cy, TRUE);
	} while(--cnt);

	if ((xmiloscfg.winx != winx) || (xmiloscfg.winy != winy)) {
		xmiloscfg.winx = winx;
		xmiloscfg.winy = winy;
		sysmng_update(SYS_UPDATEOSCFG);
	}
}

static void renewalclientsize(BOOL winloc) {

	int		width;
	int		height;
	int		multiple;
	int		scrnwidth;
	int		scrnheight;

	width = min(scrnstat.width, ddraw.width);
	height = min(scrnstat.height, ddraw.height);

	// •`‰æ”ÍˆÍ`
	if (ddraw.scrnmode & SCRNMODE_FULLSCREEN) {
		ddraw.rect.right = width;
		ddraw.rect.bottom = height;
		ddraw.scrn.left = (ddraw.width - width) / 2;
		ddraw.scrn.top = (ddraw.height - height) / 2;
		ddraw.scrn.right = ddraw.scrn.left + width;
		ddraw.scrn.bottom = ddraw.scrn.top + height;
	}
	else {
		multiple = 8;
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

	base.left = 0;
	base.top = (GetWindowLong(hWndMain, EXTGWL_HMENU))?0:ddraw.menusize;
	base.right = ddraw.width;			// (+ ddraw.extend)
	base.bottom = ddraw.height;
	clearoutofrect(&ddraw.scrn, &base);
#if defined(SUPPORT_DCLOCK)
	dclock_redraw();
#endif	// defined(SUPPORT_DCLOCK)
}

static void paletteinit(void) {

	HDC 	hdc;
	UINT	i;

	hdc = GetDC(hWndMain);
	GetSystemPaletteEntries(hdc, 0, 256, ddraw.pal);
	ReleaseDC(hWndMain, hdc);
	for (i=0; i<XMILPAL_USE; i++) {
		ddraw.pal[i + START_PAL].peFlags = PC_RESERVED | PC_NOCOLLAPSE;
	}
#if defined(SUPPORT_DCLOCK)
	for (i=0; i<4; i++) {
		ddraw.pal[i + START_PALORG].peBlue = dclockpal.pal32[i].p.b;
		ddraw.pal[i + START_PALORG].peRed = dclockpal.pal32[i].p.r;
		ddraw.pal[i + START_PALORG].peGreen = dclockpal.pal32[i].p.g;
		ddraw.pal[i + START_PALORG].peFlags = PC_RESERVED | PC_NOCOLLAPSE;
	}
#endif	// defined(SUPPORT_DCLOCK)
	pal_reset();
	ddraw.ddraw2->CreatePalette(DDPCAPS_8BIT, ddraw.pal, &ddraw.palette, 0);
	ddraw.primsurf->SetPalette(ddraw.palette);
	scrnmng.palchanged = 0;
}

static void paletteset(void) {

	UINT	i;

	if ((ddraw.palette != NULL) && (xmil_palettes)) {
		for (i=0; i<xmil_palettes; i++) {
			ddraw.pal[i+START_PAL].peRed = xmil_pal32[i].p.r;
			ddraw.pal[i+START_PAL].peBlue = xmil_pal32[i].p.b;
			ddraw.pal[i+START_PAL].peGreen = xmil_pal32[i].p.g;
		}
		ddraw.palette->SetEntries(0, START_PAL, xmil_palettes,
													&ddraw.pal[START_PAL]);
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
//	scrnstat.multiple = 8;
	setwindowsize(hWndMain, 640, 400);
}

BRESULT scrnmng_create(UINT8 mode) {

	DWORD			winstyle;
	DWORD			winstyleex;
	HMENU			hmenu;
	LPDIRECTDRAW2	ddraw2;
	DDSURFACEDESC	ddsd;
	DDPIXELFORMAT	ddpf;
	int				height;
	UINT			bitcolor;

	ZeroMemory(&scrnmng, sizeof(scrnmng));
	winstyle = GetWindowLong(hWndMain, GWL_STYLE);
	winstyleex = GetWindowLong(hWndMain, GWL_EXSTYLE);
	hmenu = GetMenu(hWndMain);
	if (mode & SCRNMODE_FULLSCREEN) {
		scrnmng.flag = SCRNFLAG_FULLSCREEN;
		winstyle &= ~(WS_CAPTION | WS_OVERLAPPED | WS_SYSMENU);
		winstyle |= WS_POPUP;
		winstyleex |= WS_EX_TOPMOST;
		CheckMenuItem(hmenu, IDM_WINDOW, MF_UNCHECKED);
		CheckMenuItem(hmenu, IDM_FULLSCREEN, MF_CHECKED);
		ddraw.menudisp = FALSE;
		ddraw.menusize = GetSystemMetrics(SM_CYMENU);
		extclass_enablemenu(hWndMain, FALSE);
	}
	else {
//		scrnmng.flag = 0;
		winstyle |= WS_SYSMENU;
		winstyle |= WS_CAPTION;
		winstyle &= ~WS_POPUP;
		winstyleex &= ~WS_EX_TOPMOST;
		CheckMenuItem(hmenu, IDM_WINDOW, MF_CHECKED);
		CheckMenuItem(hmenu, IDM_FULLSCREEN, MF_UNCHECKED);
	}
	SetWindowLong(hWndMain, GWL_STYLE, winstyle);
	SetWindowLong(hWndMain, GWL_EXSTYLE, winstyleex);

	if (DirectDrawCreate(NULL, &ddraw.ddraw1, NULL) != DD_OK) {
		goto scre_err;
	}
	ddraw.ddraw1->QueryInterface(IID_IDirectDraw2, (void **)&ddraw2);
	ddraw.ddraw2 = ddraw2;

	if (mode & SCRNMODE_FULLSCREEN) {
#if defined(SUPPORT_DCLOCK)
		dclock_init();
#endif	// defined(SUPPORT_DCLOCK)
		ddraw2->SetCooperativeLevel(hWndMain,
										DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN);
		height = 480;
		bitcolor = 8;
		if (mode & (SCRNMODE_SYSHIGHCOLOR | SCRNMODE_COREHIGHCOLOR)) {
			bitcolor = 16;
		}
		if (ddraw2->SetDisplayMode(SURFACE_WIDTH, height,
												bitcolor, 0, 0) != DD_OK) {
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
		ddsd.dwHeight = height;
		if (ddraw2->CreateSurface(&ddsd, &ddraw.backsurf, NULL) != DD_OK) {
			goto scre_err;
		}
		if (bitcolor == 8) {
			paletteinit();
		}
		else {
			make16mask(ddpf.dwBBitMask, ddpf.dwRBitMask, ddpf.dwGBitMask);
		}

#if defined(SUPPORT_DCLOCK)
		if (bitcolor == 8) {
			dclock_init8();
		}
		else {
			dclock_init16();
		}
		ZeroMemory(&ddsd, sizeof(ddsd));
		ddsd.dwSize = sizeof(ddsd);
		ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;
		ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
		ddsd.dwWidth = DCLOCK_X;
		ddsd.dwHeight = DCLOCK_Y;
		if (ddraw2->CreateSurface(&ddsd, &ddraw.clocksurf, NULL) != DD_OK) {
			ddraw.clocksurf = NULL;
		}
		dclock_reset();
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
	ddraw.scrnmode = mode;
	ddraw.width = SURFACE_WIDTH;
	ddraw.height = height;
	ddraw.cliping = 0;
	renewalclientsize(FALSE);
	return(SUCCESS);

scre_err:
	scrnmng_destroy();
	return(FAILURE);
}

void scrnmng_destroy(void) {					// ddraws_TermDirectDraw

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

BRESULT scrnmng_changescreen(REG8 newmode) {

	BOOL	renewal;
	REG8	change;
	BRESULT	r;

	renewal = FALSE;
	if (scrnmode & (SCRNMODE_SYSHIGHCOLOR | SCRNMODE_COREHIGHCOLOR)) {
		renewal = !renewal;
	}
	if (newmode & (SCRNMODE_SYSHIGHCOLOR | SCRNMODE_COREHIGHCOLOR)) {
		renewal = !renewal;
	}

	change = scrnmode ^ newmode;
	if (change & SCRNMODE_FULLSCREEN) {
		renewal = TRUE;
	}
	r = SUCCESS;
	if (renewal) {
		soundmng_stop();
		mousemng_disable(MOUSEPROC_WINUI);
		scrnmng_destroy();
		if (scrnmng_create(newmode) == SUCCESS) {
			scrnmode = newmode;
		}
		else {
			r = FAILURE;
			if (scrnmng_create(scrnmode) != SUCCESS) {
				PostQuitMessage(0);
				return(r);
			}
		}
		scrndraw_redraw();
		mousemng_enable(MOUSEPROC_WINUI);
		soundmng_play();
	}
	else {
		scrnmode = newmode;
	}
	return(r);
}

void scrnmng_querypalette(void) {				// ddraws_palette

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

	BOOL	menudisp;

	if (scrnmng.flag & SCRNFLAG_FULLSCREEN) {
		menudisp = ((y >= 0) && (y < ddraw.menusize))?TRUE:FALSE;
		if (ddraw.menudisp != menudisp) {
			ddraw.menudisp = menudisp;
			if (menudisp == 1) {
				extclass_enablemenu(hWndMain, TRUE);
			}
			else {
				extclass_enablemenu(hWndMain, FALSE);
				clearoutfullscreen();
			}
		}
	}
}

void scrnmng_topwinui(void) {						// ddraws_topwinui

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

void scrnmng_clearwinui(void) {						// ddraws_clearwinui

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

void clearblanklines(DWORD posx, DWORD posy) {

	DDBLTFX		ddbf;
	RECT		fillrect;

	fillrect.left = posx;
	fillrect.top = posy + ddraw.rect.bottom;
	fillrect.right = posx + SURFACE_WIDTH;
	fillrect.bottom = posy + SURFACE_HEIGHT;

	ZeroMemory(&ddbf, sizeof(ddbf));
	ddbf.dwSize = sizeof(ddbf);
	ddbf.dwFillColor = 0;
	ddraw.primsurf->Blt(&fillrect, NULL, NULL, DDBLT_COLORFILL, &ddbf);
}

void scrnmng_setheight(int posy, int height) {

	scrnstat.height = height;
	renewalclientsize(TRUE);
}

BRESULT scrnmng_setcolormode(BRESULT fullcolor) {

	REG8	newmode;

	newmode = scrnmode & (~(SCRNMODE_COREHIGHCOLOR));
	if (fullcolor) {
		newmode |= SCRNMODE_COREHIGHCOLOR;
	}
	return(scrnmng_changescreen(newmode));
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
			dst = ddraw.scrn;
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
		}
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


// -------------------------------------------------------------------- clock

#if defined(SUPPORT_DCLOCK)

static const RECT rectclk = {0, 0, DCLOCK_X, DCLOCK_Y};

void scrnmng_dispclock(void)
{
	DDSURFACEDESC	dest;

	if ((ddraw.clocksurf) &&
		(ddraw.scrn.top >= DCLOCK_Y) && (dclock_disp())) {
		dclock_make();
		ZeroMemory(&dest, sizeof(dest));
		dest.dwSize = sizeof(dest);
		if (ddraw.clocksurf->Lock(NULL, &dest, DDLOCK_WAIT, NULL) == DD_OK) {
			if (scrnmng.bpp == 8) {
				dclock_out8(dest.lpSurface, dest.lPitch);
			}
			else {
				dclock_out16(dest.lpSurface, dest.lPitch);
			}
			ddraw.clocksurf->Unlock(NULL);
		}

		if (ddraw.primsurf->BltFast(640 - DCLOCK_X - 4,
									ddraw.height - DCLOCK_Y,
									ddraw.clocksurf, (RECT *)&rectclk,
									DDBLTFAST_WAIT) == DDERR_SURFACELOST) {
			ddraw.primsurf->Restore();
			ddraw.clocksurf->Restore();
		}
		dclock_cntdown(xmiloscfg.DRAW_SKIP);
	}
}
#endif	// defined(SUPPORT_DCLOCK)

