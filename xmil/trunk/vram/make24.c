#include	"compiler.h"
#include	"scrnmng.h"
#include	"pccore.h"
#include	"vram.h"
#include	"scrndraw.h"
#include	"makescrn.h"
#include	"makesub.h"
#include	"font.h"


void width80x25_200h(void) {								// 80x25 200line

	UINT	fontcy;
	UINT	pos;
	BRESULT	newline;
	UINT8	*dst;
	UINT	y;
	UINT8	*lp;
	UINT	x;
	REG8	udtmp;
	REG8	dirty;
	UINT8	work[MAKETEXT_STEP * 2];
const UINT8	*src;
	UINT	lines;
	UINT	i;

	fontcy = min(makescrn.fontcy, MAKETEXT_ROW);
	pos = makescrn.vramtop;
	dst = screenmap;
	newline = FALSE;
	y = makescrn.surfcy;
	lp = renewalline;
	do {
		x = makescrn.surfcx;
		do {
			udtmp = TRAMUPDATE(pos);
			dirty = udtmp & makescrn.dispflag;
			if (dirty) {
				TRAMUPDATE(pos) = (UINT8)(udtmp ^ dirty);
				newline = TRUE;
				if (dirty & UPDATE_TRAM) {
					ZeroMemory(work, sizeof(work));
					makechr16(work, pos, fontcy, udtmp);
					makemix_mixtext(dst, SURFACE_WIDTH * 2, work, fontcy);
					makemix_mixtext(dst + SURFACE_WIDTH, SURFACE_WIDTH * 2,
											work + MAKETEXT_STEP, fontcy);
				}
				if (dirty & UPDATE_VRAM) {
					src = makescrn.disp1 + TRAM2GRAM(pos);
					makemix_mixgrph(dst, SURFACE_WIDTH * 2, src, fontcy);
					makemix_mixgrph(dst + SURFACE_WIDTH, SURFACE_WIDTH * 2,
															src, fontcy);
				}
				if (fontcy < makescrn.fontcy) {
					makemix_cpy400(dst, fontcy * 2, makescrn.fontcy * 2);
				}
			}
			pos = LOW11(pos + 1);
			dst += 8;
		} while(--x);

		pos = LOW11(pos + makescrn.surfrx);
		if (TRAMUPDATE(LOW11(pos - 1)) & 4) {			// c”{Šp•`‰æ‚¾‚Á‚½‚ç
			makescrn.fontycnt += makescrn.charcy;
		}
		else {
			makescrn.fontycnt = makescrn.charcy * 2;
		}
		makescrn.fontycnt &= 15;
		dst += makescrn.surfstep;
		lines = makescrn.charcy * 2;
		if (newline) {
			newline = FALSE;
			for (i=0; i<lines; i++) {
				lp[i] = 1;
			}
		}
		lp += lines;
	} while(--y);
}

