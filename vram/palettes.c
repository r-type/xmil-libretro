#include	"compiler.h"
#include	"scrnmng.h"
#include	"pccore.h"
#include	"x1_io.h"
#include	"x1_crtc.h"
#include	"draw.h"
#include	"palettes.h"


	RGB32	x1n_pal32[256];			// xm_palette
	RGB32	x1z_pal32[4096+8];		// GRPHPAL4096

#if defined(SUPPORT_16BPP)
	RGB16	x1n_pal16[256];			// xmil_pal16
	RGB16	x1z_pal16[4096+8];		// GRPHPAL16bit
#endif


// drop
	int		xm_palettes = 0;





void pals_cnv16pal(RGB16 *dst, const RGB32 *src, UINT pals) {

	while(pals--) {
		*dst++ = scrnmng_makepal16(*src++);
	}
}


void pal_makegrad(RGB32 *pal, int pals, DWORD bg, DWORD fg) {

	int		i;

	if (pals >= 2) {
		pals--;
		for (i=0; i<=pals; i++) {
			pal[i].p.b = (BYTE)
				((((fg >> 0) & 0x0000ff) * i + 
				((bg >> 0) & 0x0000ff) * (pals-i)) / pals);
			pal[i].p.g = (BYTE)
				((((fg >> 8) & 0x0000ff) * i + 
				((bg >> 8) & 0x0000ff) * (pals-i)) / pals);
			pal[i].p.r = (BYTE)
				((((fg >> 16) & 0x0000ff) * i + 
				((bg >> 16) & 0x0000ff) * (pals-i)) / pals);
			pal[i].p.e = 0;
		}
	}
}

// ---------------------------------------------------------------------------

static	BYTE	blankcol = 0;
static	WORD	halfgrp = 0;
static	WORD	halftxt = 0;

		RGB32	TEXTPALS[24];
		RGB32	GRPHPALS[2][16];
		RGB32	GRPHPALS64[2][64];


void textpalette1(BYTE num) {

	REG8	f;
	RGB32	c;

	f = 0;
	c.p.e = 0;
	c.p.b = (crtc_TEXTPAL[num] & 0x3) * 0x55;
	if (c.p.b < blankcol) {
		f |= 1;
	}
	c.p.g = ((crtc_TEXTPAL[num] & 0x30) >> 4) * 0x55;
	if (c.p.g < blankcol) {
		f |= 2;
	}
	c.p.r = ((crtc_TEXTPAL[num] & 0x0c) >> 2) * 0x55;
	if (c.p.r < blankcol) {
		f |= 4;
	}
	if (f == 7) {
		c.d = 0x010101 * blankcol;
	}
	x1z_pal32[4096+num].d = c.d;
#if defined(SUPPORT_16BPP)
	if (scrnmng_getbpp() == 16) {
		x1z_pal16[4096+num] = scrnmng_makepal16(c);
	}
#endif

	TEXTPALS[num].d = c.d;
	TEXTPALS[num].p.e = num + START_EXT;

	TEXTPALS[num+8].p.b = (halftxt * c.p.b) / 0x00ff;
	TEXTPALS[num+8].p.g = (halftxt * c.p.g) / 0x00ff;
	TEXTPALS[num+8].p.r = (halftxt * c.p.r) / 0x00ff;
	TEXTPALS[num+8].p.e = num + START_EXT + 0x08;

	TEXTPALS[num+16].p.b = (halfgrp * c.p.b) / 0x00ff;
	TEXTPALS[num+16].p.g = (halfgrp * c.p.g) / 0x00ff;
	TEXTPALS[num+16].p.r = (halfgrp * c.p.r) / 0x00ff;
	TEXTPALS[num+16].p.e = num + START_EXT + 0x10;

	palandply = 1;
}


void grphpalette1(BYTE bank, BYTE num) {

	RGB32	c;
	BYTE	f = 0;

	c.p.b = (crtc_GRPHPAL[bank][num] & 0xf) * 0x11;
	if (c.p.b < blankcol) {
		f |= 1;
	}
	c.p.g = (crtc_GRPHPAL[bank][num] >> 8) * 0x11;
	if (c.p.g < blankcol) {
		f |= 2;
	}
	c.p.r = ((crtc_GRPHPAL[bank][num] & 0xf0) >> 4) * 0x11;
	if (c.p.r < blankcol) {
		f |= 4;
	}
	if (f == 7) {
		c.d = 0x010101 * blankcol;
	}

	GRPHPALS64[bank][num].d = c.d;
	GRPHPALS64[bank][num].p.e = num + START_PAL;
	if (!(((num >> 3) ^ num) & 7)) {
		num &= 7;
		GRPHPALS[bank][num+0].d = c.d;
		GRPHPALS[bank][num+0].p.e = num + START_EXT + 0x18;
		GRPHPALS[bank][num+8].p.b = (halfgrp * c.p.b) / 0xff;
		GRPHPALS[bank][num+8].p.g = (halfgrp * c.p.g) / 0xff;
		GRPHPALS[bank][num+8].p.r = (halfgrp * c.p.r) / 0xff;
		GRPHPALS[bank][num+8].p.e = num + START_EXT + 0x20;
	}
	palandply = 1;
}


void grphpal4096(WORD num) {

	RGB32	c;
	BYTE	f = 0;

	c.p.e = 0;
	c.p.b = (crtc_PAL4096[num] & 0xf) * 0x11;
	if (c.p.b < blankcol) {
		f |= 1;
	}
	c.p.g = (crtc_PAL4096[num] >> 8) * 0x11;
	if (c.p.g < blankcol) {
		f |= 2;
	}
	c.p.r = ((crtc_PAL4096[num] & 0xf0) >> 4) * 0x11;
	if (c.p.r < blankcol) {
		f |= 4;
	}
	if (f == 7) {
		c.d = 0x010101 * blankcol;
	}
	x1z_pal32[num].d = c.d;
#if defined(SUPPORT_16BPP)
	if (scrnmng_getbpp() == 16) {
		x1z_pal16[num] = scrnmng_makepal16(c);
	}
#endif
	palandply = 1;
}



void reflesh_palette(void) {

	BYTE	i, j;
	WORD	k;

	blankcol = xmilcfg.BLKLIGHT;
	if ((!xmilcfg.SKIP_LINE) || (crtc.SCRN_BITS & SCRN_24KHZ)) {
		blankcol >>= 1;
	}
	halfgrp = xmilcfg.LINEDEPTH;
	switch(xmilcfg.TEXT400L) {
		case 0:
			halftxt = xmilcfg.LINEDEPTH;
			break;
		case 1:
			halftxt = 0xff;
			break;
		default:
			halftxt = xmilcfg.LINETEXT;
			break;
	}

	for (i=0; i<8; i++) {
		textpalette1(i);
	}
	for (j=0; j<2; j++) {
		for (i=0; i<64; i++) {
			grphpalette1(j, i);
		}
	}
	for (k=0; k<4096; k++) {
		grphpal4096(k);
	}
}

