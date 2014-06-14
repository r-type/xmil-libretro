#include	"compiler.h"
#include	"fontmng.h"


typedef struct {
	int			fontsize;
	UINT		fonttype;
	int			fontwidth;
	int			fontheight;

// ���Ƃ͊g���`
	HDC			hdcimage;
	HBITMAP		hBitmap;
	BYTE		*image;
	HFONT		hfont;
	RECT		rect;
	int			bmpwidth;
	int			bmpheight;
	int			bmpalign;
} _FNTMNG, *FNTMNG;


static const OEMCHAR deffontface[] = OEMTEXT("�l�r �S�V�b�N");
static const OEMCHAR deffontface2[] = OEMTEXT("�l�r �o�S�V�b�N");


void *fontmng_create(int size, UINT type, const OEMCHAR *fontface) {

	int			i;
	int			fontalign;
	int			allocsize;
	FNTMNG		ret;
	BITMAPINFO	*bi;
	HDC			hdc;
	int			fontwidth;
	int			fontheight;
	int			weight;
	DWORD		pitch;

	if (size < 0) {
		size *= -1;
	}
	if (size < 6) {
		size = 6;
	}
	else if (size > 128) {
		size = 128;
	}
	fontwidth = size;
	fontheight = size;
	if (type & FDAT_BOLD) {
		fontwidth++;
	}

	fontalign = sizeof(_FNTDAT) + (fontwidth * fontheight);
	fontalign = (fontalign + 3) & (~3);

	allocsize = sizeof(_FNTMNG);
	allocsize += fontalign;
	allocsize += sizeof(BITMAPINFOHEADER) + (4 * 2);

	ret = (FNTMNG)_MALLOC(allocsize, "font mng");
	if (ret == NULL) {
		return(NULL);
	}
	ZeroMemory(ret, allocsize);
	ret->fontsize = size;
	ret->fonttype = type;
	ret->fontwidth = fontwidth;
	ret->fontheight = fontheight;
	ret->bmpwidth = fontwidth;
	ret->bmpheight = fontheight;
	ret->bmpalign = (((ret->bmpwidth + 31) / 8) & ~3);


	bi = (BITMAPINFO *)(((BYTE *)(ret + 1)) + fontalign);
	bi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bi->bmiHeader.biWidth = ret->bmpwidth;
	bi->bmiHeader.biHeight = ret->bmpheight;
	bi->bmiHeader.biPlanes = 1;
	bi->bmiHeader.biBitCount = 1;
	bi->bmiHeader.biCompression = BI_RGB;
	bi->bmiHeader.biSizeImage = ret->bmpalign * ret->bmpheight;
	bi->bmiHeader.biXPelsPerMeter = 0;
	bi->bmiHeader.biYPelsPerMeter = 0;
	bi->bmiHeader.biClrUsed = 2;
	bi->bmiHeader.biClrImportant = 2;
	for (i=0; i<2; i++) {
		bi->bmiColors[i].rgbRed = (i ^ 1) - 1;
		bi->bmiColors[i].rgbGreen = (i ^ 1) - 1;
		bi->bmiColors[i].rgbBlue = (i ^ 1) - 1;
		bi->bmiColors[i].rgbReserved = PC_RESERVED;
	}

    hdc = GetDC(NULL);
	ret->hBitmap = CreateDIBSection(hdc, bi, DIB_RGB_COLORS,
											(void **)&ret->image, NULL, 0);
	ret->hdcimage = CreateCompatibleDC(hdc);
	ReleaseDC(NULL, hdc);
	ret->hBitmap = (HBITMAP)SelectObject(ret->hdcimage, ret->hBitmap);
	SetDIBColorTable(ret->hdcimage, 0, 2, bi->bmiColors);

	weight = (type & FDAT_BOLD)?FW_BOLD:FW_REGULAR;
	pitch = (type & FDAT_PROPORTIONAL)?VARIABLE_PITCH:FIXED_PITCH;
	if (fontface == NULL) {
		fontface = (type & FDAT_PROPORTIONAL)?deffontface2:deffontface;
	}
	ret->hfont = CreateFont(size, 0,
						FW_DONTCARE, FW_DONTCARE, weight,
						FALSE, FALSE, FALSE, SHIFTJIS_CHARSET,
						OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
						NONANTIALIASED_QUALITY, pitch, fontface);
	ret->hfont = (HFONT)SelectObject(ret->hdcimage, ret->hfont);
	SetTextColor(ret->hdcimage, RGB(255, 255, 255));
	SetBkColor(ret->hdcimage, RGB(0, 0, 0));
	SetRect(&ret->rect, 0, 0, ret->bmpwidth, ret->bmpheight);
	return(ret);
}


void fontmng_destroy(void *hdl) {

	FNTMNG	fhdl;

	fhdl = (FNTMNG)hdl;
	if (fhdl) {
		DeleteObject(SelectObject(fhdl->hdcimage, fhdl->hBitmap));
		DeleteObject(SelectObject(fhdl->hdcimage, fhdl->hfont));
		DeleteDC(fhdl->hdcimage);
		_MFREE(hdl);
	}
}


#if !defined(UNICODE)
static void getlength1(FNTMNG fhdl, FNTDAT fdat,
											const char *string, int length) {

	SIZE	fntsize;

	if ((fhdl->fonttype & FDAT_PROPORTIONAL) &&
		(GetTextExtentPoint32(fhdl->hdcimage, string, length, &fntsize))) {
		fntsize.cx = min(fntsize.cx, fhdl->bmpwidth);
		fdat->width = fntsize.cx;
		fdat->pitch = fntsize.cx;
	}
	else if (length < 2) {
		fdat->width = fhdl->fontwidth;
		fdat->pitch = (fhdl->fontsize + 1) >> 1;
	}
	else {
		fdat->width = fhdl->fontwidth;
		fdat->pitch = fhdl->fontsize;
	}
	fdat->height = fhdl->fontheight;
}

