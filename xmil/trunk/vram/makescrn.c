#include	"compiler.h"
#include	"scrnmng.h"
#include	"sysmng.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"vram.h"
#include	"scrndraw.h"
#include	"palettes.h"
#include	"makescrn.h"
#include	"makesub.h"


	MAKESCRN	makescrn;

	UINT8	scrnallflash;
	UINT	drawtime = 0;


static void fillupdatetmp(void) {

	UINT32	*p;
	UINT	i;

	p = (UINT32 *)updatetmp;
	for (i=0; i<0x200; i++) {
		p[i] |= (UPDATE_TVRAM << 24) | (UPDATE_TVRAM << 16) |
										(UPDATE_TVRAM << 8) | UPDATE_TVRAM;
	}
}

static void flashupdatetmp(void) {

	UINT	posl;
	UINT	y;
	UINT	x;
	UINT	r;
	UINT	posr;
	BRESULT	y2;
	REG16	atr;
	REG16	udtbase;
	REG16	udt;

	if (!makescrn.vramsize) {
		return;
	}
	posl = crtc.e.pos;
	y = crtc.e.yl;
	do {
		for (x=0; x<crtc.s.reg[CRTCREG_HDISP]; x++) {
			if (!(tram[TRAM_ATR + LOW11(posl + x)] & TRAMATR_Yx2)) {
				break;
			}
		}
		y2 = (x < crtc.s.reg[CRTCREG_HDISP])?FALSE:TRUE;
		udtbase = (x < crtc.s.reg[CRTCREG_HDISP])?0x0000:0x0404;
		r = (crtc.s.reg[CRTCREG_HDISP] + 1) >> 1;
		do {
			posr = LOW11(posl + 1);
			atr = (tram[TRAM_ATR + posl] << 8) | tram[TRAM_ATR + posr];
			udt = udtbase;
			if (!y2) {
				if (atr & (TRAMATR_Yx2 << 8)) {
					udt |= (UPDATE_TRAM | 1) << 8;		// ¶’×‚êc”{Šp
				}
				else {
					y2 = TRUE;
				}
			}
			if (!y2) {
				if (atr & (TRAMATR_Yx2 << 0)) {
					udt |= (UPDATE_TRAM | 1) << 0;		// ‰E’×‚êc”{Šp
				}
				else {
					y2 = TRUE;
				}
			}
			if (atr & (TRAMATR_Xx2 << 8)) {				// ¶‘¤”{Šp?
				udt |= 0x0812;
			}
			if (atr & (TRAMATR_Xx2 << 0)) {				// ‰E‘¤”{Šp?
				udt |= 0x0008;
			}
			if ((updatetmp[posl] ^ (udt >> 8)) & 0x1f) {
				updatetmp[posl] = (UINT8)((udt >> 8) | UPDATE_TVRAM);
			}
			if ((updatetmp[posr] ^ (udt >> 0)) & 0x1f) {
				updatetmp[posr] = (UINT8)((udt >> 0) | UPDATE_TVRAM);
			}
			posl = LOW11(posl + 2);
		} while(--r);
		if (crtc.s.reg[CRTCREG_HDISP] & 1) {
			posl = LOW11(posl - 1);
		}
	} while(--y);
}

static BRESULT updateblink(void) {

	UINT	pos;
	REG8	update;
	UINT	r;

	if (makescrn.blinktime) {
		makescrn.blinktime--;
		return(FALSE);
	}
	else {
		makescrn.blinktime = 30 - 1;
		pos = makescrn.vramtop;
		makescrn.blinktest ^= 0x10;
		update = 0;
		r = makescrn.vramsize;
		while(r) {
			r--;
			if (tram[TRAM_ATR + pos] & 0x10) {
				updatetmp[pos] |= UPDATE_TRAM;
				update = UPDATE_TRAM;
			}
			pos = LOW11(pos + 1);
		}
		if (update) {
			makescrn.existblink = 1;
			return(TRUE);
		}
		else {
			return(FALSE);
		}
	}
}


// ----

typedef void (*MAKEFN)(void);

static void width_dummy(void) { }

