#include	"compiler.h"
#include	<ddraw.h>
#ifndef __GNUC__
#include	<winnls32.h>
#endif


typedef struct {
	HWND				hWnd;
	UINT				bitcolor;
	UINT				mode;
	LPDIRECTDRAW		ddraw1;
	LPDIRECTDRAW2		ddraw2;
	LPDIRECTDRAWSURFACE	primsurf;
	LPDIRECTDRAWSURFACE	backsurf;
	LPDIRECTDRAWCLIPPER	clipper;
	int					cliping;
	RECT				scrn;
	RECT				rect;
} DDRAW;

typedef struct {
	UINT16	palette[0x200];			// 1KB
	UINT8	buffer[0x18000];		// 96KB
	union {
		UINT16	pal16[0x8000];
		UINT32	pal32[0x8000];
	}	p;
} ADVV;


		UINT16	*ADVVIDEOPAL;
		UINT8	*ADVVIDEOBUFFER;
static	DDRAW	ddraw;
static	ADVV	*advv;


static void setwindowsize(HWND hWnd, int width, int height) {

	RECT	workrc;
	int		scx;
	int		scy;
	UINT	cnt;
	RECT	rectwindow;
	RECT	rectclient;
	int		cx;
	int		cy;
	int		winx;
	int		winy;
	UINT	update;

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
			winx = rectwindow.left;
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
			winy = rectwindow.top;
			if ((winy + cy) > workrc.bottom) {
				winy = workrc.bottom - cy;
			}
			if (winy < workrc.top) {
				winy = workrc.top;
			}
		}
		MoveWindow(hWnd, winx, winy, cx, cy, TRUE);
	} while(--cnt);
}

static void advv16(ADVV *av, const DDPIXELFORMAT *ddpf) {

	UINT32	bmask;
	UINT8	b16r;
	UINT32	rmask;
	UINT8	r16l;
	UINT32	gmask;
	UINT8	g16l;
	UINT	i;
	UINT32	r;
	UINT32	g;
	UINT32	b;

	bmask = ddpf->dwBBitMask;
	b16r = 0;
	while((!(bmask & 0x80)) && (b16r < 32)) {
		bmask <<= 1;
		b16r++;
	}
	rmask = ddpf->dwRBitMask;
	r16l = 0;
	while((rmask & 0xffffff00) && (r16l < 32)) {
		rmask >>= 1;
		r16l++;
	}
	gmask = ddpf->dwGBitMask;
	g16l = 0;
	while((gmask & 0xffffff00) && (g16l < 32)) {
		gmask >>= 1;
		g16l++;
	}

	// …で、全パレット作る（ぉ
	for (i=0; i<0x8000; i++) {
		r = (i << 3) & 0xf8;
		g = (i >> 2) & 0xf8;
		b = (i >> 7) & 0xf8;
		r = (r + (r >> 5)) & rmask;
		g = (g + (g >> 5)) & gmask;
		b = (b + (b >> 5)) & bmask;
		av->p.pal16[i] = (r << r16l) + (g << g16l) + (b >> b16r);
	}
}

static void advv32(ADVV *av, const DDPIXELFORMAT *ddpf) {

	UINT	i;
	UINT32	r;
	UINT32	g;
	UINT32	b;

	// …やっぱりパレット作る
	for (i=0; i<0x8000; i++) {
		r = (i << 3) & 0xf8;
		g = (i >> 2) & 0xf8;
		b = (i >> 7) & 0xf8;
		r = (r + (r >> 5));
		g = (g + (g >> 5));
		b = (b + (b >> 5));
		av->p.pal32[i] = (r << 16) + (g << 8) + b;
	}
}

static BRESULT bgen(UINT mode) {

	return((mode & ADVV_BG2)?TRUE:FALSE);
}

static void mode3_16(UINT mode, const ADVV *av, const DDSURFACEDESC *dsd) {

const UINT8	*p;
	UINT8	*q;
	UINT	y;
	UINT	x;
	UINT	rgb;

	p = av->buffer;
	q = (UINT8 *)dsd->lpSurface;
	y = ADVV_HEIGHT;
	do {
		for (x=0; x<ADVV_WIDTH; x++) {
			rgb = p[x*2+0] + ((p[x*2+1] & 0x7f) << 8);
			((UINT16 *)q)[x] = av->p.pal16[rgb];
		}
		p += ADVV_WIDTH * 2;
		q += dsd->lPitch;
	} while(--y);
}

