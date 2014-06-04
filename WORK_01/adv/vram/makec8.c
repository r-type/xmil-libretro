#include	"compiler.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"vram.h"
#include	"makescrn.h"
#include	"makesub.h"
#include	"font.h"


void makechr8(UINT8 *dst, const MAKESCRN *m, UINT32 pos_cy, REG8 udtmp) {

	UINT		pos;
	UINT		count;
	REG8		atr;
	REG8		ank;
	REG8		knj;
const UINT8		*pat;
	REG8		func;

	pos = pos_cy >> 21;
	count = pos_cy & 0xff;

	atr = TRAM_ATR(pos);
	if (atr & m->blinktest) {
		atr ^= TRAMATR_REVERSE;
	}
	if (udtmp & 0x10) {
		pos = LOW11(pos - 1);
	}
	ank = TRAM_ANK(pos);
	knj = TRAM_KNJ(pos);
	func = udtmp & 0x0f;
	if (!(TRAM_ATR(pos) & 0x20)) {				// CHR
		if (!(knj & 0x80)) {					// ASCII
			pat = font_ank + (ank << 3);
		}
		else {									// KANJI
			pat = font_knjx1t;
			pat += (knj & 0x1f) << 12;
			pat += ank << 4;
			if (knj & 0x40) {
				pat += FONTX1T_LR;
			}
			func += 16;
		}
		maketxt8_make(dst, dst + count, pat, func, atr);
	}
	else {										// PCG
		if (!(knj & 0x90)) {					// PCGの出力
			pat = pcg.d + (ank << 3);
		}
		else {									// 16ドットPCGの出力
			pat = pcg.d + ((ank & (~1)) << 3);
			func += 16;
		}
		makepcg8_make(dst, dst + count, pat, func, atr);
	}
}

