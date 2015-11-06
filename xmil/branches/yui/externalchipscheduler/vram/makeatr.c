#include	"compiler.h"
#include	"makesub.h"
#include	"vram.h"


static void atrb0(UINT8 *dst, UINT8 *term) {

	do {
		*(UINT32 *)(dst + (MAKETEXT_ROW * 0)) = 0;
		dst += 4;
	} while(dst < term);
}

static void atrb1(UINT8 *dst, UINT8 *term) {

	(void)dst;
	(void)term;
}

static void atrb2(UINT8 *dst, UINT8 *term) {

	UINT32	dat;

	do {
		dat = *(UINT32 *)(dst + (MAKETEXT_ROW * 0));
		*(UINT32 *)(dst + (MAKETEXT_ROW * 0)) = 0;
		*(UINT32 *)(dst + (MAKETEXT_ROW * 1)) = dat;
		dst += 4;
	} while(dst < term);
}

static void atrb3(UINT8 *dst, UINT8 *term) {

	UINT32	dat;

	do {
		dat = *(UINT32 *)(dst + (MAKETEXT_ROW * 0));
		*(UINT32 *)(dst + (MAKETEXT_ROW * 1)) = dat;
		dst += 4;
	} while(dst < term);
}

static void atrb4(UINT8 *dst, UINT8 *term) {

	UINT32	dat;

	do {
		dat = *(UINT32 *)(dst + (MAKETEXT_ROW * 0));
		*(UINT32 *)(dst + (MAKETEXT_ROW * 0)) = 0;
		*(UINT32 *)(dst + (MAKETEXT_ROW * 2)) = dat;
		dst += 4;
	} while(dst < term);
}

static void atrb5(UINT8 *dst, UINT8 *term) {

	UINT32	dat;

	do {
		dat = *(UINT32 *)(dst + (MAKETEXT_ROW * 0));
		*(UINT32 *)(dst + (MAKETEXT_ROW * 2)) = dat;
		dst += 4;
	} while(dst < term);
}

static void atrb6(UINT8 *dst, UINT8 *term) {

	UINT32	dat;

	do {
		dat = *(UINT32 *)(dst + (MAKETEXT_ROW * 0));
		*(UINT32 *)(dst + (MAKETEXT_ROW * 0)) = 0;
		*(UINT32 *)(dst + (MAKETEXT_ROW * 1)) = dat;
		*(UINT32 *)(dst + (MAKETEXT_ROW * 2)) = dat;
		dst += 4;
	} while(dst < term);
}

static void atrb7(UINT8 *dst, UINT8 *term) {

	UINT32	dat;

	do {
		dat = *(UINT32 *)(dst + (MAKETEXT_ROW * 0));
		*(UINT32 *)(dst + (MAKETEXT_ROW * 1)) = dat;
		*(UINT32 *)(dst + (MAKETEXT_ROW * 2)) = dat;
		dst += 4;
	} while(dst < term);
}

static void atrb0r(UINT8 *dst, UINT8 *term) {

	do {
		*(UINT32 *)(dst + (MAKETEXT_ROW * 0)) = (UINT32)-1;
		*(UINT32 *)(dst + (MAKETEXT_ROW * 1)) = (UINT32)-1;
		*(UINT32 *)(dst + (MAKETEXT_ROW * 2)) = (UINT32)-1;
		dst += 4;
	} while(dst < term);
}

static void atrb1r(UINT8 *dst, UINT8 *term) {

	UINT32	dat;

	do {
		dat = *(UINT32 *)(dst + (MAKETEXT_ROW * 0));
		*(UINT32 *)(dst + (MAKETEXT_ROW * 0)) = ~dat;
		*(UINT32 *)(dst + (MAKETEXT_ROW * 1)) = (UINT32)-1;
		*(UINT32 *)(dst + (MAKETEXT_ROW * 2)) = (UINT32)-1;
		dst += 4;
	} while(dst < term);
}

static void atrb2r(UINT8 *dst, UINT8 *term) {

	UINT32	dat;

	do {
		dat = *(UINT32 *)(dst + (MAKETEXT_ROW * 0));
		*(UINT32 *)(dst + (MAKETEXT_ROW * 0)) = (UINT32)-1;
		*(UINT32 *)(dst + (MAKETEXT_ROW * 1)) = ~dat;
		*(UINT32 *)(dst + (MAKETEXT_ROW * 2)) = (UINT32)-1;
		dst += 4;
	} while(dst < term);
}

static void atrb3r(UINT8 *dst, UINT8 *term) {

	UINT32	dat;

	do {
		dat = *(UINT32 *)(dst + (MAKETEXT_ROW * 0));
		*(UINT32 *)(dst + (MAKETEXT_ROW * 0)) = (UINT32)-1;
		*(UINT32 *)(dst + (MAKETEXT_ROW * 1)) = dat;
		*(UINT32 *)(dst + (MAKETEXT_ROW * 2)) = (UINT32)-1;
		dst += 4;
	} while(dst < term);
}