void width80x25_400h(void) {								// 80x25 400line

	UINT	fontcy;
	UINT	pos;
	BRESULT	newline;
	UINT8	*dst;
	UINT	y;
	UINT8	*lp;
	UINT	x;
	REG8	udtmp;
	UINT8	work[MAKETEXT_STEP * 2];
const UINT8	*src;
	UINT	lines;
	UINT	i;

	fontcy = min(makescrn.fontcy, MAKETEXT_ROW);
	pos = makescrn.vramtop;
	dst = screenmap;
	newline = FALSE;
	y = makescrn.surfcy;
	lp = renewalline;
	do {
		x = makescrn.surfcx;
		do {
			udtmp = TRAMUPDATE(pos);
			if (udtmp & UPDATE_TVRAM) {
				TRAMUPDATE(pos) = (UINT8)(udtmp & (~UPDATE_TVRAM));
				newline = TRUE;
				if (udtmp & UPDATE_TRAM) {
					ZeroMemory(work, sizeof(work));
					makechr16(work, pos, fontcy, udtmp);
					makemix_mixtext(dst + SURFACE_WIDTH * 0, SURFACE_WIDTH * 2,
												work, fontcy);
					makemix_mixtext(dst + SURFACE_WIDTH * 1, SURFACE_WIDTH * 2,
												work + MAKETEXT_STEP, fontcy);
				}
				src = gram + TRAM2GRAM(pos);
				if (udtmp & UPDATE_VRAM0) {
					makemix_mixgrph(dst + SURFACE_WIDTH * 0, SURFACE_WIDTH * 2,
												src + GRAM_BANK0, fontcy);
				}
				if (udtmp & UPDATE_VRAM1) {
					makemix_mixgrph(dst + SURFACE_WIDTH * 1, SURFACE_WIDTH * 2,
												src + GRAM_BANK1, fontcy);
				}
				if (fontcy < makescrn.fontcy) {
					makemix_cpy400(dst, fontcy * 2, makescrn.fontcy * 2);
				}
			}
			pos = LOW11(pos + 1);
			dst += 8;
		} while(--x);

		pos = LOW11(pos + makescrn.surfrx);
		if (TRAMUPDATE(LOW11(pos - 1)) & 4) {			// c”{Šp•`‰æ‚¾‚Á‚½‚ç
			makescrn.fontycnt += makescrn.charcy;
		}
		else {
			makescrn.fontycnt = makescrn.charcy * 2;
		}
		makescrn.fontycnt &= 15;
		dst += makescrn.surfstep;
		lines = makescrn.charcy * 2;
		if (newline) {
			newline = FALSE;
			for (i=0; i<lines; i++) {
				lp[i] = 1;
			}
		}
		lp += lines;
	} while(--y);
}

void width80x12_200h(void) {								// 80x12 200line

	UINT	fontcy;
	UINT	pos;
	BRESULT	newline;
	UINT8	*dst;
	UINT	y;
	UINT8	*lp;
	UINT	x;
	REG8	udtmp;
	REG8	dirty;
	UINT8	work[MAKETEXT_STEP * 2];
const UINT8	*src;
	UINT	lines;
	UINT	i;

	fontcy = min(makescrn.fontcy, MAKETEXT_ROW);
	pos = makescrn.vramtop;
	dst = screenmap;
	newline = FALSE;
	y = makescrn.surfcy;
	lp = renewalline;
	do {
		x = makescrn.surfcx;
		do {
			udtmp = TRAMUPDATE(pos);
			dirty = udtmp & makescrn.dispflag;
			if (dirty) {
				TRAMUPDATE(pos) = (UINT8)(udtmp ^ dirty);
				newline = TRUE;
				if (dirty & UPDATE_TRAM) {
					ZeroMemory(work, sizeof(work));
					makechr16(work, pos, fontcy, udtmp);
					makemix_mixtext(dst, SURFACE_WIDTH * 4, work, fontcy);
					makemix_mixtext(dst + SURFACE_WIDTH * 2, SURFACE_WIDTH * 4,
											work + MAKETEXT_STEP, fontcy);
				}
				if (dirty & UPDATE_VRAM) {
					src = makescrn.disp1 + TRAM2GRAM(LOW10(pos));
					makemix_mixgrph(dst, SURFACE_WIDTH * 4, src, fontcy);
					makemix_mixgrph(dst + SURFACE_WIDTH * 2, SURFACE_WIDTH * 4,
											src + GRAM_HALFSTEP, fontcy);
				}
			//	makemix_doubler(dst, fontcy * 2, 0);
				if (fontcy < makescrn.fontcy) {
					makemix_cpy200(dst, fontcy * 2, makescrn.fontcy * 2);
				}
			}
			pos = LOW11(pos + 1);
			dst += 8;
		} while(--x);

		pos = LOW11(pos + makescrn.surfrx);
		if (TRAMUPDATE(LOW11(pos - 1)) & 4) {			// c”{Šp•`‰æ‚¾‚Á‚½‚ç
			makescrn.fontycnt += makescrn.charcy;
		}
		else {
			makescrn.fontycnt = makescrn.charcy * 2;
		}
		makescrn.fontycnt &= 15;
		dst += makescrn.surfstep;
		lines = makescrn.charcy * 4;
		if (newline) {
			newline = FALSE;
			for (i=0; i<lines; i++) {
				lp[i] = 1;
			}
		}
		lp += lines;
	} while(--y);
}

