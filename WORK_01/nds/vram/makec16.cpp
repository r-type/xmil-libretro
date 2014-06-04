#include	"compiler.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"vram.h"
#include	"makescrn.h"
#include	"makesub.h"
#include	"font.h"


static void copy2odd(UINT8 *dst, UINT8 *term) {

	do {
		*(UINT32 *)(dst + (MAKETEXT_ROW * 0)) =
						*(UINT32 *)(dst + MAKETEXT_STEP + (MAKETEXT_ROW * 0));
		*(UINT32 *)(dst + (MAKETEXT_ROW * 1)) =
						*(UINT32 *)(dst + MAKETEXT_STEP + (MAKETEXT_ROW * 1));
		*(UINT32 *)(dst + (MAKETEXT_ROW * 2)) =
						*(UINT32 *)(dst + MAKETEXT_STEP + (MAKETEXT_ROW * 2));
		dst += 4;
	} while(dst < term);
}

void makechr16(UINT8 *dst, UINT pos, UINT count, REG8 udtmp) {

	REG8		atr;
	REG8		ank;
	REG8		knj;
const UINT8		*pat;
	REG8		func;
	UINT8		*term;

	atr = TRAM_ATR(pos);
	if (atr & makescrn.blinktest) {
		atr ^= TRAMATR_REVERSE;
	}
	if (udtmp & 0x10) {
		pos = LOW11(pos - 1);
	}
	ank = TRAM_ANK(pos);
	knj = TRAM_KNJ(pos);
	func = udtmp & 0x0f;
	term = dst + count;
	if (!(TRAM_ATR(pos) & 0x20)) {				// CHR
		if (!(knj & 0x80)) {					// ASCII
			pat = font_txt + (ank << 4);
		}
		else {									// KANJI
			pat = font_knjx1t;
			pat += (knj & 0x1f) << 12;
			pat += ank << 4;
			if (knj & 0x40) {
				pat += FONTX1T_LR;
			}
		}
		func += 0x10;
		maketxt8_make(dst, term, pat, func, atr);
		maketxt8_make(dst + MAKETEXT_STEP, term + MAKETEXT_STEP,
														pat + 1, func, atr);
	}
	else {										// PCG
		if (!(knj & 0x90)) {					// PCGの出力
			pat = pcg.d + (ank << 3);
		}
		else {									// 16ドットPCGの出力
			pat = pcg.d + ((ank & (~1)) << 3);
			func += 0x10;
		}
		makepcg8_make(dst, term, pat, func, atr);
		if (!(func & 0x10)) {
			copy2odd(dst, term);
		}
		else {
			makepcg8_make(dst + MAKETEXT_STEP, term + MAKETEXT_STEP,
														pat + 1, func, atr);
		}
	}
}