static void mode3_32(UINT mode, const ADVV *av, const DDSURFACEDESC *dsd) {

const UINT8	*p;
	UINT8	*q;
	UINT	y;
	UINT	x;
	UINT	rgb;

	p = av->buffer;
	q = (UINT8 *)dsd->lpSurface;
	y = ADVV_HEIGHT;
	do {
		for (x=0; x<ADVV_WIDTH; x++) {
			rgb = p[x*2+0] + ((p[x*2+1] & 0x7f) << 8);
			((UINT32 *)q)[x] = av->p.pal32[rgb];
		}
		p += ADVV_WIDTH * 2;
		q += dsd->lPitch;
	} while(--y);
}

static void mode4_16(UINT mode, const ADVV *av, const DDSURFACEDESC *dsd) {

	UINT	i;
	UINT16	pal[256];
const UINT8	*p;
	UINT8	*q;
	UINT	y;
	UINT	x;

	// 先にスタックにつっこんどいて…
	for (i=0; i<256; i++) {
		pal[i] = av->p.pal16[av->palette[i] & 0x7fff];
	}
	p = av->buffer;
	if (mode & ADVV_PLANE) {
		p += 0xa000;
	}
	q = (UINT8 *)dsd->lpSurface;
	y = ADVV_HEIGHT;
	do {
		for (x=0; x<ADVV_WIDTH; x++) {
			((UINT16 *)q)[x] = pal[p[x]];
		}
		p += ADVV_WIDTH;
		q += dsd->lPitch;
	} while(--y);
}

static void mode4_32(UINT mode, const ADVV *av, const DDSURFACEDESC *dsd) {

	UINT	i;
	UINT32	pal[256];
const UINT8	*p;
	UINT8	*q;
	UINT	y;
	UINT	x;

	// 先にスタックにつっこんどいて…
	for (i=0; i<256; i++) {
		pal[i] = av->p.pal32[av->palette[i] & 0x7fff];
	}
	p = av->buffer;
	if (mode & ADVV_PLANE) {
		p += 0xa000;
	}
	q = (UINT8 *)dsd->lpSurface;
	y = ADVV_HEIGHT;
	do {
		for (x=0; x<ADVV_WIDTH; x++) {
			((UINT32 *)q)[x] = pal[p[x]];
		}
		p += ADVV_WIDTH;
		q += dsd->lPitch;
	} while(--y);
}

typedef void (*DRAWFN)(UINT mode, const ADVV *av, const DDSURFACEDESC *dsd);
typedef struct {
	BRESULT	(*reqdraw)(UINT mode);
	DRAWFN	draw16;
	DRAWFN	draw32;
} ADVDRAW;

static const ADVDRAW advdraw[6] = {
		{NULL,		NULL,		NULL},
		{NULL,		NULL,		NULL},
		{NULL,		NULL,		NULL},
		{bgen,		mode3_16,	mode3_32},
		{bgen,		mode4_16,	mode4_32},
		{bgen,		NULL,		NULL}};


// ----

void advv_setmode(UINT16 mode) {

	ddraw.mode = mode;
}

void advv_palupdate(void) {

	advv_bufferupdate();
}

void advv_bufferupdate(void) {

	UINT			mode;
	UINT			scrnmode;
const ADVDRAW		*draw;
	DRAWFN			fn;
	DDSURFACEDESC	destscrn;
	HRESULT			r;

	if (ddraw.backsurf == NULL) {
		return;
	}

	mode = ddraw.mode;
	scrnmode = mode & 0x0f;
	if (scrnmode >= NELEMENTS(advdraw)) {
		return;
	}
	draw = advdraw + scrnmode;
	if ((draw->reqdraw == NULL) || (!(*(draw->reqdraw))(mode))) {
		return;
	}
	if (ddraw.bitcolor == 16) {
		fn = draw->draw16;
	}
	else if (ddraw.bitcolor == 32) {
		fn = draw->draw32;
	}
	else {
		fn = NULL;
	}
	if (fn == NULL) {
		return;
	}

	ZeroMemory(&destscrn, sizeof(destscrn));
	destscrn.dwSize = sizeof(destscrn);
	r = ddraw.backsurf->Lock(NULL, &destscrn, DDLOCK_WAIT, NULL);
	if (r != DD_OK) {
		return;
	}
	(*fn)(mode, advv, &destscrn);
	ddraw.backsurf->Unlock(NULL);
	advv_update();
}


// ----

