#include	"compiler.h"
#include	"scrnmng.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"scrndraw.h"
#include	"palettes.h"
#include	"makescrn.h"


	RGB32	x1n_pal32[256];			// xm_palette
	RGB32	x1z_pal32[4096+8];		// GRPHPAL4096

#if defined(SUPPORT_16BPP)
	RGB16	x1n_pal16[256];			// xmil_pal16
	RGB16	x1z_pal16[4096+8];		// GRPHPAL16bit
#endif


// drop
	int		xm_palettes = 0;


extern	BYTE	dispmode;
extern	BYTE	pal_disp;


static const UINT16 pal4096banktbl[2][64] = {
			{	0x000, 0x008, 0x080, 0x088, 0x800, 0x808, 0x880, 0x888,
				0x004, 0x00C, 0x084, 0x08C, 0x804, 0x80C, 0x884, 0x88C,
				0x040, 0x048, 0x0C0, 0x0C8, 0x840, 0x848, 0x8C0, 0x8C8,
				0x044, 0x04C, 0x0C4, 0x0CC, 0x844, 0x84C, 0x8C4, 0x8CC,
				0x400, 0x408, 0x480, 0x488, 0xC00, 0xC08, 0xC80, 0xC88,
				0x404, 0x40C, 0x484, 0x48C, 0xC04, 0xC0C, 0xC84, 0xC8C,
				0x440, 0x448, 0x4C0, 0x4C8, 0xC40, 0xC48, 0xCC0, 0xCC8,
				0x444, 0x44C, 0x4C4, 0x4CC, 0xC44, 0xC4C, 0xCC4, 0xCCC},

			{	0x000, 0x002, 0x020, 0x022, 0x200, 0x202, 0x220, 0x222,
				0x001, 0x003, 0x021, 0x023, 0x201, 0x203, 0x221, 0x223,
				0x010, 0x012, 0x030, 0x032, 0x210, 0x212, 0x230, 0x232,
				0x011, 0x013, 0x031, 0x033, 0x211, 0x213, 0x231, 0x233,
				0x100, 0x102, 0x120, 0x122, 0x300, 0x302, 0x320, 0x322,
				0x101, 0x103, 0x121, 0x123, 0x301, 0x303, 0x321, 0x323,
				0x110, 0x112, 0x130, 0x132, 0x310, 0x312, 0x330, 0x332,
				0x111, 0x113, 0x131, 0x133, 0x311, 0x313, 0x331, 0x333}};




void pal_makegrad(RGB32 *pal, int pals, UINT32 bg, UINT32 fg) {

	int		i;

	if (pals >= 2) {
		pals--;
		for (i=0; i<=pals; i++) {
			pal[i].p.b = (UINT8)
				((((fg >> 0) & 0x0000ff) * i + 
				((bg >> 0) & 0x0000ff) * (pals-i)) / pals);
			pal[i].p.g = (UINT8)
				((((fg >> 8) & 0x0000ff) * i + 
				((bg >> 8) & 0x0000ff) * (pals-i)) / pals);
			pal[i].p.r = (UINT8)
				((((fg >> 16) & 0x0000ff) * i + 
				((bg >> 16) & 0x0000ff) * (pals-i)) / pals);
			pal[i].p.e = 0;
		}
	}
}


// ----

static	WORD	halfgrp = 0;
static	WORD	halftxt = 0;


typedef struct {
	UINT8	blankcol;
	UINT8	padding[3];
	RGB32	text[24];
	RGB32	grph[2][16];
	RGB32	grph64[2][64];
} PALS;

static	PALS	pals;


