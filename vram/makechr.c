#include	"compiler.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"vram.h"
#include	"makescrn.h"
#include	"makesub.h"
#include	"font.h"


void makechr8(UINT8 *dst, UINT pos, UINT count, REG8 udtmp) {

	REG8		atr;
	REG8		ank;
	REG8		knj;
const UINT8		*pat;
	MAKETXTFN	fn;

	atr = TRAM_ATR(pos);
	if (atr & makescrn.blinktest) {
		atr ^= TRAMATR_REVERSE;
	}
	if (udtmp & 0x10) {
		pos = LOW11(pos - 1);
	}
	ank = TRAM_ANK(pos);
	knj = TRAM_KNJ(pos);
	if (!(TRAM_ATR(pos) & 0x20)) {				// CHR
		if (!(knj & 0x80)) {					// ASCII
			pat = font_ank + (ank << 3);
			fn = maketxt8fn[udtmp & 15];
		}
		else {									// KANJI
			pat = font_knjx1t;
			pat += (knj & 0x1f) << 12;
			pat += ank << 4;
			if (knj & 0x40) {
				pat += FONTX1T_LR;
			}
			fn = makeknj8fn[udtmp & 15];
		}
		(*fn)(dst, dst + count, pat);
		(*makeatr8[atr & 15])(dst, dst + count);
	}
	else {										// PCG
		if (!(knj & 0x90)) {					// PCGの出力
			pat = pcg.d + (ank << 3);
			fn = maketxt8fn[udtmp & 15];
		}
		else {									// 16ドットPCGの出力
			pat = pcg.d + ((ank & (~1)) << 3);
			fn = makeknj8fn[udtmp & 15];
		}
		makeatr_pcg8(dst, count, pat, atr, fn);
	}
}

void makechr16(UINT8 *dst, UINT pos, UINT count, REG8 udtmp) {

	REG8		atr;
	REG8		ank;
	REG8		knj;
const UINT8		*pat;
	MAKETXTFN	fn;

	atr = TRAM_ATR(pos);
	if (atr & makescrn.blinktest) {
		atr ^= TRAMATR_REVERSE;
	}
	if (udtmp & 0x10) {
		pos = LOW11(pos - 1);
	}
	ank = TRAM_ANK(pos);
	knj = TRAM_KNJ(pos);
	if (!(TRAM_ATR(pos) & 0x20)) {				// CHR
		if (!(knj & 0x80)) {					// ASCII
			pat = font_txt;
		}
		else {									// KANJI
			pat = font_knjx1t;
			pat += (knj & 0x1f) << 12;
			if (knj & 0x40) {
				pat += FONTX1T_LR;
			}
		}
		fn = maketxt16fn[udtmp & 15];
		(*fn)(dst, dst + count, pat + (ank << 4));
		(*makeatr8[atr & 15])(dst, dst + count);
		(*makeatr8[atr & 15])(dst + MAKETEXT_STEP,
								dst + MAKETEXT_STEP + count);
	}
	else {										// PCG
		if (!(knj & 0x90)) {					// PCGの出力
			pat = pcg.d + (ank << 3);
			fn = makepcg16fn[udtmp & 15];
		}
		else {									// 16ドットPCGの出力
			pat = pcg.d + ((ank & (~1)) << 3);
			fn = maketxt16fn[udtmp & 15];
		}
		makeatr_pcg16(dst, count, pat, atr, fn);
	}
}