static const UINT8 screendraw[] = {
				MAKESCRN_320x200S,	MAKESCRN_320x400,
				MAKESCRN_320x200S,	MAKESCRN_320x400,
				MAKESCRN_320x200S,	MAKESCRN_320x400,
				MAKESCRN_320x200S,	MAKESCRN_320x200H,

				MAKESCRN_320x200S,	MAKESCRN_320x400,
				MAKESCRN_320x200S,	MAKESCRN_320x400,
				MAKESCRN_320x200S,	MAKESCRN_320x200H,
				MAKESCRN_320x200S,	MAKESCRN_320x200H,

#if defined(SUPPORT_TURBOZ)
				MAKESCRN_320x200H,	MAKESCRN_320x200H,
				MAKESCRN_320x200H,	MAKESCRN_320x200H,
				MAKESCRN_320x200H,	MAKESCRN_320x200H,
				MAKESCRN_320x200H,	MAKESCRN_320x200H,

				MAKESCRN_320x200H,	MAKESCRN_320x200H,
				MAKESCRN_320x200H,	MAKESCRN_320x200H,
				MAKESCRN_320x200H,	MAKESCRN_320x200H,
				MAKESCRN_320x200H,	MAKESCRN_320x200H,
#endif
};

static const MAKEFN screenmake[] = {
				width80x25_200l,	width80x25_400h,
				width80x25_200l,	width80x25_200h,
				width80x12_200l,	width80x12_400h,
				width80x12_200l,	width80x12_200h,

				width80x20l,		width80x20h,
				width80x20l,		width80x20h,
				width80x10l,		width80x10h,
				width80x10l,		width80x10h,

#if defined(SUPPORT_TURBOZ)
				width_dummy,		width_dummy,
				width_dummy,		width_dummy,
				width_dummy,		width_dummy,
				width_dummy,		width_dummy,

				width_dummy,		width_dummy,
				width_dummy,		width_dummy,
				width_dummy,		width_dummy,
				width_dummy,		width_dummy,
#endif
};


static void changemodes(void) {

	REG8	dispmode;

	dispmode = crtc.e.dispmode;
	makescrn.dispmode = dispmode;
	makescrn.drawmode = screendraw[dispmode & DISPMODE_MASKMODE];
	if (dispmode & DISPMODE_WIDTH80) {
		makescrn.drawmode |= 1;
	}
	sysmng_scrnwidth((dispmode & DISPMODE_WIDTH80) == 0);

	if (!(dispmode & DISPMODE_BANK1)) {
		makescrn.disp1 = gram + GRAM_BANK0;
		makescrn.disp2 = gram + GRAM_BANK1;
		makescrn.dispflag = UPDATE_TRAM + UPDATE_VRAM0;
	}
	else {
		makescrn.disp1 = gram + GRAM_BANK1;
		makescrn.disp2 = gram + GRAM_BANK0;
		makescrn.dispflag = UPDATE_TRAM + UPDATE_VRAM1;
	}
	scrnallflash = 1;
	makescrn.palandply = 1;
}

