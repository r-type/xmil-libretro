#include	"compiler.h"
#include	"dosio.h"
#include	"font.h"
#include	"fontdata.h"
#include	"fontmake.h"


static const UINT8 jis2x1t_table[64] = {
					0xff, 0xff, 0xff, 0xff,		/* 0x00 */
					0xff, 0xff, 0xff, 0xff,		/* 0x08 */
					0xff, 0xff, 0xff, 0xff,		/* 0x10 */
					0xff, 0xff, 0xff, 0xff,		/* 0x18 */
					0xff, 0x00, 0x02, 0x01,		/* 0x20 */
					0xff, 0xff, 0xff, 0xff,		/* 0x28 */
					0xff, 0x04, 0x06, 0x08,		/* 0x30 */
					0xff, 0x05, 0x07, 0x09,		/* 0x38 */

					0xff, 0x0a, 0x0c, 0x0e,		/* 0x40 */
					0xff, 0x0b, 0x0d, 0x0f,		/* 0x48 */
					0xff, 0x10, 0x12, 0x14,		/* 0x50 */
					0xff, 0x11, 0x13, 0x15,		/* 0x58 */
					0xff, 0x16, 0x18, 0x1a,		/* 0x60 */
					0xff, 0x17, 0x19, 0x1b,		/* 0x68 */
					0xff, 0x1c, 0x1e, 0x1d,		/* 0x70 */
					0xff, 0xff, 0xff, 0xff};	/* 0x78 */


	UINT8	font_ank[0x0800];
	UINT8	font_txt[0x1000];
	UINT8	font_knjx1[0x20000];
	UINT8	font_knjx1t[0x40000];


UINT8 *font_adrsx1t(UINT jis) {

	UINT	badr;
	UINT8	*ret;

	badr = jis2x1t_table[((jis >> 9) & 0x3c) + ((jis >> 5) & 3)];
	if (badr & 0xc0) {
		return(NULL);
	}
	ret = font_knjx1t;
	ret += (badr << 12);
	ret += ((jis & 0x700) << 1);
	ret += ((jis & 0x01f) << 4);
	return(ret);
}

static void font_mirrorx1t(void) {

	CopyMemory(font_knjx1t + 0x03000, font_knjx1t + 0x02000, 0x1000);
	CopyMemory(font_knjx1t + 0x23000, font_knjx1t + 0x22000, 0x1000);
	CopyMemory(font_knjx1t + 0x1f000, font_knjx1t + 0x1e000, 0x1000);
	CopyMemory(font_knjx1t + 0x3f000, font_knjx1t + 0x3e000, 0x1000);
}

static void font_cnvx1t2x1(void) {

	UINT	i;
	UINT	j;
	UINT8	*src;
	UINT8	*dst;

	dst = font_knjx1 + 0x0100;
	for (i=0x2100; i<0x2800; i+=0x100) {
		for (j=0x20; j<0x80; j+=0x20) {
			src = font_adrsx1t(i + j);
			if (src) {
				CopyMemory(dst, src, 0x200);
				CopyMemory(dst + FONTX1_LR, src + FONTX1T_LR, 0x200);
			}
			dst += 0x200;
		}
	}
	dst = font_knjx1 + 0x4000;
	for (i=0x3000; i<0x5000; i+=0x100) {
		for (j=0x20; j<0x80; j+=0x20) {
			src = font_adrsx1t(i + j);
			if (src) {
				CopyMemory(dst, src, 0x200);
				CopyMemory(dst + FONTX1_LR, src + FONTX1T_LR, 0x200);
			}
			dst += 0x200;
		}
	}
}


REG8 font_load(const OEMCHAR *filename, BRESULT force) {

	OEMCHAR	fname[MAX_PATH];
	REG8	type;
	REG8	loading;
	UINT	i;

	if (filename) {
		file_cpyname(fname, filename, NELEMENTS(fname));
	}
	else {
		fname[0] = '\0';
	}
	type = 0;
	if ((!type) && (!force)) {
		return(0);
	}

	CopyMemory(font_ank, defrom_ank, 0x800);
	for (i=0; i<0x800; i++) {
		font_txt[(i<<1)+0] = defrom_ank[i];
		font_txt[(i<<1)+1] = defrom_ank[i];
	}
	CopyMemory(font_txt + 0x000, defrom_txt, 0x200);
	CopyMemory(font_txt + 0x7f0, defrom_txt + 0x200, 0x210);
	CopyMemory(font_txt + 0xe00, defrom_txt + 0x410, 0x200);
	ZeroMemory(font_knjx1, sizeof(font_knjx1));
	ZeroMemory(font_knjx1t, sizeof(font_knjx1t));

	loading = 0xff;
	loading = x1fontread(file_getcd(x1ank1name), loading);
	if (loading & FONTLOAD_16) {
		makex1font(loading);
		loading = x1fontread(file_getcd(x1ank1name), loading);
	}
	font_mirrorx1t();
	font_cnvx1t2x1();
	return(type);
}

