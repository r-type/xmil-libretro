#include "compiler.h"
#include "libnds.h"
#include <ddraw.h>


#pragma comment(lib, "ddraw.lib")
#pragma comment(lib, "dxguid.lib")

#define NDSVIDEO_WIDTH		SCREEN_WIDTH
#define NDSVIDEO_HEIGHT		SCREEN_HEIGHT

	CRegScreen16 g_bgpalettesub;
	CRegVram16 g_vram[0x80000];
	CRegPalette16 g_pal[256];
	CRegPalette16 g_palsub[256];
	tagNdsBgReg g_bg[4];
	tagNdsBgReg g_subbg[4];

static UINT s_uMode;

void videoSetMode(uint32 mode)
{
	s_uMode = mode;
}

void videoSetModeSub(uint32 mode)
{
}

uint32 vramSetMainBanks(VRAM_A_TYPE a, VRAM_B_TYPE b, VRAM_C_TYPE c, VRAM_D_TYPE d)
{
	return 0;
}


// ----

struct tagDDraw
{
	HWND				hWnd;
	UINT				uBitColor;
	LPDIRECTDRAW		pDDraw;
	LPDIRECTDRAW2		pDDraw2;
	LPDIRECTDRAWSURFACE	pPrimSurf;
	LPDIRECTDRAWSURFACE	pBackSurf;
	LPDIRECTDRAWCLIPPER	pClipper;
};

union tagPalTable
{
	UINT16	pal16[0x8000];
	UINT32	pal32[0x8000];
};

struct tagVideoParam
{
	tagPalTable p;
};

static tagDDraw s_ddraw;
static tagVideoParam *s_pVideo;

static void setwindowsize(HWND hWnd, int nWidth, int nHeight)
{
	RECT rcWorkArea;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rcWorkArea, 0);
	const int nScreenWidth = GetSystemMetrics(SM_CXSCREEN);
	const int nScreenHeight = GetSystemMetrics(SM_CYSCREEN);

	int nCount = 2;
	do
	{
		RECT rcWindow;
		GetWindowRect(hWnd, &rcWindow);

		RECT rcClient;
		GetClientRect(hWnd, &rcClient);

		int nNewWidth = nWidth;
		nNewWidth += rcWindow.right - rcWindow.left;
		nNewWidth -= rcClient.right - rcClient.left;
		int nNewHeight = nHeight;
		nNewHeight += rcWindow.bottom - rcWindow.top;
		nNewHeight -= rcClient.bottom - rcClient.top;

		int nPosX = rcWindow.left;

		if (nScreenWidth < nNewWidth)
		{
			nPosX = (nScreenWidth - nNewWidth) / 2;
		}
		else
		{
			if ((nPosX + nNewWidth) > rcWorkArea.right)
			{
				nPosX = rcWorkArea.right - nNewWidth;
			}
			if (nPosX < rcWorkArea.left)
			{
				nPosX = rcWorkArea.left;
			}
		}

		int nPosY = rcWindow.top;
		if (nScreenHeight < nNewHeight)
		{
			nPosY = (nScreenHeight - nNewHeight) / 2;
		}
		else
		{
			if ((nPosY + nNewHeight) > rcWorkArea.bottom)
			{
				nPosY = rcWorkArea.bottom - nNewHeight;
			}
			if (nPosY < rcWorkArea.top)
			{
				nPosY = rcWorkArea.top;
			}
		}
		MoveWindow(hWnd, nPosX, nPosY, nNewWidth, nNewHeight, TRUE);
	} while(--nCount);
}