void width80x12_400h(void) {								// 80x12 400line

	UINT	fontcy;
	UINT	pos;
	BRESULT	newline;
	UINT8	*dst;
	UINT	y;
	UINT8	*lp;
	UINT	x;
	REG8	udtmp;
	UINT8	work[MAKETEXT_STEP * 2];
const UINT8	*src;
	UINT	lines;
	UINT	i;

	fontcy = min(makescrn.fontcy, MAKETEXT_ROW);
	pos = makescrn.vramtop;
	dst = screenmap;
	newline = FALSE;
	y = makescrn.surfcy;
	lp = renewalline;
	do {
		x = makescrn.surfcx;
		do {
			udtmp = TRAMUPDATE(pos);
			if (udtmp & UPDATE_TVRAM) {
				TRAMUPDATE(pos) = (UINT8)(udtmp & (~UPDATE_TVRAM));
				newline = TRUE;
				if (udtmp & UPDATE_TRAM) {
					ZeroMemory(work, sizeof(work));
					makechr16(work, pos, fontcy, udtmp);
					makemix_mixtext(dst + SURFACE_WIDTH * 0, SURFACE_WIDTH * 4,
											work, fontcy);
					makemix_mixtext(dst + SURFACE_WIDTH * 1, SURFACE_WIDTH * 4,
											work, fontcy);
					makemix_mixtext(dst + SURFACE_WIDTH * 2, SURFACE_WIDTH * 4,
											work + MAKETEXT_STEP, fontcy);
					makemix_mixtext(dst + SURFACE_WIDTH * 3, SURFACE_WIDTH * 4,
											work + MAKETEXT_STEP, fontcy);
				}
				src = gram + TRAM2GRAM(LOW10(pos));
				if (udtmp & UPDATE_VRAM0) {
					makemix_mixgrph(dst + SURFACE_WIDTH * 0, SURFACE_WIDTH * 4,
												src + GRAM_BANK0L, fontcy);
					makemix_mixgrph(dst + SURFACE_WIDTH * 2, SURFACE_WIDTH * 4,
												src + GRAM_BANK0H, fontcy);
				}
				if (udtmp & UPDATE_VRAM1) {
					makemix_mixgrph(dst + SURFACE_WIDTH * 1, SURFACE_WIDTH * 4,
												src + GRAM_BANK1L, fontcy);
					makemix_mixgrph(dst + SURFACE_WIDTH * 3, SURFACE_WIDTH * 4,
												src + GRAM_BANK1H, fontcy);
				}
				if (fontcy < makescrn.fontcy) {
					makemix_cpy400(dst, fontcy * 4, makescrn.fontcy * 4);
				}
			}
			pos = LOW11(pos + 1);
			dst += 8;
		} while(--x);

		pos = LOW11(pos + makescrn.surfrx);
		if (TRAMUPDATE(LOW11(pos - 1)) & 4) {			// c”{Šp•`‰æ‚¾‚Á‚½‚ç
			makescrn.fontycnt += makescrn.charcy;
		}
		else {
			makescrn.fontycnt = makescrn.charcy * 2;
		}
		makescrn.fontycnt &= 15;
		dst += makescrn.surfstep;
		lines = makescrn.charcy * 4;
		if (newline) {
			newline = FALSE;
			for (i=0; i<lines; i++) {
				lp[i] = 1;
			}
		}
		lp += lines;
	} while(--y);
}

