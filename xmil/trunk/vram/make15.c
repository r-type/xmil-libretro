#include	"compiler.h"
#include	"scrnmng.h"
#include	"pccore.h"
#include	"vram.h"
#include	"scrndraw.h"
#include	"makescrn.h"
#include	"makesub.h"
#include	"font.h"


extern	BYTE	dispflg;
extern	BYTE	*dispp;


void width80x25_200l(void) {								// 80x25 200line

	UINT	fontcy;
	UINT	pos;
	BRESULT	newline;
	UINT8	*dst;
	UINT	y;
	UINT8	*lp;
	UINT	x;
	REG8	udtmp;
	UINT8	work[MAKETEXT_ROW * 3];
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
			udtmp = updatetmp[pos];
			if (udtmp & dispflg) {
				updatetmp[pos] = (UINT8)(udtmp & (~dispflg));
				newline = TRUE;
				ZeroMemory(work, sizeof(work));
				makechr8(work, pos, fontcy, udtmp);
				makemix_mix(dst, SURFACE_WIDTH * 2,
									work, dispp + (pos << 5), fontcy);
				makemix_doubler(dst, fontcy, 0x40404040);
				if (fontcy < makescrn.fontcy) {
					makemix_remcpy(dst, fontcy * 2, makescrn.fontcy * 2);
				}
			}
			pos = LOW11(pos + 1);
			dst += 8;
		} while(--x);

		pos = LOW11(pos + makescrn.surfrx);
		if (updatetmp[LOW11(pos - 1)] & 4) {			// c”{Šp•`‰æ‚¾‚Á‚½‚ç
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

void width80x12_200l(void) {								// 80x12 200line

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
			udtmp = updatetmp[pos];
			if (udtmp & dispflg) {
				updatetmp[pos] = (UINT8)(udtmp & (~dispflg));
				newline = TRUE;
				ZeroMemory(work, sizeof(work));
				makechr16(work, pos, fontcy, udtmp);
				src = dispp + (LOW10(pos) << 5);
				makemix_mix(dst, SURFACE_WIDTH * 4, work, src, fontcy);
				makemix_mix(dst + SURFACE_WIDTH * 2, SURFACE_WIDTH * 4,
							work + MAKETEXT_STEP, src + GRAM_HALFSTEP, fontcy);
				makemix_doubler(dst, fontcy * 2, 0x40404040);
				if (fontcy < makescrn.fontcy) {
					makemix_remcpy(dst, fontcy * 4, makescrn.fontcy * 4);
				}
			}
			pos = LOW11(pos + 1);
			dst += 8;
		} while(--x);

		pos = LOW11(pos + makescrn.surfrx);
		if (updatetmp[LOW11(pos - 1)] & 4) {			// c”{Šp•`‰æ‚¾‚Á‚½‚ç
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

void width80x20l(void) {

	UINT	fontcy;
	UINT	pos;
	BRESULT	newline;
	UINT8	*dst;
	UINT	y;
	UINT8	*lp;
	UINT	x;
	REG8	udtmp;
	UINT8	work[MAKETEXT_ROW * 3];
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
			udtmp = updatetmp[pos];
			if (udtmp & UPDATE_TRAM) {
				updatetmp[pos] = (UINT8)(udtmp & (~UPDATE_TRAM));
				newline = TRUE;
				ZeroMemory(work, sizeof(work));
				makechr8(work, pos, fontcy, udtmp);
				makemix_txt(dst, SURFACE_WIDTH * 2, work, fontcy);
				makemix_doubler(dst, fontcy, 0x40404040);
				if (fontcy < makescrn.fontcy) {
					makemix_remcpy(dst, fontcy * 2, makescrn.fontcy * 2);
				}
				makemix_ul20(dst + SURFACE_WIDTH * makescrn.fontcy * 2,
															pos, 0x40404040);
			}
			pos = LOW11(pos + 1);
			dst += 8;
		} while(--x);

		pos = LOW11(pos + makescrn.surfrx);
		if (updatetmp[LOW11(pos - 1)] & 4) {			// c”{Šp•`‰æ‚¾‚Á‚½‚ç
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

void width80x10l(void) {

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
			udtmp = updatetmp[pos];
			if (udtmp & UPDATE_TRAM) {
				updatetmp[pos] = (UINT8)(udtmp & (~UPDATE_TRAM));
				newline = TRUE;
				ZeroMemory(work, sizeof(work));
				makechr16(work, pos, fontcy, udtmp);
				makemix_txt(dst, SURFACE_WIDTH * 4, work, fontcy);
				makemix_txt(dst + SURFACE_WIDTH * 2, SURFACE_WIDTH * 4,
											work + MAKETEXT_STEP, fontcy);
				makemix_doubler(dst, fontcy * 2, 0x40404040);
				if (fontcy < makescrn.fontcy) {
					makemix_remcpy(dst, fontcy * 4, makescrn.fontcy * 4);
				}
				makemix_ul10(dst + SURFACE_WIDTH * makescrn.fontcy * 4,
															pos, 0x40404040);
			}
			pos = LOW11(pos + 1);
			dst += 8;
		} while(--x);

		pos = LOW11(pos + makescrn.surfrx);
		if (updatetmp[LOW11(pos - 1)] & 4) {			// c”{Šp•`‰æ‚¾‚Á‚½‚ç
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