static void initPal16(tagPalTable &pal, const DDPIXELFORMAT &ddpf)
{
	UINT32 bmask = ddpf.dwBBitMask;
	UINT b16r = 0;
	while((!(bmask & 0x80)) && (b16r < 32))
	{
		bmask <<= 1;
		b16r++;
	}
	UINT32 rmask = ddpf.dwRBitMask;
	UINT r16l = 0;
	while((rmask & 0xffffff00) && (r16l < 32))
	{
		rmask >>= 1;
		r16l++;
	}
	UINT32 gmask = ddpf.dwGBitMask;
	UINT g16l = 0;
	while((gmask & 0xffffff00) && (g16l < 32))
	{
		gmask >>= 1;
		g16l++;
	}

	// …で、全パレット作る（ぉ
	for (UINT i=0; i<0x8000; i++)
	{
		UINT r = (i << 3) & 0xf8;
		UINT g = (i >> 2) & 0xf8;
		UINT b = (i >> 7) & 0xf8;
		r = (r + (r >> 5)) & rmask;
		g = (g + (g >> 5)) & gmask;
		b = (b + (b >> 5)) & bmask;
		pal.pal16[i] = (r << r16l) + (g << g16l) + (b >> b16r);
	}
}

static void initPal32(tagPalTable &pal)
{
	// …やっぱりパレット作る
	for (UINT i=0; i<0x8000; i++)
	{
		UINT r = (i << 3) & 0xf8;
		UINT g = (i >> 2) & 0xf8;
		UINT b = (i >> 7) & 0xf8;
		r = (r + (r >> 5));
		g = (g + (g >> 5));
		b = (b + (b >> 5));
		pal.pal32[i] = (r << 16) + (g << 8) + b;
	}
}


// ----

static void drawext_16(const tagVideoParam &video, tagNdsBgReg &bg, const DDSURFACEDESC &dsd)
{
	// 先にスタックにつっこんどいて…
	UINT16 pal[256];
	for (int i=0; i<256; i++)
	{
		pal[i] = video.p.pal16[g_pal[i] & 0x7fff];
	}

	UINT uWidth = 0;
	UINT uHeight = 0;
	switch((bg.cr >> 14) & 3)
	{
		case BG_RS_16x16 >> 14:
			uWidth = 128;
			uHeight = 128;
			break;

		case BG_RS_32x32 >> 14:
			uWidth = 256;
			uHeight = 256;
			break;

		case BG_RS_64x64 >> 14:
			uWidth = 512;
			uHeight = 256;
			break;

		case BG_RS_128x128 >> 14:
			uWidth = 512;
			uHeight = 512;
			break;
	}
	UINT uAlign = uWidth;
	uWidth = min(uWidth, NDSVIDEO_WIDTH);
	uHeight = min(uHeight, NDSVIDEO_HEIGHT);

	const UINT8 *p = reinterpret_cast<UINT8 *>(g_vram);
	UINT8 *q = reinterpret_cast<UINT8 *>(dsd.lpSurface);

	for (UINT y=0; y<uHeight; y++)
	{
		for (UINT x=0; x<uWidth; x++)
		{
			(reinterpret_cast<UINT16 *>(q))[x] = pal[p[x]];
		}
		p += uAlign;
		q += dsd.lPitch;
	}
}

static void drawext_32(const tagVideoParam &video, tagNdsBgReg &bg, const DDSURFACEDESC &dsd)
{
	// 先にスタックにつっこんどいて…
	UINT32 pal[256];
	for (int i=0; i<256; i++)
	{
		pal[i] = video.p.pal32[g_pal[i] & 0x7fff];
	}

	UINT uWidth = 0;
	UINT uHeight = 0;
	switch((bg.cr >> 14) & 3)
	{
		case BG_RS_16x16 >> 14:
			uWidth = 128;
			uHeight = 128;
			break;

		case BG_RS_32x32 >> 14:
			uWidth = 256;
			uHeight = 256;
			break;

		case BG_RS_64x64 >> 14:
			uWidth = 512;
			uHeight = 256;
			break;

		case BG_RS_128x128 >> 14:
			uWidth = 512;
			uHeight = 512;
			break;
	}
	UINT uAlign = uWidth;
	uWidth = min(uWidth, NDSVIDEO_WIDTH);
	uHeight = min(uHeight, NDSVIDEO_HEIGHT);

	const UINT8 *p = reinterpret_cast<UINT8 *>(g_vram);
	UINT8 *q = reinterpret_cast<UINT8 *>(dsd.lpSurface);

	for (UINT y=0; y<uHeight; y++)
	{
		for (UINT x=0; x<uWidth; x++)
		{
			(reinterpret_cast<UINT32 *>(q))[x] = pal[p[x]];
		}
		p += uAlign;
		q += dsd.lPitch;
	}
}