static void atrb4r(UINT8 *dst, UINT8 *term) {

	UINT32	dat;

	do {
		dat = *(UINT32 *)(dst + (MAKETEXT_ROW * 0));
		*(UINT32 *)(dst + (MAKETEXT_ROW * 0)) = (UINT32)-1;
		*(UINT32 *)(dst + (MAKETEXT_ROW * 1)) = (UINT32)-1;
		*(UINT32 *)(dst + (MAKETEXT_ROW * 2)) = ~dat;
		dst += 4;
	} while(dst < term);
}

static void atrb5r(UINT8 *dst, UINT8 *term) {

	UINT32	dat;

	do {
		dat = *(UINT32 *)(dst + (MAKETEXT_ROW * 0));
		*(UINT32 *)(dst + (MAKETEXT_ROW * 0)) = ~dat;
		*(UINT32 *)(dst + (MAKETEXT_ROW * 1)) = (UINT32)-1;
		*(UINT32 *)(dst + (MAKETEXT_ROW * 2)) = ~dat;
		dst += 4;
	} while(dst < term);
}

static void atrb6r(UINT8 *dst, UINT8 *term) {

	UINT32	dat;

	do {
		dat = *(UINT32 *)(dst + (MAKETEXT_ROW * 0));
		*(UINT32 *)(dst + (MAKETEXT_ROW * 0)) = (UINT32)-1;
		*(UINT32 *)(dst + (MAKETEXT_ROW * 1)) = ~dat;
		*(UINT32 *)(dst + (MAKETEXT_ROW * 2)) = ~dat;
		dst += 4;
	} while(dst < term);
}

static void atrb7r(UINT8 *dst, UINT8 *term) {

	UINT32	dat;

	do {
		dat = *(UINT32 *)(dst + (MAKETEXT_ROW * 0));
		*(UINT32 *)(dst + (MAKETEXT_ROW * 0)) = ~dat;
		*(UINT32 *)(dst + (MAKETEXT_ROW * 1)) = ~dat;
		*(UINT32 *)(dst + (MAKETEXT_ROW * 2)) = ~dat;
		dst += 4;
	} while(dst < term);
}

const MAKEATRFN makeatr8[16] = {
			atrb0,	atrb1,	atrb2,	atrb3,	atrb4,	atrb5,	atrb6,	atrb7,
			atrb0r,	atrb1r,	atrb2r,	atrb3r,	atrb4r,	atrb5r,	atrb6r,	atrb7r};



// ----

void makeatr_pcg8(UINT8 *dst, UINT cnt,
								const UINT8 *src, REG8 atr, MAKETXTFN fn) {

	if (atr & 1) {
		(*fn)(dst, dst + cnt, src);
	}
	dst += MAKETEXT_ROW;
	if (atr & 2) {
		(*fn)(dst, dst + cnt, src + 0x800);
	}
	dst += MAKETEXT_ROW;
	if (atr & 4) {
		(*fn)(dst, dst + cnt, src + 0x1000);
	}
	if (atr & TRAMATR_REVERSE) {
		dst -= MAKETEXT_ROW * 2;
		cnt = (cnt + 3) >> 2;
		do {
			*(UINT32 *)(dst + (MAKETEXT_ROW * 0)) ^= (UINT32)-1;
			*(UINT32 *)(dst + (MAKETEXT_ROW * 1)) ^= (UINT32)-1;
			*(UINT32 *)(dst + (MAKETEXT_ROW * 2)) ^= (UINT32)-1;
			dst += 4;
		} while(--cnt);
	}
}

void makeatr_pcg16(UINT8 *dst, UINT cnt,
								const UINT8 *src, REG8 atr, MAKETXTFN fn) {

	if (atr & 1) {
		(*fn)(dst, dst + cnt, src);
	}
	dst += MAKETEXT_ROW;
	if (atr & 2) {
		(*fn)(dst, dst + cnt, src + 0x800);
	}
	dst += MAKETEXT_ROW;
	if (atr & 4) {
		(*fn)(dst, dst + cnt, src + 0x1000);
	}
	if (atr & TRAMATR_REVERSE) {
		dst -= MAKETEXT_ROW * 2;
		cnt = (cnt + 3) >> 2;
		do {
			*(UINT32 *)(dst + (MAKETEXT_ROW*0)) ^= (UINT32)-1;
			*(UINT32 *)(dst + (MAKETEXT_ROW*1)) ^= (UINT32)-1;
			*(UINT32 *)(dst + (MAKETEXT_ROW*2)) ^= (UINT32)-1;
			*(UINT32 *)(dst + (MAKETEXT_STEP + MAKETEXT_ROW*0)) ^= (UINT32)-1;
			*(UINT32 *)(dst + (MAKETEXT_STEP + MAKETEXT_ROW*1)) ^= (UINT32)-1;
			*(UINT32 *)(dst + (MAKETEXT_STEP + MAKETEXT_ROW*2)) ^= (UINT32)-1;
			dst += 4;
		} while(--cnt);
	}
}