void pal_settext(REG8 num) {

	REG8	textpal;
	REG8	black;
	RGB32	rgb;

	textpal = crtc_TEXTPAL[num];
	black = 0;
	rgb.p.e = 0;
	rgb.p.b = ((textpal >> 0) & 3) * 0x55;
	if (rgb.p.b < pals.blankcol) {
		black |= 1;
	}
	rgb.p.g = ((textpal >> 4) & 3) * 0x55;
	if (rgb.p.g < pals.blankcol) {
		black |= 2;
	}
	rgb.p.r = ((textpal >> 2) & 3) * 0x55;
	if (rgb.p.r < pals.blankcol) {
		black |= 4;
	}
	if (black == 7) {
		rgb.p.b = pals.blankcol;
		rgb.p.g = pals.blankcol;
		rgb.p.r = pals.blankcol;
	}
	x1z_pal32[4096 + num].d = rgb.d;
#if defined(SUPPORT_16BPP)
	if (scrnmng_getbpp() == 16) {
		x1z_pal16[4096 + num] = scrnmng_makepal16(rgb);
	}
#endif

	pals.text[num].d = rgb.d;
	pals.text[num].p.e = num + START_EXT;

	pals.text[num+8].p.b = (halftxt * rgb.p.b) / 255;
	pals.text[num+8].p.g = (halftxt * rgb.p.g) / 255;
	pals.text[num+8].p.r = (halftxt * rgb.p.r) / 255;
	pals.text[num+8].p.e = num + START_EXT + 0x08;

	pals.text[num+16].p.b = (halfgrp * rgb.p.b) / 255;
	pals.text[num+16].p.g = (halfgrp * rgb.p.g) / 255;
	pals.text[num+16].p.r = (halfgrp * rgb.p.r) / 255;
	pals.text[num+16].p.e = num + START_EXT + 0x10;

	makescrn.palandply = 1;
}

void pal_setgrph(REG8 bank, REG8 num) {

	UINT	grphpal;
	REG8	black;
	RGB32	rgb;

	grphpal = crtc_GRPHPAL[bank][num];
	black = 0;
	rgb.p.b = ((grphpal >> 0) & 0x0f) * 0x11;
	if (rgb.p.b < pals.blankcol) {
		black |= 1;
	}
	rgb.p.g = ((grphpal >> 8) & 0x0f) * 0x11;
	if (rgb.p.g < pals.blankcol) {
		black |= 2;
	}
	rgb.p.r = ((grphpal >> 4) & 0x0f) * 0x11;
	if (rgb.p.r < pals.blankcol) {
		black |= 4;
	}
	if (black == 7) {
		rgb.p.b = pals.blankcol;
		rgb.p.g = pals.blankcol;
		rgb.p.r = pals.blankcol;
	}
	pals.grph64[bank][num].d = rgb.d;
	pals.grph64[bank][num].p.e = num + START_PAL;
	if (!(((num >> 3) ^ num) & 7)) {
		num &= 7;
		pals.grph[bank][num+0].d = rgb.d;
		pals.grph[bank][num+0].p.e = num + START_EXT + 0x18;
		pals.grph[bank][num+8].p.b = (halfgrp * rgb.p.b) / 0xff;
		pals.grph[bank][num+8].p.g = (halfgrp * rgb.p.g) / 0xff;
		pals.grph[bank][num+8].p.r = (halfgrp * rgb.p.r) / 0xff;
		pals.grph[bank][num+8].p.e = num + START_EXT + 0x20;
	}
	makescrn.palandply = 1;
}

void pal_setgrph4096(UINT num) {

	UINT	grphpal;
	REG8	black;
	RGB32	rgb;

	grphpal = crtc_PAL4096[num];
	black = 0;
	rgb.p.e = 0;
	rgb.p.b = ((grphpal >> 0) & 0x0f) * 0x11;
	if (rgb.p.b < pals.blankcol) {
		black |= 1;
	}
	rgb.p.g = ((grphpal >> 8) & 0x0f) * 0x11;
	if (rgb.p.g < pals.blankcol) {
		black |= 2;
	}
	rgb.p.r = ((grphpal >> 4) & 0x0f) * 0x11;
	if (rgb.p.r < pals.blankcol) {
		black |= 4;
	}
	if (black == 7) {
		rgb.p.b = pals.blankcol;
		rgb.p.g = pals.blankcol;
		rgb.p.r = pals.blankcol;
	}
	x1z_pal32[num].d = rgb.d;
#if defined(SUPPORT_16BPP)
	if (scrnmng_getbpp() == 16) {
		x1z_pal16[num] = scrnmng_makepal16(rgb);
	}
#endif
	makescrn.palandply = 1;
}


// ----

static void pal4096to64(RGB32 *pal, const UINT16 *map) {

	UINT	r;

	r = 64;
	do {
		pal->d = x1z_pal32[*map++].d;
		pal++;
	} while(--r);
}