// ----

static void mode5_16(const tagVideoParam &video, const DDSURFACEDESC &dsd)
{
	drawext_16(video, g_bg[3], dsd);
}


static void mode5_32(const tagVideoParam &video, const DDSURFACEDESC &dsd)
{
	drawext_32(video, g_bg[3], dsd);
}


// ----

typedef void (*DRAWFN)(const tagVideoParam &video, const DDSURFACEDESC &dsd);
struct tagNdsDraw
{
	DRAWFN	draw16;
	DRAWFN	draw32;
};

static const tagNdsDraw ndsdraw[6] =
{
	{0,			0},
	{0,			0},
	{0,			0},
	{0,			0},
	{0,			0},
	{mode5_16,	mode5_32},
};

void ndsvideo_bufferupdate()
{
	tagDDraw &ddraw = s_ddraw;
	if (!ddraw.pBackSurf)
	{
		return;
	}

	const UINT uMode = s_uMode & 0x0f;
	if (uMode >= NELEMENTS(ndsdraw))
	{
		return;
	}
	const tagNdsDraw &draw = ndsdraw[uMode];
	DRAWFN fn = 0;
	if (ddraw.uBitColor == 16)
	{
		fn = draw.draw16;
	}
	else if (ddraw.uBitColor == 32)
	{
		fn = draw.draw32;
	}
	if (!fn)
	{
		return;
	}

	DDSURFACEDESC dsd;
	ZeroMemory(&dsd, sizeof(dsd));
	dsd.dwSize = sizeof(dsd);
	HRESULT hResult = ddraw.pBackSurf->Lock(NULL, &dsd, DDLOCK_WAIT, NULL);
	if (hResult != DD_OK)
	{
		return;
	}
	(*fn)(*s_pVideo, dsd);
	ddraw.pBackSurf->Unlock(NULL);
	ndsvideo_update();
}


// ----