static void fontmng_getchar(FNTMNG fhdl, FNTDAT fdat, const char *string) {

	int		leng;

	FillRect(fhdl->hdcimage, &fhdl->rect,
										(HBRUSH)GetStockObject(BLACK_BRUSH));
	leng = strlen(string);
	TextOut(fhdl->hdcimage, 0, 0, string, leng);
	getlength1(fhdl, fdat, string, leng);
}
#else
static void getlength1(FNTMNG fhdl, FNTDAT fdat,
											const char *string, int length) {

	TCHAR	work[4];
	int		leng;
	SIZE	fntsize;

	if (fhdl->fonttype & FDAT_PROPORTIONAL) {
		leng = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, string, -1,
												work, NELEMENTS(work)) - 1;
		GetTextExtentPoint32(fhdl->hdcimage, work, leng, &fntsize);
		fntsize.cx = min(fntsize.cx, fhdl->bmpwidth);
		fdat->width = fntsize.cx;
		fdat->pitch = fntsize.cx;
	}
	else if (length < 2) {
		fdat->width = fhdl->fontwidth;
		fdat->pitch = (fhdl->fontsize + 1) >> 1;
	}
	else {
		fdat->width = fhdl->fontwidth;
		fdat->pitch = fhdl->fontsize;
	}
	fdat->height = fhdl->fontheight;
}

static void fontmng_getchar(FNTMNG fhdl, FNTDAT fdat, const char *string) {

	TCHAR	work[4];
	int		leng;

	FillRect(fhdl->hdcimage, &fhdl->rect,
										(HBRUSH)GetStockObject(BLACK_BRUSH));
	leng = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, string, -1,
												work, NELEMENTS(work)) - 1;
	TextOut(fhdl->hdcimage, 0, 0, work, leng);
	getlength1(fhdl, fdat, string, leng);
}
#endif


BOOL fontmng_getsize(void *hdl, const char *string, POINT_T *pt) {

	char	buf[4];
	_FNTDAT	fdat;
	int		width;
	int		leng;

	width = 0;
	if ((hdl == NULL) || (string == NULL)) {
		goto fmgs_exit;
	}

	buf[2] = '\0';
	do {
		buf[0] = *string++;
		if ((((buf[0] ^ 0x20) - 0xa1) & 0xff) < 0x3c) {
			buf[1] = *string++;
			if (buf[1] == '\0') {
				break;
			}
			leng = 2;
		}
		else if (buf[0]) {
			buf[1] = '\0';
			leng = 1;
		}
		else {
			break;
		}
		getlength1((FNTMNG)hdl, &fdat, buf, leng);
		width += fdat.pitch;
	} while(1);

	if (pt) {
		pt->x = width;
		pt->y = ((FNTMNG)hdl)->fontsize;
	}
	return(SUCCESS);

fmgs_exit:
	return(FAILURE);
}

BOOL fontmng_getdrawsize(void *hdl, const char *string, POINT_T *pt) {

	char	buf[4];
	_FNTDAT	fdat;
	int		width;
	int		posx;
	int		leng;

	if ((hdl == NULL) || (string == NULL)) {
		goto fmgds_exit;
	}

	width = 0;
	posx = 0;
	buf[2] = '\0';
	do {
		buf[0] = *string++;
		if ((((buf[0] ^ 0x20) - 0xa1) & 0xff) < 0x3c) {
			buf[1] = *string++;
			if (buf[1] == '\0') {
				break;
			}
			leng = 2;
		}
		else if (buf[0]) {
			buf[1] = '\0';
			leng = 1;
		}
		else {
			break;
		}
		getlength1((FNTMNG)hdl, &fdat, buf, leng);
		width = posx + max(fdat.width, fdat.pitch);
		posx += fdat.pitch;
	} while(1);

	if (pt) {
		pt->x = width;
		pt->y = ((FNTMNG)hdl)->fontsize;
	}
	return(SUCCESS);

fmgds_exit:
	return(FAILURE);
}

static void fontmng_setpat(FNTMNG fhdl, FNTDAT fdat) {

	DWORD	remx;
	DWORD	remy;
	BYTE	*src;
	BYTE	*dst;
	BYTE	*s;
	BYTE	bit;
	BYTE	b1 = 0;				// for cygwin
	int		align;

	align = fhdl->bmpalign;
	src = fhdl->image + (fhdl->rect.bottom * align);
	if (fdat->width <= 0) {
		goto fmsp_end;
	}

	dst = (BYTE *)(fdat + 1);
	align *= -1;

	remy = fdat->height;
	do {
		src += align;
		s = src;
		remx = fdat->width;
		bit = 0;
		do {
			if (bit == 0) {
				bit = 0x80;
				b1 = *s++;
			}
			*dst++ = (b1 & bit)?FDAT_DEPTH:0x00;
			bit >>= 1;
		} while(--remx);
	} while(--remy);

fmsp_end:
	return;
}


// ----

FNTDAT fontmng_get(void *hdl, const char *string) {

	FNTMNG	fhdl;
	FNTDAT	fdat;

	if ((hdl == NULL) || (string == NULL)) {
		goto ftmggt_err;
	}
	fhdl = (FNTMNG)hdl;
	fdat = (FNTDAT)(fhdl + 1);
	fontmng_getchar(fhdl, fdat, string);
	fontmng_setpat(fhdl, fdat);
	return(fdat);

ftmggt_err:
	return(NULL);
}
