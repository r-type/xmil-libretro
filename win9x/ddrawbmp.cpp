#include	"compiler.h"
#include	<windowsx.h>
#include	"xmil.h"
#include	"dosio.h"
#include	"scrnmng.h"
#include	"scrndraw.h"
#include	"menu.h"
#include	"ini.h"
#include	"palettes.h"
#include	"dialogs.h"


typedef struct {
	BITMAPFILEHEADER	bmf;
	BITMAPINFOHEADER	bmi;
	RGB32				pal[256];
//	BYTE				data[0];
} BMP_CTRL;

#define	PALADDED		32

static const OEMCHAR bmpext[] = OEMTEXT("bmp");
static const OEMCHAR bmptitle[] = OEMTEXT("Bitmapの保存");
static const OEMCHAR *bmpextstr[] = {
						OEMTEXT("2色ビットマップ\0*.bmp\0"),
						OEMTEXT("16色ビットマップ\0*.bmp\0"),
						OEMTEXT("256色ビットマップ\0*.bmp\0"),
						OEMTEXT("フルカラービットマップ\0*.bmp\0")};

static	OEMCHAR	pathname[MAX_PATH];

static OEMCHAR *savefileselect(const OEMCHAR *filter, const OEMCHAR *defname) {

	FILESEL		bmpui;

	bmpui.title = bmptitle;
	bmpui.ext = bmpext;
	bmpui.filter = filter;
	bmpui.defindex = 1;
	if (dlgs_selectwritenum(hWndMain, &bmpui, pathname, NELEMENTS(pathname))) {
		file_cpyname(bmpfilefolder, pathname, NELEMENTS(bmpfilefolder));
//		sysmng_update(SYS_UPDATEOSCFG);
		return(pathname);
	}
	return(NULL);
}

static RGB32 palettesmix(BYTE *p) {

	WORD	c;
	BYTE	c1, c2;
	RGB32	ret;

	c1 = *p++;
	c2 = *p++;
	ret.d = x1n_pal32[c1].d;
	if (x1n_pal32[c1].p.r != x1n_pal32[c2].p.r) {
		if ((c = ((x1n_pal32[c1].p.r + x1n_pal32[c2].p.r) / 2)
														+ PALADDED) >= 256) {
			c = 255;
		}
		ret.p.r = (BYTE)c;
	}
	if (x1n_pal32[c1].p.b != x1n_pal32[c2].p.b) {
		if ((c = ((x1n_pal32[c1].p.b + x1n_pal32[c2].p.b) / 2)
														+ PALADDED) >= 256) {
			c = 255;
		}
		ret.p.b = (BYTE)c;
	}
	if (x1n_pal32[c1].p.g != x1n_pal32[c2].p.g) {
		if ((c = ((x1n_pal32[c1].p.g + x1n_pal32[c2].p.g) / 2)
														+ PALADDED) >= 256) {
			c = 255;
		}
		ret.p.g = (BYTE)c;
	}
	return(ret);
}


BYTE bmps_appal(int *pals, RGB32 *pal, DWORD col) {

	int		ret;

	col &= 0x00ffffff;
	for (ret=0; ret<(*pals); ret++) {
		if (pal[ret].d == col) {
			return((BYTE)ret);
		}
	}
	(*pals)++;
	if (*pals > 256) {
		return(0);
	}
	pal[ret].d = col;
	return((BYTE)ret);
}

