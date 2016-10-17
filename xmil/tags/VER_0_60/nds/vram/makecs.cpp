#include	"compiler.h"
#include	"vram.h"
#include	"makescrn.h"
#include	"makesub.h"


void maketxt8_make(UINT8 *dst, UINT8 *term, const UINT8 *src,
													REG8 func, REG8 attr) {

	(*maketxtlfn[func])(dst, term, src);
	(*makeatr8[attr & 15])(dst, term);
}

static void cls8(UINT8 *dst, UINT8 *term) {

	do {
		*(UINT32 *)dst = 0;
		dst += 4;
	} while(dst < term);
}

void makepcg8_make(UINT8 *dst, UINT8 *term, const UINT8 *src,
													REG8 func, REG8 attr) {

	MAKETXTFN	fn;

	fn = maketxtlfn[func];

	if (attr & 1) {
		(*fn)(dst, term, src);
	}
	else {
		cls8(dst, term);
	}
	dst += MAKETEXT_ROW;
	term += MAKETEXT_ROW;
	if (attr & 2) {
		(*fn)(dst, term, src + 0x800);
	}
	else {
		cls8(dst, term);
	}
	dst += MAKETEXT_ROW;
	term += MAKETEXT_ROW;
	if (attr & 4) {
		(*fn)(dst, term, src + 0x1000);
	}
	else {
		cls8(dst, term);
	}
	if (attr & TRAMATR_REVERSE) {
		do {
			*(UINT32 *)(dst - (MAKETEXT_ROW * 2)) ^= (UINT32)-1;
			*(UINT32 *)(dst - (MAKETEXT_ROW * 1)) ^= (UINT32)-1;
			*(UINT32 *)(dst - (MAKETEXT_ROW * 0)) ^= (UINT32)-1;
			dst += 4;
		} while(dst < term);
	}
}

