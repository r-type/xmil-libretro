#include	"compiler.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"vram.h"
#include	"palettes.h"
#include	"makescrn.h"
#include	"makesub.h"
#include	"vsyncff.h"


#define	SUPPORT_WIDTH80

	SCRNPOS		scrnpos;
	MAKESCRN	makescrn;


static void fillupdatetmp(void) {

	UINT	i;

	for (i=0; i<0x800; i++) {
		TRAMUPDATE(i) |= UPDATE_TVRAM;
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
			if (!(TRAM_ATR(LOW11(posl + x)) & TRAMATR_Yx2)) {
				break;
			}
		}
		y2 = (x < crtc.s.reg[CRTCREG_HDISP])?FALSE:TRUE;
		udtbase = (x < crtc.s.reg[CRTCREG_HDISP])?0x0000:0x0404;
		r = (crtc.s.reg[CRTCREG_HDISP] + 1) >> 1;
		do {
			posr = LOW11(posl + 1);
			atr = (TRAM_ATR(posl) << 8) | TRAM_ATR(posr);
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
			if ((TRAMUPDATE(posl) ^ (udt >> 8)) & 0x1f) {
				TRAMUPDATE(posl) = (UINT8)((udt >> 8) | UPDATE_TRAM);
			}
			if ((TRAMUPDATE(posr) ^ (udt >> 0)) & 0x1f) {
				TRAMUPDATE(posr) = (UINT8)((udt >> 0) | UPDATE_TRAM);
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

	pos = makescrn.vramtop;
	makescrn.blinktest ^= 0x10;
	update = 0;
	r = makescrn.vramsize;
	while(r) {
		r--;
		if (TRAM_ATR(pos) & 0x10) {
			TRAMUPDATE(pos) |= UPDATE_TRAM;
			update = UPDATE_TRAM;
		}
		pos = LOW11(pos + 1);
	}
	if (update) {
		return(TRUE);
	}
	else {
		return(FALSE);
	}
}


// ----

typedef void (*MAKEFN)(UINT pos);

static void width_dummy(UINT pos) { }

static const MAKEFN screenmake[] = {
				width40x25_200l,	width_dummy,
				width40x25_200l,	width40x25_200l,
				width40x12_200l,	width_dummy,
				width40x12_200l,	width40x12_200l,
				width_dummy,		width_dummy,
				width_dummy,		width_dummy,
				width_dummy,		width_dummy,
				width_dummy,		width_dummy,

				width80x25_200l,	width_dummy,
				width80x25_200l,	width80x25_200l,
				width80x12_200l,	width_dummy,
				width80x12_200l,	width80x12_200l,
				width_dummy,		width_dummy,
				width_dummy,		width_dummy,
				width_dummy,		width_dummy,
				width_dummy,		width_dummy,
};


static void changemodes(void) {

	REG8	dispmode;

	dispmode = crtc.e.dispmode;
	makescrn.dispmode = dispmode;
	makescrn.drawmode = dispmode & DISPMODE_MASKMODE;
	makemix_mixstep((BRESULT)(dispmode & 4));
#if defined(SUPPORT_WIDTH80)
	if (dispmode & DISPMODE_WIDTH80) {
		makescrn.drawmode += 16;
		vsyncff_turn(1);
	}
	else {
		vsyncff_turn(0);
	}
#endif
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
}

static void changecrtc(void) {

	UINT	scrnxmax;
	UINT	scrnymax;
	UINT	charcx;
	UINT	textxl;
	UINT	surfcx;
	UINT	fontcy;
	UINT	underlines;
	REG8	y2;
	UINT	charcy;
	UINT	surfcy;
	UINT	bl;

	makescrn.vramtop = crtc.e.pos;

#if defined(SUPPORT_WIDTH80)
	if (makescrn.dispmode & DISPMODE_WIDTH80) {
		scrnxmax = ADVV_WIDTH >> 2;
		charcx = 4;
	}
	else {
		scrnxmax = ADVV_WIDTH >> 3;
		charcx = 8;
	}
#else
	scrnxmax = ADVV_WIDTH >> 3;
	charcx = 8;
#endif
	scrnymax = ADVV_HEIGHT;

	textxl = crtc.s.reg[CRTCREG_HDISP];
	surfcx = min(scrnxmax, textxl);
	bl = textxl - surfcx;
	if (scrnpos.x > bl) {
		scrnpos.x = bl;
	}

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
	bl = crtc.e.yl - surfcy;
	if (scrnpos.y > bl) {
		scrnpos.y = bl;
	}

	makescrn.surfcx = surfcx;
	makescrn.surfrx = textxl - surfcx;
	makescrn.surfcy = surfcy;
	makescrn.surfstep = (ADVV_WIDTH * charcy) - (surfcx * charcx);
	makescrn.vramsize = min(0x800, surfcy * textxl);
}

void scrnupdate(void) {

	REG8	flag;
	REG8	existblink;
	UINT	fontycnt;
	UINT	pos;
	UINT	y;
	REG8	udtmp;

	if (!corestat.drawframe) {
		return;
	}
	corestat.drawframe = 0;

	flag = 0;
	if (crtc.e.scrnflash) {
		crtc.e.scrnflash = 0;
		flag |= SCRNUPD_FLASH;
	}
	if (crtc.e.scrnallflash) {
		crtc.e.scrnallflash = 0;
		flag |= SCRNUPD_ALLFLASH;
	}
	if (crtc.e.palandply) {
		crtc.e.palandply = 0;
		flag |= SCRNUPD_PALANDPLY;
	}

	if (makescrn.dispmode != crtc.e.dispmode) {
		TRACEOUT(("change mode!"));
		changemodes();
		flag |= SCRNUPD_ALLFLASH | SCRNUPD_PALANDPLY;
	}
	if (flag & SCRNUPD_ALLFLASH) {
		changecrtc();
		fillupdatetmp();
	}
	if (crtc.e.remakeattr) {
		crtc.e.remakeattr = 0;
		flashupdatetmp();
	}
	if (flag & SCRNUPD_PALANDPLY) {
		pal_update();
	}

	if (crtc.e.blinktime) {
		crtc.e.blinktime--;
	}
	else {
		crtc.e.blinktime = 30 - 1;
		if (crtc.e.existblink) {
			existblink = updateblink();
			crtc.e.existblink = existblink;
			flag |= existblink;
		}
	}

	if (flag & (SCRNUPD_FLASH | SCRNUPD_ALLFLASH)) {
		if (makescrn.vramsize) {
			fontycnt = 0;
			pos = makescrn.vramtop;
			y = scrnpos.y;
			if (y) {
				do {
					udtmp = TRAMUPDATE(pos);
					if (udtmp & 4) {
						fontycnt += makescrn.charcy;
					}
					else {
						fontycnt = makescrn.charcy * 2;
					}
					pos = LOW11(pos + crtc.s.reg[CRTCREG_HDISP]);
				} while(--y);
				fontycnt = fontycnt & 15;
			}
			makescrn.fontycnt = fontycnt;
			pos += scrnpos.x;
			screenmake[makescrn.drawmode](pos);
			advv_bufferupdate();
		}
	}
}


void makescrn_initialize(void) {

	CopyMemory(makescrn.patx2, patx2, 0x100);
	vsyncff_init();
}

void makescrn_reset(void) {

	changemodes();
	changecrtc();
	flashupdatetmp();
}