void bmpsavefile(const OEMCHAR *filename, BMP_CTRL *bmc) {

	FILEH	dst;
	DWORD	linesize;
	long	x;
	long	y;
	BYTE	*p;
	BYTE	bit;
	BYTE	work[SURFACE_WIDTH];

	if (bmc->bmi.biClrImportant <= 2) {
		bmc->bmi.biBitCount = 1;
		bmc->bmi.biClrUsed = 2L;
	}
	else if (bmc->bmi.biClrImportant <= 16) {
		bmc->bmi.biBitCount = 4;
		bmc->bmi.biClrUsed = 16L;
	}
	else if (bmc->bmi.biClrImportant <= 256) {
		bmc->bmi.biBitCount = 8;
		bmc->bmi.biClrUsed = 256L;
	}
	else {
		bmc->bmi.biBitCount = 24;
		bmc->bmi.biClrUsed = 0;
		bmc->bmi.biClrImportant = 0;
	}
	linesize = (((bmc->bmi.biWidth * bmc->bmi.biBitCount + 31) >> 5) << 2);
	bmc->bmf.bfType = 0x4d42;
	bmc->bmi.biSize = sizeof(BITMAPINFOHEADER);
	bmc->bmi.biPlanes = 1;
	bmc->bmi.biSizeImage = bmc->bmi.biHeight * linesize;
	bmc->bmf.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER)
												+ (bmc->bmi.biClrUsed * 4);
	bmc->bmf.bfSize = bmc->bmf.bfOffBits + bmc->bmi.biSizeImage;


	if ((dst = file_create(filename)) != FILEH_INVALID) {
		file_write(dst, bmc, bmc->bmf.bfOffBits);
		y = bmc->bmi.biHeight;
		while(y--) {
			p = (BYTE *)bmc + sizeof(BMP_CTRL) + (y * bmc->bmi.biWidth);
			if (bmc->bmi.biBitCount == 1) {
				bit = 0x80;
				ZeroMemory(work, sizeof(work));
				for (x=0; x<bmc->bmi.biWidth; x++) {
					if (*p++) {
						work[x/8] |= bit;
					}
					__asm {
						ror		bit, 1
					}
				}
				file_write(dst, work, linesize);
			}
			else if (bmc->bmi.biBitCount == 4) {
				bit = 0;
				ZeroMemory(work, sizeof(work));
				for (x=0; x<bmc->bmi.biWidth; x++) {
					if (bit) {
						work[x/2] |= *p++;
					}
					else {
						work[x/2] = (BYTE)((*p++) << 4);
					}
					bit ^= 1;
				}
				file_write(dst, work, linesize);
			}
			else if (bmc->bmi.biBitCount == 8) {
				file_write(dst, p, linesize);
			}
			else if (bmc->bmi.biBitCount == 24) {
				p = (BYTE *)bmc + sizeof(BMP_CTRL)
											+ (3L * y * bmc->bmi.biWidth);
				file_write(dst, p, linesize);
			}
		}
		file_close(dst);
	}
}

void bmpsave(void) {

	HANDLE		hbmc;
	BMP_CTRL	*bmc;
	BYTE		*p, *q;
	BYTE		remap[256];
	BYTE		remapflg[256];
	BYTE		c;
	int			pals = 0;
	long		x, y;
const OEMCHAR	*f;
	OEMCHAR		*r;

	if ((hbmc = GlobalAlloc(GPTR, (DWORD)sizeof(BMP_CTRL) + 1 +
					((DWORD)SURFACE_WIDTH * SURFACE_HEIGHT * 3L))) == NULL) {
		return;
	}
	if ((bmc = (BMP_CTRL *)GlobalLock(hbmc)) == NULL) {
		GlobalFree(hbmc);
		return;
	}
	ZeroMemory(bmc, sizeof(BMP_CTRL));
	ZeroMemory(remap, 256);
	ZeroMemory(remapflg, 256);
	bmc->bmi.biWidth = SURFACE_WIDTH;
	bmc->bmi.biHeight = SURFACE_HEIGHT;
	p = screenmap;
	q = (BYTE *)bmc + sizeof(BMP_CTRL);
	for (y=0; y<SURFACE_HEIGHT; y++) {
		switch(scrn.widthmode) {
			case SCRNWIDTHMODE_WIDTH80:
			default:
				for (x=0; x<SURFACE_WIDTH; x++) {
					c = *p++;
					if (!remapflg[c]) {
						remapflg[c] = 1;
						remap[c] = bmps_appal(&pals, bmc->pal,
															x1n_pal32[c].d);
					}
					*q++ = remap[c];
				}
				break;

			case SCRNWIDTHMODE_WIDTH40:
				for (x=0; x<SURFACE_WIDTH/2; x++) {
					c = *p++;
					if (!remapflg[c]) {
						remapflg[c] = 1;
						remap[c] = bmps_appal(&pals, bmc->pal,
															x1n_pal32[c].d);
					}
					*q++ = remap[c];
					*q++ = remap[c];
				}
				p += SURFACE_WIDTH/2;
				break;

			case SCRNWIDTHMODE_4096:
				for (x=0; x<SURFACE_WIDTH/2; x++) {
					c = bmps_appal(&pals, bmc->pal, 
										x1z_pal32[(p[320] << 8) | p[0]].d);
					p++;
					*q++ = c;
					*q++ = c;
				}
				p += SURFACE_WIDTH/2;
				break;
		}
		if (pals > 256) {
			break;
		}
	}
	bmc->bmi.biClrImportant = (DWORD)pals;
	f = bmpextstr[0];
	if (pals > 256) {
		f = bmpextstr[3];
		p = screenmap;
		q = (BYTE *)bmc + sizeof(BMP_CTRL);
		for (y=0; y<SURFACE_HEIGHT; y++) {
			switch(scrn.widthmode) {
				case 0:
				default:
					for (x=0; x<SURFACE_WIDTH; x++) {
						*(DWORD *)q = x1n_pal32[*p++].d;
						q += 3;
					}
					break;

				case 1:
					for (x=0; x<SURFACE_WIDTH/2; x++) {
						*(DWORD *)q = x1n_pal32[*p].d;
						*(DWORD *)(q+3) = x1n_pal32[*p].d;
						p++;
						q += 6;
					}
					p += SURFACE_WIDTH/2;
					break;

				case 2:
					for (x=0; x<SURFACE_WIDTH/2; x++) {
						*(DWORD *)q = x1z_pal32[(p[320] << 8) | p[0]].d;
						*(DWORD *)(q+3) = x1z_pal32[(p[320] << 8) | p[0]].d;
						p++;
						q += 6;
					}
					p += SURFACE_WIDTH/2;
					break;
			}
		}
	}
	else if (pals > 16) {
		f = bmpextstr[2];
	}
	else if (pals > 2) {
		f = bmpextstr[1];
	}
	if ((r = savefileselect(f, OEMTEXT("X1R_%04d.BMP"))) != NULL) {
		bmpsavefile(r, bmc);
	}
	GlobalUnlock(hbmc);
	GlobalFree(hbmc);
}