void pal_update(void) {

	int		i, j;
	BYTE	bit;
	BYTE	c;
	BYTE	skip8 = 0;
	BYTE	skip16 = 0;

	if (pal_disp & PAL_4096) {
		switch(pal_disp & 0xf) {
			case PAL_4096H:
				pal4096to64(x1n_pal32, pal4096banktbl[0]);
				xm_palettes = 64;
				break;

			case PAL_4096L:
				pal4096to64(x1n_pal32, pal4096banktbl[1]);
				xm_palettes = 64;
				break;

			case (PAL_4096H | PAL_64x2):
				pal4096to64(&x1n_pal32[ 0], pal4096banktbl[0]);
				pal4096to64(&x1n_pal32[64], pal4096banktbl[1]);
				xm_palettes = 128;
				break;

			case (PAL_4096L | PAL_64x2):
				pal4096to64(&x1n_pal32[ 0], pal4096banktbl[0]);
				pal4096to64(&x1n_pal32[64], pal4096banktbl[1]);
				xm_palettes = 128;
				break;

			default:						// fullcolor!
				return;
		}
		for (i=0; i<8; i++) {
			x1n_pal32[xm_palettes++].d = pals.text[i].d;
		}
	}
	else if ((dispmode & SCRN64_MASK) == SCRN64_INVALID) {
		if ((xmilcfg.SKIP_LINE) && (!(crtc.s.SCRN_BITS & SCRN_24KHZ))) {
			skip8 = 8;
			skip16 = 16;
		}
		for (i=0, bit=1; i<8; i++, bit<<=1) {
			if (!(crtc.s.EXTPALMODE & 0x80)) {
				c = 0;
				if (crtc.s.PAL_B & bit) {
					c |= 1;
				}
				if (crtc.s.PAL_R & bit) {
					c |= 2;
				}
				if (crtc.s.PAL_G & bit) {
					c |= 4;
				}
			}
			else {
				c = i;
			}
			x1n_pal32[i].d = pals.grph[pal_disp][c].d;
			x1n_pal32[i+64].d = x1n_pal32[i+128].d
										= pals.grph[pal_disp][c+skip8].d;
			if (crtc.s.PLY & bit) {
				for (j=i+8; j<64; j+=8) {
					x1n_pal32[j].d = x1n_pal32[i].d;
					x1n_pal32[j+64].d = x1n_pal32[j+128].d
													= x1n_pal32[i+64].d;
				}
			}
			else {
				BYTE cnt = (crtc.s.BLACKPAL & 15) - 8;
				for (j=i+8; j<64; j+=8) {
					if (--cnt) {
						c = crtc.s.TEXT_PAL[j>>3];
					}
					else {
						c = 0;
					}
					x1n_pal32[j].d = pals.text[c].d;
					x1n_pal32[j+64].d = pals.text[c+skip8].d;
					x1n_pal32[j+128].d = pals.text[c+skip16].d;
				}
			}
		}
		for (i=0; i<24; i++) {
			x1n_pal32[i+192].d = pals.text[i].d;
		}
		for (i=0; i<16; i++) {
			x1n_pal32[i+192+24].d = pals.grph[pal_disp][i].d;
		}
		xm_palettes = 64+64+64+24+16;
	}
	else {
		for (i=0; i<64; i++) {
			x1n_pal32[i].d = pals.grph64[pal_disp][i].d;
		}
		for (i=0; i<8; i++) {
			x1n_pal32[i+64].d = pals.text[i].d;
		}
		xm_palettes = 64+8;
	}
#if defined(SUPPORT_16BPP)
	if (scrnmng_getbpp() == 16) {
		int i;
		for (i=0; i<xm_palettes; i++) {
			x1n_pal16[i] = scrnmng_makepal16(x1n_pal32[i]);
		}
	}
#endif
	scrndraw_changepalette();
}


// ----

void pal_reset(void) {

	REG8	i;
	REG8	j;
	UINT	k;

	pals.blankcol = xmilcfg.BLKLIGHT;
	if ((!xmilcfg.SKIP_LINE) || (crtc.s.SCRN_BITS & SCRN_24KHZ)) {
		pals.blankcol >>= 1;
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
		pal_settext(i);
	}
	for (i=0; i<2; i++) {
		for (j=0; j<64; j++) {
			pal_setgrph(i, j);
		}
	}
	for (k=0; k<4096; k++) {
		pal_setgrph4096(k);
	}
}

