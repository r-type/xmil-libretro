#include	"compiler.h"
#include	"scrnmng.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"vram.h"
#include	"scrndraw.h"
#include	"palettes.h"
#include	"makescrn.h"
#include	"makesub.h"


		MAKESCRN	makescrn;

		BYTE	scrnallflash;
static	BYTE	lastdisp = 0;
static	BYTE	blinktime = 1;
		DWORD	drawtime = 0;


static void fillupdatetmp(void) {

	UINT32	*p;
	UINT	i;

	p = (UINT32 *)updatetmp;
	for (i=0; i<0x200; i++) {
		p[i] |= (UPDATE_TVRAM << 24) | (UPDATE_TVRAM << 16) |
										(UPDATE_TVRAM << 8) | UPDATE_TVRAM;
	}
}

#if 1
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

	posl = crtc.s.TXT_TOP;
	y = crtc.s.TXT_YL;
	do {
		for (x=0; x<crtc.s.TXT_XL; x++) {
			if (!(tram[TRAM_ATR + LOW11(posl + x)] & TRAMATR_Yx2)) {
				break;
			}
		}
		y2 = (x < crtc.s.TXT_XL)?FALSE:TRUE;
		udtbase = (x < crtc.s.TXT_XL)?0x0000:0x0404;
		r = (crtc.s.TXT_XL + 1) >> 1;
		do {
			posr = LOW11(posl + 1);
			atr = (tram[TRAM_ATR + posl] << 8) | tram[TRAM_ATR + posr];
			udt = udtbase;
			if (!y2) {
				if (atr & (TRAMATR_Yx2 << 8)) {
					udt |= (UPDATE_TRAM | 1) << 8;		// ç∂í◊ÇÍècî{äp
				}
				else {
					y2 = TRUE;
				}
			}
			if (!y2) {
				if (atr & (TRAMATR_Yx2 << 0)) {
					udt |= (UPDATE_TRAM | 1) << 0;		// âEí◊ÇÍècî{äp
				}
				else {
					y2 = TRUE;
				}
			}
			if (atr & (TRAMATR_Xx2 << 8)) {				// ç∂ë§î{äp?
				udt |= 0x0812;
			}
			if (atr & (TRAMATR_Xx2 << 0)) {				// âEë§î{äp?
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
		if (crtc.s.TXT_XL & 1) {
			posl = LOW11(posl - 1);
		}
	} while(--y);
}
#else
static LABEL void flashupdatetmp(void) {

		__asm {
				push	ebx
				push	esi
				push	edi

				movzx	esi, crtc.s.TXT_TOP
				xor		dl, dl
										// Ç‹Ç∏çsÇ∑Ç◊ÇƒÇ™ècî{äpÇ©í≤Ç◊ÇÈ
check_tateflag:	mov		edi, esi
				movzx	ecx, crtc.s.TXT_XL
tateflaglp_s:	and		edi, (TRAM_MAX - 1)
				test	tram[TRAM_ATR + edi], X1ATR_Yx2
				je		tatex1
				inc		edi
				loop	tateflaglp_s
										// Ç∑Ç◊Çƒècî{äp ÇæÇ¡ÇΩÇÁ

				movzx	ecx, crtc.s.TXT_XL
				shr		cl, 1

tatex2loop_s:	and		esi, (TRAM_MAX - 1)
				mov		ax, (UPDATE_TVRAM or 04h) * 0101h		// ècî{äp
				mov		bx, word ptr (tram[TRAM_ATR + esi])

				test	bl, X1ATR_Xx2
				je		tatex2_norleft
												// î{äpÉtÉâÉOÇ™óßÇ¡ÇƒÇÈ
				or		ax, 08h + 12h * 256		// ç∂ÇSî{äp + âEècî{äpÇÕämíË
				test	bh, X1ATR_Xx2
				je		tatex2_pcg
				or		ah, 8					// âEë§Ç‡î{äpÉrÉbÉgÇ™óßÇ¡ÇƒÇΩ
				jmp		tatex2_pcg

tatex2_norleft:	test	bh, X1ATR_Xx2			// ÇSî{äpÇ≈Ç»Ç¢èÍçá
				je		tatex2_pcg
				or		ah, 8					// âEë§ÇÃÇ›î{äpÇæÇ¡ÇΩ
tatex2_pcg:		mov		bx, word ptr (updatetmp[esi])
				and		bx, UPDATE_TVRAM * 0101h
				or		ax, bx					// UPDATEÉtÉâÉOÇâ¡Ç¶ÇÈ
				cmp		word ptr (updatetmp[esi]), ax
				je		tatex2noupdate
				or		ax, UPDATE_TVRAM * 0101h
				mov		word ptr (updatetmp[esi]), ax
tatex2noupdate:	add		esi, 2
				loop	tatex2loop_s
				jmp		nextlinecheck

tatex1:			movzx	ecx, crtc.s.TXT_XL
				shr		cl, 1
				xor		dh, dh
tatex1loop_s:	and		esi, (TRAM_MAX - 1)
				xor		ax, ax
				mov		bx, word ptr (tram[TRAM_ATR + esi])
				or		dh, dh					// ècÉmÅ[É}ÉãÇÕä˘Ç…Ç†Ç¡ÇΩÇ©ÅH
				jne		tatex1_tate_e
				test	bl, X1ATR_Yx2
				je		tatex1_tate0
				mov		al, UPDATE_TVRAM or 1	// ç∂í◊ÇÍècî{äp
				test	bh, X1ATR_Yx2
				je		tatex1_tate0
				mov		ah, UPDATE_TVRAM or 1	// âEí◊ÇÍècî{äp
				jmp		tatex1_tate_e
tatex1_tate0:	inc		dh						// ècÉmÅ[É}ÉãÇÃèoåª
tatex1_tate_e:
				test	bl, X1ATR_Xx2
				je		tatex1_norleft
												// î{äpÉtÉâÉOÇ™óßÇ¡ÇƒÇÈ
				or		ax, 08h + (12h * 256)	// ç∂â°î{äp ÇÕämíË
				test	bh, X1ATR_Xx2
				je		tatex1_pcg
				or		ah, 8					// âEë§Ç‡î{äpÉrÉbÉgÇ™óßÇ¡ÇƒÇΩ
				jmp		tatex1_pcg

tatex1_norleft:	test	bh, X1ATR_Xx2			// î{äpÇ≈Ç»Ç¢èÍçá
				je		tatex1_pcg
				or		ah, 8					// âEë§ÇÃÇ›î{äpÇæÇ¡ÇΩ
tatex1_pcg:		mov		bx, word ptr (updatetmp[esi])
				and		bx, UPDATE_TVRAM * 0101h
				or		ax, bx
				cmp		word ptr (updatetmp[esi]), ax
				je		tatex1noupdate
				or		ax, UPDATE_TVRAM * 0101h
				mov		word ptr (updatetmp[esi]), ax
tatex1noupdate:	add		esi, 2
				loop	tatex1loop_s

nextlinecheck:	inc		dl
				cmp		dl, crtc.s.TXT_YL
				jb		check_tateflag

				pop		edi
				pop		esi
				pop		ebx
				ret
	}
}
#endif

static BRESULT updateblink(void) {

	UINT	pos;
	REG8	update;
	UINT	r;

	if (blinktime) {
		blinktime--;
		return(FALSE);
	}
	else {
		blinktime = 30 - 1;
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

static void changemodes(void) {

	lastdisp = crtc.e.dispmode;
	if (!(lastdisp & SCRN_BANK1)) {
		makescrn.disp1 = GRP_RAM + GRAM_BANK0;
		makescrn.disp2 = GRP_RAM + GRAM_BANK1;
		makescrn.dispflag = UPDATE_TRAM | UPDATE_VRAM0;
	}
	else {
		makescrn.disp1 = GRP_RAM + GRAM_BANK1;
		makescrn.disp2 = GRP_RAM + GRAM_BANK0;
		makescrn.dispflag = UPDATE_TRAM | UPDATE_VRAM1;
	}
	scrnallflash = 1;
	makescrn.palandply = 1;
}

static void changecrtc(void) {

	REG8	widthmode;
	UINT	fontcy;
	UINT	underlines;
	REG8	y2;
	UINT	charcy;
	UINT	surfcy;

	makescrn.vramtop = LOW11(crtc.s.TXT_TOP);

	if (crtc.s.TXT_XL <= 40) {
		if (lastdisp & SCRN_DRAW4096) {
			widthmode = SCRNWIDTHMODE_4096;
		}
		else {
			widthmode = SCRNWIDTHMODE_WIDTH40;
		}
	}
	else {
		widthmode = SCRNWIDTHMODE_WIDTH80;
	}
	scrndraw_changewidth(widthmode);

	makescrn.surfcx = min(80, crtc.s.TXT_XL);
	makescrn.surfrx = crtc.s.TXT_XL - makescrn.surfcx;

	fontcy = crtc.s.FNT_YL;
	if (crtc.s.SCRN_BITS & SCRN_24KHZ) {
		fontcy >>= 1;
	}
	underlines = (crtc.s.SCRN_BITS & SCRN_UNDERLINE)?2:0;
	if (fontcy > underlines) {
		fontcy -= underlines;
	}
	else {
		fontcy = 0;
	}
	y2 = (crtc.s.SCRN_BITS & SCRN_TEXTYx2)?1:0;
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
	surfcy = 200 / charcy;
	if (surfcy > crtc.s.TXT_YL) {
		surfcy = crtc.s.TXT_YL;
	}
	makescrn.surfcy = surfcy;
	makescrn.surfstep = (SURFACE_WIDTH * charcy * 2) - (makescrn.surfcx * 8);
	makescrn.vramsize = min(0x800, surfcy * crtc.s.TXT_XL);
	scrnmng_setheight(0, charcy * surfcy * 2);
}


// -------------------------------------------------------------------------

typedef void (*DRAWFN)(void);

static const DRAWFN screendraw[8] = {
				width80x25_200l,	width80x25_400h,
				width80x25_200l,	width80x25_200h,
				width80x12_200l,	width80x12_400h,
				width80x12_200l,	width80x12_200h};

static const DRAWFN screendraw2[8] = {
				width80x20l,		width80x20h,
				width80x20l,		width80x20h,
				width80x10l,		width80x10h,
				width80x10l,		width80x10h};


void scrnupdate(void) {

	BRESULT	ddrawflash;

	if (!corestat.drawframe) {
		return;
	}
	corestat.drawframe = 0;

	ddrawflash = FALSE;
	if (lastdisp != crtc.e.dispmode) {
		changemodes();
	}
	if (scrnallflash) {
		scrnallflash = 0;
		fillupdatetmp();
		changecrtc();
		makescrn.scrnflash = 1;
	}
	if (makescrn.remakeattr) {
		makescrn.remakeattr = 0;
		flashupdatetmp();
	}
	if (makescrn.palandply) {
		makescrn.palandply = 0;
		pal_update();
		ddrawflash = 1;
	}
	if (makescrn.existblink) {
		makescrn.scrnflash |= updateblink();
	}

	if (makescrn.scrnflash) {
		makescrn.scrnflash = 0;
		makescrn.fontycnt = 0;
		switch(lastdisp & SCRN64_MASK) {
			case SCRN64_320x200:
//				width40x25_64s();
				break;

			case SCRN64_L320x200x2:
//				width40x25_64x2();
				break;

			case SCRN64_L640x200:
//				width80x25_64s();
				break;

			case SCRN64_H320x400:
//				width40x25_64h();
				break;

			case SCRN64_320x200x4096:
//				width40x25_4096();
				break;

			case SCRN64_320x100:
//				width40x12_64l();
				break;

			case SCRN64_320x100x2:
//				width40x12_64x2();
				break;

			case SCRN64_L640x100:
//				width80x12_64s();
				break;

			case SCRN64_H320x200:
//				width40x12_64h();
				break;

			case SCRN64_320x100x4096:
//				width40x12_4096();
				break;

//			case SCRN64_INVALID:
			default:
				if (!(crtc.s.SCRN_BITS & SCRN_UNDERLINE)) {
					screendraw[crtc.s.SCRN_BITS & 7]();
				}
				else {
					screendraw2[crtc.s.SCRN_BITS & 7]();
				}
				break;
		}
		ddrawflash = 1;
	}

	if (ddrawflash) {
		ddrawflash = 0;
		scrndraw_draw(FALSE);
		drawtime++;
	}
}


void makescrn_initialize(void) {

	makesub_initialize();
}

void makescrn_reset(void) {

	fillupdatetmp();
	changemodes();
	changecrtc();
}