BRESULT advv_initialize(HWND hWnd) {

	DWORD			winstyle;
	DWORD			winstyleex;
	ADVV			*av;
	LPDIRECTDRAW2	ddraw2;
	DDSURFACEDESC	ddsd;
	DDPIXELFORMAT	ddpf;
	UINT			bitcolor;

	winstyle = GetWindowLong(hWnd, GWL_STYLE);
	winstyleex = GetWindowLong(hWnd, GWL_EXSTYLE);
	winstyle |= WS_SYSMENU | WS_CAPTION;
	winstyle &= ~WS_POPUP;
	winstyleex &= ~WS_EX_TOPMOST;
	SetWindowLong(hWnd, GWL_STYLE, winstyle);
	SetWindowLong(hWnd, GWL_EXSTYLE, winstyleex);
	setwindowsize(hWnd, ADVV_WIDTH, ADVV_HEIGHT);

	av = (ADVV *)_MALLOC(sizeof(ADVV), "ADVV");
	advv = av;
	if (av == NULL) {
		goto scre_err;
	}
	ZeroMemory(av, sizeof(ADVV));
	ADVVIDEOPAL = av->palette;
	ADVVIDEOBUFFER = av->buffer;

	if (DirectDrawCreate(NULL, &ddraw.ddraw1, NULL) != DD_OK) {
		goto scre_err;
	}
	ddraw.ddraw1->QueryInterface(IID_IDirectDraw2, (void **)&ddraw2);
	ddraw.ddraw2 = ddraw2;

	ddraw2->SetCooperativeLevel(hWnd, DDSCL_NORMAL);

	ZeroMemory(&ddsd, sizeof(ddsd));
	ddsd.dwSize = sizeof(ddsd);
	ddsd.dwFlags = DDSD_CAPS;
	ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
	if (ddraw2->CreateSurface(&ddsd, &ddraw.primsurf, NULL) != DD_OK) {
		goto scre_err;
	}

	ddraw2->CreateClipper(0, &ddraw.clipper, NULL);
	ddraw.clipper->SetHWnd(0, hWnd);
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
	ddsd.dwWidth = ADVV_WIDTH;
	ddsd.dwHeight = ADVV_HEIGHT;

	if (ddraw2->CreateSurface(&ddsd, &ddraw.backsurf, NULL) != DD_OK) {
		goto scre_err;
	}
	bitcolor = ddpf.dwRGBBitCount;
	if (bitcolor == 16) {
		advv16(av, &ddpf);
	}
	else if (bitcolor == 32) {
		advv32(av, &ddpf);
	}
	else {
		goto scre_err;
	}

	ddraw.hWnd = hWnd;
	ddraw.bitcolor = bitcolor;
	return(SUCCESS);

scre_err:
	advv_deinitialize();
	return(FAILURE);
}

void advv_deinitialize(void) {

	if (ddraw.backsurf) {
		ddraw.backsurf->Release();
	}
	if (ddraw.clipper) {
		ddraw.clipper->Release();
	}
	if (ddraw.primsurf) {
		ddraw.primsurf->Release();
	}
	if (ddraw.ddraw2) {
		ddraw.ddraw2->Release();
	}
	if (ddraw.ddraw1) {
		ddraw.ddraw1->Release();
	}
	ZeroMemory(&ddraw, sizeof(ddraw));
	if (advv) {
		_MFREE(advv);
		advv = NULL;
	}
}

void advv_update(void) {

	POINT	clip;
	RECT	dst;
	RECT	src;
	HRESULT	r;

	if (ddraw.backsurf != NULL) {
		clip.x = 0;
		clip.y = 0;
		ClientToScreen(ddraw.hWnd, &clip);
		dst.left = clip.x;
		dst.top = clip.y;
		dst.right = clip.x + ADVV_WIDTH;
		dst.bottom = clip.y + ADVV_HEIGHT;
		src.left = 0;
		src.top = 0;
		src.right = ADVV_WIDTH;
		src.bottom = ADVV_HEIGHT;
		r = ddraw.primsurf->Blt(&dst, ddraw.backsurf, &src,
														DDBLT_WAIT, NULL);
		if (r == DDERR_SURFACELOST) {
			ddraw.backsurf->Restore();
			ddraw.primsurf->Restore();
			ddraw.primsurf->Blt(&dst, ddraw.backsurf, &src,
														DDBLT_WAIT, NULL);
		}
	}
}

void advv_topwinui(void) {

	if (!ddraw.cliping++) {
#ifndef __GNUC__
		WINNLSEnableIME(ddraw.hWnd, TRUE);
#endif
	}
}

void advv_clearwinui(void) {

	if ((ddraw.cliping > 0) && (!(--ddraw.cliping))) {
#ifndef __GNUC__
		WINNLSEnableIME(ddraw.hWnd, FALSE);
#endif
	}
}