static bool _initialize(HWND hWnd)
{
	DWORD dwStyle = GetWindowLong(hWnd, GWL_STYLE);
	DWORD dwExStyle = GetWindowLong(hWnd, GWL_EXSTYLE);
	dwStyle |= WS_SYSMENU | WS_CAPTION;
	dwStyle &= ~WS_POPUP;
	dwExStyle &= ~WS_EX_TOPMOST;
	SetWindowLong(hWnd, GWL_STYLE, dwStyle);
	SetWindowLong(hWnd, GWL_EXSTYLE, dwExStyle);
	setwindowsize(hWnd, NDSVIDEO_WIDTH, NDSVIDEO_HEIGHT);

	tagVideoParam *pVideo = new tagVideoParam;
	s_pVideo = pVideo;
	if (!pVideo)
	{
		return false;
	}
	ZeroMemory(pVideo, sizeof(*pVideo));

	tagDDraw &ddraw = s_ddraw;
	if (DirectDrawCreate(NULL, &ddraw.pDDraw, NULL) != DD_OK)
	{
		return false;
	}
	LPDIRECTDRAW2 pDDraw2;
	ddraw.pDDraw->QueryInterface(IID_IDirectDraw2, (void **)&pDDraw2);
	ddraw.pDDraw2 = pDDraw2;

	pDDraw2->SetCooperativeLevel(hWnd, DDSCL_NORMAL);

	DDSURFACEDESC ddsd;
	ZeroMemory(&ddsd, sizeof(ddsd));
	ddsd.dwSize = sizeof(ddsd);
	ddsd.dwFlags = DDSD_CAPS;
	ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
	if (pDDraw2->CreateSurface(&ddsd, &ddraw.pPrimSurf, NULL) != DD_OK)
	{
		return false;
	}

	pDDraw2->CreateClipper(0, &ddraw.pClipper, NULL);
	ddraw.pClipper->SetHWnd(0, hWnd);
	ddraw.pPrimSurf->SetClipper(ddraw.pClipper);

	DDPIXELFORMAT ddpf;
	ZeroMemory(&ddpf, sizeof(ddpf));
	ddpf.dwSize = sizeof(DDPIXELFORMAT);
	if (ddraw.pPrimSurf->GetPixelFormat(&ddpf) != DD_OK)
	{
		return false;
	}

	ZeroMemory(&ddsd, sizeof(ddsd));
	ddsd.dwSize = sizeof(ddsd);
	ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;
	ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
	ddsd.dwWidth = NDSVIDEO_WIDTH;
	ddsd.dwHeight = NDSVIDEO_HEIGHT;

	if (pDDraw2->CreateSurface(&ddsd, &ddraw.pBackSurf, NULL) != DD_OK)
	{
		return false;
	}
	UINT uBitColor = ddpf.dwRGBBitCount;
	if (uBitColor == 16)
	{
		initPal16(pVideo->p, ddpf);
	}
	else if (uBitColor == 32)
	{
		initPal32(pVideo->p);
	}
	else
	{
		return false;
	}

	ddraw.hWnd = hWnd;
	ddraw.uBitColor = uBitColor;
	return true;
}

static void _deinitialize()
{
	tagDDraw &ddraw = s_ddraw;
	if (ddraw.pBackSurf)
	{
		ddraw.pBackSurf->Release();
	}
	if (ddraw.pClipper)
	{
		ddraw.pClipper->Release();
	}
	if (ddraw.pPrimSurf)
	{
		ddraw.pPrimSurf->Release();
	}
	if (ddraw.pDDraw2)
	{
		ddraw.pDDraw2->Release();
	}
	if (ddraw.pDDraw)
	{
		ddraw.pDDraw->Release();
	}
	ZeroMemory(&ddraw, sizeof(ddraw));

	if (s_pVideo)
	{
		delete s_pVideo;
		s_pVideo = 0;
	}
}

bool ndsvideo_initialize(HWND hWnd)
{
	bool bResult = _initialize(hWnd);
	if (!bResult)
	{
		_deinitialize();
	}
	return bResult;
}
void ndsvideo_deinitialize()
{
	_deinitialize();
}

void ndsvideo_update()
{
	tagDDraw &ddraw = s_ddraw;
	if (ddraw.pBackSurf)
	{
		POINT ptClip;
		ptClip.x = 0;
		ptClip.y = 0;
		ClientToScreen(ddraw.hWnd, &ptClip);

		RECT rtDst;
		rtDst.left = ptClip.x;
		rtDst.top = ptClip.y;
		rtDst.right = ptClip.x + NDSVIDEO_WIDTH;
		rtDst.bottom = ptClip.y + NDSVIDEO_HEIGHT;

		RECT rtSrc;
		rtSrc.left = 0;
		rtSrc.top = 0;
		rtSrc.right = NDSVIDEO_WIDTH;
		rtSrc.bottom = NDSVIDEO_HEIGHT;
		HRESULT hResult = ddraw.pPrimSurf->Blt(&rtDst, ddraw.pBackSurf, &rtSrc, DDBLT_WAIT, NULL);
		if (hResult == DDERR_SURFACELOST)
		{
			ddraw.pBackSurf->Restore();
			ddraw.pPrimSurf->Restore();
			ddraw.pPrimSurf->Blt(&rtDst, ddraw.pBackSurf, &rtSrc, DDBLT_WAIT, NULL);
		}
	}
}