void width80x20h(void) {

	UINT	fontcy;
	UINT	pos;
	BRESULT	newline;
	UINT8	*dst;
	UINT	y;
	UINT8	*lp;
	UINT	x;
	REG8	udtmp;
	UINT8	work[MAKETEXT_STEP * 2];
	UINT	lines;
	UINT	i;

	fontcy = min(makescrn.fontcy, MAKETEXT_ROW);
	pos = makescrn.vramtop;
	dst = screenmap;
	newline = FALSE;
	y = makescrn.surfcy;
	lp = renewalline;
	do {
		x = makescrn.surfcx;
		do {
			udtmp = TRAMUPDATE(pos);
			if (udtmp & UPDATE_TRAM) {
				TRAMUPDATE(pos) = (UINT8)(udtmp & (~UPDATE_TRAM));
				newline = TRUE;
				ZeroMemory(work, sizeof(work));
				makechr16(work, pos, fontcy, udtmp);
				makemix_settext(dst, SURFACE_WIDTH * 2, work, fontcy);
				makemix_settext(dst + SURFACE_WIDTH, SURFACE_WIDTH * 2,
												work + MAKETEXT_STEP, fontcy);
				if (fontcy < makescrn.fontcy) {
					makemix_cpy400(dst, fontcy * 2, makescrn.fontcy * 2);
				}
				makemix_ul20(dst + SURFACE_WIDTH * makescrn.fontcy * 2, pos);
			}
			pos = LOW11(pos + 1);
			dst += 8;
		} while(--x);

		pos = LOW11(pos + makescrn.surfrx);
		if (TRAMUPDATE(LOW11(pos - 1)) & 4) {			// c”{Šp•`‰æ‚¾‚Á‚½‚ç
			makescrn.fontycnt += makescrn.charcy;
		}
		else {
			makescrn.fontycnt = makescrn.charcy * 2;
		}
		makescrn.fontycnt &= 15;
		dst += makescrn.surfstep;
		lines = makescrn.charcy * 2;
		if (newline) {
			newline = FALSE;
			for (i=0; i<lines; i++) {
				lp[i] = 1;
			}
		}
		lp += lines;
	} while(--y);
}

void width80x10h(void) {

	UINT	fontcy;
	UINT	pos;
	BRESULT	newline;
	UINT8	*dst;
	UINT	y;
	UINT8	*lp;
	UINT	x;
	REG8	udtmp;
	UINT8	work[MAKETEXT_STEP * 2];
	UINT	lines;
	UINT	i;

	fontcy = min(makescrn.fontcy, MAKETEXT_ROW);
	pos = makescrn.vramtop;
	dst = screenmap;
	newline = FALSE;
	y = makescrn.surfcy;
	lp = renewalline;
	do {
		x = makescrn.surfcx;
		do {
			udtmp = TRAMUPDATE(pos);
			if (udtmp & UPDATE_TRAM) {
				TRAMUPDATE(pos) = (UINT8)(udtmp & (~UPDATE_TRAM));
				newline = TRUE;
				ZeroMemory(work, sizeof(work));
				makechr16(work, pos, fontcy, udtmp);
				makemix_settext(dst, SURFACE_WIDTH * 4, work, fontcy);
				makemix_settext(dst + SURFACE_WIDTH * 2, SURFACE_WIDTH * 4,
											work + MAKETEXT_STEP, fontcy);
				if (fontcy < makescrn.fontcy) {
					makemix_cpy200(dst, fontcy * 2, makescrn.fontcy * 2);
				}
				makemix_ul10(dst + SURFACE_WIDTH * makescrn.fontcy * 4, pos);
			}
			pos = LOW11(pos + 1);
			dst += 8;
		} while(--x);

		pos = LOW11(pos + makescrn.surfrx);
		if (TRAMUPDATE(LOW11(pos - 1)) & 4) {			// c”{Šp•`‰æ‚¾‚Á‚½‚ç
			makescrn.fontycnt += makescrn.charcy;
		}
		else {
			makescrn.fontycnt = makescrn.charcy * 2;
		}
		makescrn.fontycnt &= 15;
		dst += makescrn.surfstep;
		lines = makescrn.charcy * 4;
		if (newline) {
			newline = FALSE;
			for (i=0; i<lines; i++) {
				lp[i] = 1;
			}
		}
		lp += lines;
	} while(--y);
}

