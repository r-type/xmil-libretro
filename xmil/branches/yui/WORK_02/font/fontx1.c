#include	"compiler.h"
#include	"parts.h"
#include	"dosio.h"
#include	"font.h"
#include	"fontdata.h"


static const OEMCHAR dot_tmp[] = OEMTEXT(".TMP");

static FILEH fontopen(const OEMCHAR *path, const OEMCHAR *fontname) {

	OEMCHAR	filename[MAX_PATH];
	FILEH	ret;

	file_cpyname(filename, path, NELEMENTS(filename));
	file_cutname(filename);
	file_catname(filename, fontname, NELEMENTS(filename));
	ret = file_open_rb(filename);
	if (ret == FILEH_INVALID) {
		file_cutext(filename);
		file_catname(filename, dot_tmp, NELEMENTS(filename));
		ret = file_open_rb(filename);
	}
	return(ret);
}

static void x1knjcpy(const UINT8 *src, UINT from, UINT to) {

	UINT	i;
	UINT	h;
	UINT	l;
	UINT	sjis;
	UINT	j;
const UINT8	*p;
	UINT8	*q;

	for (i=from; i<to; i++) {
		h = i << 8;
		for (l=0x21; l<0x7f; l++) {
			/* 漢字のポインタを求める */
			sjis = jis2sjis(h + l);
			if ((sjis >= 0x8140) && (sjis < 0x84c0)) {
				p = src + 0x00000 + ((sjis - 0x8140) << 5);
			}
			else if ((sjis >= 0x8890) && (sjis < 0xa000)) {
				p = src + 0x07000 + ((sjis - 0x8890) << 5);
			}
			else if ((sjis >= 0xe040) && (sjis < 0xeab0)) {
				p = src + 0x35e00 + ((sjis - 0xe040) << 5);
			}
			else {
				p = NULL;
			}
			if (p) {							/* 規格内コードならば */
				q = font_adrsx1t(h + l);
				/* コピーする */
				if (q) {
					for (j=0; j<16; j++) {
						q[0] = p[0];
						q[FONTX1T_LR] = p[1];
						p += 2;
						q += 1;
					}
				}
			}
		}
	}
}

REG8 x1fontread(const OEMCHAR *path, REG8 loading) {

	UINT8	*work;
	FILEH	fh;

	work = (UINT8 *)_MALLOC(306176, filename);
	if (work == NULL) {
		return(loading);
	}
	if (loading & FONT_ANK8) {
		fh = fontopen(path, x1ank1name);
		if (fh != FILEH_INVALID) {
			if (file_read(fh, work, 0x800) == 0x800) {
				loading &= ~FONT_ANK8;
				CopyMemory(font_ank, work, 0x800);
			}
			file_close(fh);
		}
	}
	if (loading & FONTLOAD_ANK) {
		fh = fontopen(path, x1ank2name);
		if (fh != FILEH_INVALID) {
			if (file_read(fh, work, 4096) == 4096) {
				if (loading & FONT_ANK16a) {
					loading &= ~FONT_ANK16a;
					CopyMemory(font_txt, work, 0x800);
				}
				if (loading & FONT_ANK16b) {
					loading &= ~FONT_ANK16b;
					CopyMemory(font_txt + 0x800, work + 0x800, 0x800);
				}
			}
			file_close(fh);
		}
	}
	if (loading & (FONT_KNJ1 | FONT_KNJ2)) {
		fh = fontopen(path, x1knjname);
		if (fh != FILEH_INVALID) {
			if (file_read(fh, work, 306176) == 306176) {
				if (loading & FONT_KNJ1) {
					loading &= ~FONT_KNJ1;
					x1knjcpy(work, 0x21, 0x50);
				}
				if (loading & FONT_KNJ2) {
					loading &= ~FONT_KNJ2;
					x1knjcpy(work, 0x50, 0x78);
				}
			}
			file_close(fh);
		}
	}
	_MFREE(work);
	return(loading);
}