void bmpsavehalf(void) {

	HANDLE		hbmc;
	BMP_CTRL	*bmc;
	BYTE		*p, *q;
	int			pals = 0;
	long		x, y;
const OEMCHAR	*f;
	OEMCHAR		*r;

	if ((hbmc = GlobalAlloc(GPTR, (DWORD)sizeof(BMP_CTRL) + 1 +
					((DWORD)SURFACE_WIDTH * SURFACE_HEIGHT * 3L))) == NULL) {
		return;
	}
	if ((bmc = (BMP_CTRL *)GlobalLock(hbmc)) == NULL) {
		GlobalFree(hbmc);
		return;
	}
	ZeroMemory(bmc, sizeof(BMP_CTRL));
	bmc->bmi.biWidth = SURFACE_WIDTH/2;
	bmc->bmi.biHeight = SURFACE_HEIGHT/2;
	p = screenmap;
	q = (BYTE *)bmc + sizeof(BMP_CTRL);
	for (y=0; y<SURFACE_HEIGHT/2; y++) {
		switch(scrn.widthmode) {
			case SCRNWIDTHMODE_WIDTH80:
			default:
				for (x=0; x<SURFACE_WIDTH/2; x++) {
					*q++ = bmps_appal(&pals, bmc->pal, palettesmix(p).d);
					p += 2;
				}
				p += SURFACE_WIDTH;
				break;

			case SCRNWIDTHMODE_WIDTH40:
				for (x=0; x<SURFACE_WIDTH/2; x++) {
					*q++ = bmps_appal(&pals, bmc->pal, x1n_pal32[*p++].d);
				}
				p += SURFACE_WIDTH + (SURFACE_WIDTH/2);
				break;

			case SCRNWIDTHMODE_4096:
				for (x=0; x<SURFACE_WIDTH/2; x++) {
					*q++ = bmps_appal(&pals, bmc->pal, 
										x1z_pal32[(p[320] << 8) | p[0]].d);
					p++;
				}
				p += SURFACE_WIDTH + (SURFACE_WIDTH/2);
				break;
		}
		if (pals > 256) {
			break;
		}
	}
	bmc->bmi.biClrImportant = (DWORD)pals;
	f = bmpextstr[0];
	if (pals > 256) {
		f = bmpextstr[3];
		p = screenmap;
		q = (BYTE *)bmc + sizeof(BMP_CTRL);
		for (y=0; y<SURFACE_HEIGHT/2; y++) {
			switch(scrn.widthmode) {
				case SCRNWIDTHMODE_WIDTH80:
				default:
					for (x=0; x<SURFACE_WIDTH/2; x++) {
						*(DWORD *)q = palettesmix(p).d;
						p += 2;
						q += 3;
					}
					p += SURFACE_WIDTH;
					break;

				case SCRNWIDTHMODE_WIDTH40:
					for (x=0; x<SURFACE_WIDTH/2; x++) {
						*(DWORD *)q = x1n_pal32[*p++].d;
						q += 3;
					}
					p += SURFACE_WIDTH + (SURFACE_WIDTH/2);
					break;

				case SCRNWIDTHMODE_4096:
					for (x=0; x<SURFACE_WIDTH/2; x++) {
						*(DWORD *)q = x1z_pal32[(p[320] << 8) | p[0]].d;
						p++;
						q += 3;
					}
					p += SURFACE_WIDTH + (SURFACE_WIDTH/2);
					break;
			}
		}
	}
	else if (pals > 16) {
		f = bmpextstr[2];
	}
	else if (pals > 2) {
		f = bmpextstr[1];
	}
	if ((r = savefileselect(f, OEMTEXT("X1RH%04d.BMP"))) != NULL) {
		bmpsavefile(r, bmc);
	}
	GlobalUnlock(hbmc);
	GlobalFree(hbmc);
}