static void changecrtc(void) {

	UINT	scrnxmax;
	UINT	scrnymax;
	UINT	textxl;
	UINT	surfcx;
	UINT	fontcy;
	UINT	underlines;
	REG8	y2;
	UINT	charcy;
	UINT	surfcy;
	UINT	surfsy;
	UINT	x;
	UINT	y;
	UINT8	*p;

	makescrn.vramtop = crtc.e.pos;

	scrnxmax = (makescrn.dispmode & DISPMODE_WIDTH80)?80:40;
	scrnymax = 200;

	textxl = crtc.s.reg[CRTCREG_HDISP];
	surfcx = min(scrnxmax, textxl);

	fontcy = crtc.e.fonty;
	underlines = (makescrn.dispmode & DISPMODE_UNDERLINE)?2:0;
	if (fontcy > underlines) {
		fontcy -= underlines;
	}
	else {
		fontcy = 0;
	}
	y2 = (makescrn.dispmode & DISPMODE_TEXTYx2)?1:0;
	fontcy >>= y2;
#if 0
	if (((dispmode & SCRN64_MASK) != SCRN64_INVALID) && (fontcy > 8)) {
		fontcy = 8;
	}
#endif
	if (!fontcy) {
		fontcy = 1;
	}
	if (fontcy > 8) {
		fontcy = 8;
	}
	charcy = fontcy + underlines;
	makescrn.fontcy = fontcy;
	makescrn.charcy = charcy;
	charcy <<= y2;
	surfcy = scrnymax / charcy;
	if (surfcy > crtc.e.yl) {
		surfcy = crtc.e.yl;
	}

	surfsy = charcy * surfcy * 2;
	x = min(scrnxmax, makescrn.surfcx);
	if (surfcx < x) {								// ¬‚³‚­‚È‚Á‚½
		x = (x - surfcx) * 8;
		p = screenmap + (surfcx * 8);
		y = surfsy;
		while(y) {
			y--;
			ZeroMemory(p, x);
			p += SURFACE_WIDTH;
		}
	}
	if (surfsy < makescrn.surfsy) {
		ZeroMemory(screenmap + (SURFACE_WIDTH * surfsy),
								SURFACE_WIDTH * (makescrn.surfsy - surfsy));
	}
	makescrn.surfcx = surfcx;
	makescrn.surfrx = textxl - surfcx;
	makescrn.surfcy = surfcy;
	makescrn.surfsy = surfsy;
	makescrn.surfstep = (SURFACE_WIDTH * charcy * 2) - (surfcx * 8);
	makescrn.vramsize = min(0x800, surfcy * textxl);
//	scrnmng_setheight(0, charcy * surfcy * 2);
}

void scrnupdate(void) {

	BRESULT	ddrawflash;
	BRESULT	allflash;

	if (!corestat.drawframe) {
		return;
	}
	corestat.drawframe = 0;

	ddrawflash = makescrn.nextdraw;
	allflash = FALSE;
	if (makescrn.dispmode != crtc.e.dispmode) {
		changemodes();
	}
	if (scrnallflash) {
		scrnallflash = 0;
		fillupdatetmp();
		changecrtc();
		ddrawflash = TRUE;
		allflash = TRUE;
		makescrn.scrnflash = 1;
	}
	if (makescrn.remakeattr) {
		makescrn.remakeattr = 0;
		flashupdatetmp();
	}
	if (makescrn.palandply) {
		makescrn.palandply = 0;
		pal_update();
		ddrawflash = TRUE;
	}
	if (makescrn.existblink) {
		makescrn.scrnflash |= updateblink();
	}

	if (makescrn.scrnflash) {
		makescrn.scrnflash = 0;
		if (makescrn.vramsize) {
			makescrn.fontycnt = 0;
			screenmake[makescrn.dispmode & DISPMODE_MASKMODE]();
#if 0
			switch(lastdisp & SCRN64_MASK) {
				case SCRN64_320x200:
//					width40x25_64s();
					break;

				case SCRN64_L320x200x2:
//					width40x25_64x2();
					break;

				case SCRN64_L640x200:
//					width80x25_64s();
					break;

				case SCRN64_H320x400:
//					width40x25_64h();
					break;

				case SCRN64_320x200x4096:
//					width40x25_4096();
					break;

				case SCRN64_320x100:
//					width40x12_64l();
					break;

				case SCRN64_320x100x2:
//					width40x12_64x2();
					break;

				case SCRN64_L640x100:
//					width80x12_64s();
					break;

				case SCRN64_H320x200:
//					width40x12_64h();
					break;

				case SCRN64_320x100x4096:
//					width40x12_4096();
					break;

//				case SCRN64_INVALID:
				default:
					if (!(crtc.s.SCRN_BITS & SCRN_UNDERLINE)) {
						screenmake[(crtc.s.SCRN_BITS & 7) + 0]();
					}
					else {
						screenmake[(crtc.s.SCRN_BITS & 7) + 8]();
					}
					break;
			}
#endif
			ddrawflash = TRUE;
		}
	}

	if (ddrawflash) {
		makescrn.nextdraw = scrndraw_draw(allflash);
		drawtime++;
	}
}


void makescrn_initialize(void) {

	makesub_initialize();
}

void makescrn_reset(void) {

	changemodes();
	changecrtc();
	flashupdatetmp();
}

