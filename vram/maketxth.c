#include	"compiler.h"
#include	"scrndraw.h"
#include	"makescrn.h"
#include	"makesub.h"


// --------------------------------------------------- 16ライン ANK キャラクタ

													// ノーマル
static void txt16_nor(UINT8 *dst, UINT8 *term, const UINT8 *src) {

	UINT	pos;

	pos = 0;										// カウンタは戻る
	do {
		dst[0] = src[pos + 0];
		dst[0 + MAKETEXT_STEP] = src[pos + 1];
		pos = (pos + 2) & 15;
		dst++;
	} while(dst < term);
}

													// 横倍角左
static void txt16_x2left(UINT8 *dst, UINT8 *term, const UINT8 *src) {

	UINT	pos;

	pos = 0;										// カウンタは戻る
	do {
		dst[0] = PATx2LEFT(src[pos]);
		dst[0 + MAKETEXT_STEP] = PATx2LEFT(src[pos + 1]);
		pos = (pos + 2) & 15;
		dst++;
	} while(dst < term);
}

													// 横倍角右
static void txt16_x2right(UINT8 *dst, UINT8 *term, const UINT8 *src) {

	UINT	pos;

	pos = 0;										// カウンタは戻る
	do {
		dst[0] = PATx2RIGHT(src[pos]);
		dst[0 + MAKETEXT_STEP] = PATx2RIGHT(src[pos + 1]);
		pos = (pos + 2) & 15;
		dst++;
	} while(dst < term);
}

													// 縦倍角上
static void txt16_Yx2(UINT8 *dst, UINT8 *term, const UINT8 *src) {

	UINT	pos;
	REG8	dat;

	pos = makescrn.fontycnt;						// レジューム
	do {
		dat = src[pos];
		pos = (pos + 1) & 15;
		dst[0] = (UINT8)dat;
		dst[0 + MAKETEXT_STEP] = (UINT8)dat;
		dst++;
	} while(dst < term);
}

													// ４倍角左
static void txt16_x4left(UINT8 *dst, UINT8 *term, const UINT8 *src) {

	UINT	pos;
	REG8	dat;

	pos = makescrn.fontycnt;						// レジューム
	do {
		dat = PATx2LEFT(src[pos]);
		pos = (pos + 1) & 15;
		dst[0] = (UINT8)dat;
		dst[0 + MAKETEXT_STEP] = (UINT8)dat;
		dst++;
	} while(dst < term);
}

													// ４倍角左
static void txt16_x4right(UINT8 *dst, UINT8 *term, const UINT8 *src) {

	UINT	pos;
	REG8	dat;

	pos = makescrn.fontycnt;						// レジューム
	do {
		dat = PATx2RIGHT(src[pos]);
		pos = (pos + 1) & 15;
		dst[0] = (UINT8)dat;
		dst[0 + MAKETEXT_STEP] = (UINT8)dat;
		dst++;
	} while(dst < term);
}

													// 右４ドット
static void txt16_right4dot(UINT8 *dst, UINT8 *term, const UINT8 *src) {

	UINT	pos;

	pos = 0;										// カウンタは戻る
	do {
		dst[0] = (UINT8)(src[pos] << 4);
		dst[0 + MAKETEXT_STEP] = (UINT8)(src[pos+1] << 4);
		pos = (pos + 2) & 15;
		dst++;
	} while(dst < term);
}

													// 潰れ右４ドット
static void txt16_right4half(UINT8 *dst, UINT8 *term, const UINT8 *src) {

	UINT	pos;
	REG8	dat;

	pos = makescrn.fontycnt;						// レジューム
	if (pos) {										// 最初の１回だけ
		dat = src[pos] << 4;
		pos = 0;
		dst[0] = (UINT8)dat;
		dst[0 + MAKETEXT_STEP] = (UINT8)dat;
		dst++;
	}
	while(dst < term) {
		dat = src[pos] << 4;
		pos = (pos + 2) & 15;
		dst[0] = (UINT8)dat;
		dst[0 + MAKETEXT_STEP] = (UINT8)dat;
		dst++;
	}
}

													// 右４ドット縦倍角上
static void txt16_right4x2(UINT8 *dst, UINT8 *term, const UINT8 *src) {

	UINT	pos;
	REG8	dat;

	pos = makescrn.fontycnt;						// レジューム
	do {
		dat = src[pos] << 4;
		pos = (pos + 2) & 15;
		dst[0] = (UINT)dat;
		dst[0 + MAKETEXT_STEP] = (UINT)dat;
		dst++;
	} while(dst < term);
}

													// 潰して縦倍角
static void txt16_halfx2(UINT8 *dst, UINT8 *term, const UINT8 *src) {

	UINT	pos;
	REG8	dat;

	pos = makescrn.fontycnt;						// レジューム ?
	if (pos) {										// 最初の１回だけ
		dat = src[pos];
		pos = 0;
		dst[0] = (UINT8)dat;
		dst[0 + MAKETEXT_STEP] = (UINT8)dat;
		dst++;
	}
	while(dst < term) {
		dat = src[pos];
		pos = (pos + 2) & 15;
		dst[0] = (UINT8)dat;
		dst[0 + MAKETEXT_STEP] = (UINT8)dat;
		dst++;
	}
}

													// 潰して４倍角左
static void txt16_halfx4left(UINT8 *dst, UINT8 *term, const UINT8 *src) {

	UINT	pos;
	REG8	dat;

	pos = makescrn.fontycnt;						// レジューム
	if (pos) {										// 最初の１回だけ
		dat = PATx2LEFT(src[pos]);
		pos = 0;
		dst[0] = (UINT8)dat;
		dst[0 + MAKETEXT_STEP] = (UINT8)dat;
		dst++;
	}
	while(dst < term) {
		dat = PATx2LEFT(src[pos]);
		pos = (pos + 2) & 15;
		dst[0] = (UINT8)dat;
		dst[0 + MAKETEXT_STEP] = (UINT8)dat;
		dst++;
	}
}

													// 潰して４倍角右
static void txt16_halfx4right(UINT8 *dst, UINT8 *term, const UINT8 *src) {

	UINT	pos;
	REG8	dat;

	pos = makescrn.fontycnt;						// レジューム
	if (pos) {										// 最初の１回だけ
		dat = PATx2RIGHT(src[pos]);
		pos = 0;
		dst[0] = (UINT8)dat;
		dst[0 + MAKETEXT_STEP] = (UINT8)dat;
		dst++;
	}
	while(dst < term) {
		dat = PATx2RIGHT(src[pos]);
		pos = (pos + 2) & 15;
		dst[0] = (UINT8)dat;
		dst[0 + MAKETEXT_STEP] = (UINT8)dat;
		dst++;
	}
}


// -------------------------------------------------- 8ライン PCG キャラクタx2

													// ノーマル
static void pcg8x2_nor(UINT8 *dst, UINT8 *term, const UINT8 *src) {

	UINT	pos;
	REG8	dat;

	pos = 0;										// カウンタは戻る
	do {
		dat = src[pos];
		pos = (pos + 1) & 7;
		dst[0] = (UINT8)dat;
		dst[0 + MAKETEXT_STEP] = (UINT8)dat;
		dst++;
	} while(dst < term);
}

													// 横倍角左
static void pcg8x2_x2left(UINT8 *dst, UINT8 *term, const UINT8 *src) {

	UINT	pos;
	REG8	dat;

	pos = 0;										// カウンタは戻る
	do {
		dat = PATx2LEFT(src[pos]);
		pos = (pos + 1) & 7;
		dst[0] = (UINT8)dat;
		dst[0 + MAKETEXT_STEP] = (UINT8)dat;
		dst++;
	} while(dst < term);
}

													// 横倍角左
static void pcg8x2_x2right(UINT8 *dst, UINT8 *term, const UINT8 *src) {

	UINT	pos;
	REG8	dat;

	pos = 0;										// カウンタは戻る
	do {
		dat = PATx2RIGHT(src[pos]);
		pos = (pos + 1) & 7;
		dst[0] = (UINT8)dat;
		dst[0 + MAKETEXT_STEP] = (UINT8)dat;
		dst++;
	} while(dst < term);
}

													// 縦倍角上
static void pcg8x2_Yx2(UINT8 *dst, UINT8 *term, const UINT8 *src) {

	UINT	pos;
	REG8	dat;

	pos = makescrn.fontycnt >> 1;					// レジューム
	do {
		dat = src[pos];
		pos = (pos + 1) & 7;
		dst[0] = (UINT8)dat;
		dst[1] = (UINT8)dat;
		dst[0 + MAKETEXT_STEP] = (UINT8)dat;
		dst[1 + MAKETEXT_STEP] = (UINT8)dat;
		dst += 2;
	} while(dst < term);
}

													// ４倍角左
static void pcg8x2_x4left(UINT8 *dst, UINT8 *term, const UINT8 *src) {

	UINT	pos;
	REG8	dat;

	pos = makescrn.fontycnt >> 1;					// レジューム
	do {
		dat = PATx2LEFT(src[pos]);
		pos = (pos + 1) & 7;
		dst[0] = (UINT8)dat;
		dst[1] = (UINT8)dat;
		dst[0 + MAKETEXT_STEP] = (UINT8)dat;
		dst[1 + MAKETEXT_STEP] = (UINT8)dat;
		dst += 2;
	} while(dst < term);
}

													// ４倍角右
static void pcg8x2_x4right(UINT8 *dst, UINT8 *term, const UINT8 *src) {

	UINT	pos;
	REG8	dat;

	pos = makescrn.fontycnt >> 1;					// レジューム
	do {
		dat = PATx2RIGHT(src[pos]);
		pos = (pos + 1) & 7;
		dst[0] = (UINT8)dat;
		dst[1] = (UINT8)dat;
		dst[0 + MAKETEXT_STEP] = (UINT8)dat;
		dst[1 + MAKETEXT_STEP] = (UINT8)dat;
		dst += 2;
	} while(dst < term);
}

													// 右４ドット
static void pcg8x2_right4dot(UINT8 *dst, UINT8 *term, const UINT8 *src) {

	UINT	pos;
	REG8	dat;

	pos = 0;										// カウンタは戻る
	do {
		dat = src[pos] << 4;
		pos = (pos + 1) & 7;
		dst[0] = (UINT8)dat;
		dst[0 + MAKETEXT_STEP] = (UINT8)dat;
		dst++;
	} while(dst < term);
}

													// 潰れ右４ドット
static void pcg8x2_right4half(UINT8 *dst, UINT8 *term, const UINT8 *src) {

	UINT	pos;
	REG8	dat;

	pos = makescrn.fontycnt >> 1;					// レジューム
	if (pos) {										// 最初の１回だけ
		dat = src[pos] << 4;
		pos = 0;
		dst[0] = (UINT8)dat;
		dst[1] = (UINT8)dat;
		dst[0 + MAKETEXT_STEP] = (UINT8)dat;
		dst[1 + MAKETEXT_STEP] = (UINT8)dat;
		dst += 2;
	}
	while(dst < term) {
		dat = src[pos] << 4;
		pos = (pos + 2) & 7;
		dst[0] = (UINT8)dat;
		dst[1] = (UINT8)dat;
		dst[0 + MAKETEXT_STEP] = (UINT8)dat;
		dst[1 + MAKETEXT_STEP] = (UINT8)dat;
		dst += 2;
	}
}

													// 右４ドット縦倍角上
static void pcg8x2_right4x2(UINT8 *dst, UINT8 *term, const UINT8 *src) {

	UINT	pos;
	REG8	dat;

	pos = makescrn.fontycnt >> 1;					// レジューム
	do {
		dat = src[pos] << 4;
		pos = (pos + 1) & 7;
		dst[0] = (UINT)dat;
		dst[1] = (UINT)dat;
		dst[0 + MAKETEXT_STEP] = (UINT)dat;
		dst[1 + MAKETEXT_STEP] = (UINT)dat;
		dst += 2;
	} while(dst < term);
}

													// 潰して縦倍角
static void pcg8x2_halfx2(UINT8 *dst, UINT8 *term, const UINT8 *src) {

	UINT	pos;
	REG8	dat;

	pos = makescrn.fontycnt >> 1;					// レジューム
	if (pos) {										// 最初の１回だけ
		dat = src[pos];
		pos = 0;
		dst[0] = (UINT8)dat;
		dst[1] = (UINT8)dat;
		dst[0 + MAKETEXT_STEP] = (UINT8)dat;
		dst[1 + MAKETEXT_STEP] = (UINT8)dat;
		dst += 2;
	}
	while(dst < term) {
		dat = src[pos];
		pos = (pos + 2) & 7;
		dst[0] = (UINT8)dat;
		dst[1] = (UINT8)dat;
		dst[0 + MAKETEXT_STEP] = (UINT8)dat;
		dst[1 + MAKETEXT_STEP] = (UINT8)dat;
		dst += 2;
	}
}

													// 潰して４倍角左
static void pcg8x2_halfx4left(UINT8 *dst, UINT8 *term, const UINT8 *src) {

	UINT	pos;
	REG8	dat;

	pos = makescrn.fontycnt >> 1;					// レジューム
	if (pos) {										// 最初の１回だけ
		dat = PATx2LEFT(src[pos]);
		pos = 0;
		dst[0] = (UINT8)dat;
		dst[1] = (UINT8)dat;
		dst[0 + MAKETEXT_STEP] = (UINT8)dat;
		dst[1 + MAKETEXT_STEP] = (UINT8)dat;
		dst += 2;
	}
	while(dst < term) {
		dat = PATx2LEFT(src[pos]);
		pos = (pos + 2) & 7;
		dst[0] = (UINT8)dat;
		dst[1] = (UINT8)dat;
		dst[0 + MAKETEXT_STEP] = (UINT8)dat;
		dst[1 + MAKETEXT_STEP] = (UINT8)dat;
		dst += 2;
	}
}

													// 潰して４倍角右
static void pcg8x2_halfx4right(UINT8 *dst, UINT8 *term, const UINT8 *src) {

	UINT	pos;
	REG8	dat;

	pos = makescrn.fontycnt >> 1;					// レジューム
	if (pos) {										// 最初の１回だけ
		dat = PATx2RIGHT(src[pos]);
		pos = 0;
		dst[0] = (UINT8)dat;
		dst[1] = (UINT8)dat;
		dst[0 + MAKETEXT_STEP] = (UINT8)dat;
		dst[1 + MAKETEXT_STEP] = (UINT8)dat;
		dst += 2;
	}
	while(dst < term) {
		dat = PATx2RIGHT(src[pos]);
		pos = (pos + 2) & 7;
		dst[0] = (UINT8)dat;
		dst[1] = (UINT8)dat;
		dst[0 + MAKETEXT_STEP] = (UINT8)dat;
		dst[1 + MAKETEXT_STEP] = (UINT8)dat;
		dst += 2;
	}
}


// ----

static void draw_nop(UINT8 *dst, UINT8 *term, const UINT8 *src) {

	(void)dst;
	(void)term;
	(void)src;
}


// --------------------------------------------------------------------------

const MAKETXTFN maketxt16fn[16] = {
						txt16_nor,					// ノーマル
						txt16_halfx2,				// 潰して縦倍角
						txt16_right4dot,			// 右４ドット
						txt16_right4half,			// 潰れ右４ドット

						txt16_Yx2,					// 縦倍角
						draw_nop,					// ありえない
						txt16_right4x2,				// 右４ドット縦倍角
						draw_nop,					// ありえない

						txt16_x2left,				// 横倍角左
						txt16_halfx4left,			// 潰して４倍角左
						txt16_x2right,				// 横倍角右
						txt16_halfx4right,			// 潰して４倍角右

						txt16_x4left,				// ４倍角左
						draw_nop,					// ありえない
						txt16_x4right,				// ４倍角右
						draw_nop					// ありえない
};

const MAKETXTFN makepcg16fn[16] = {
						pcg8x2_nor,					// ノーマル
						pcg8x2_halfx2,				// 潰して縦倍角
						pcg8x2_right4dot,			// 右４ドット
						pcg8x2_right4half,			// 潰れ右４ドット

						pcg8x2_Yx2,					// 縦倍角
						draw_nop,					// ありえない
						pcg8x2_right4x2,			// 右４ドット縦倍角
						draw_nop,					// ありえない

						pcg8x2_x2left,				// 横倍角左
						pcg8x2_halfx4left,			// 潰して４倍角左
						pcg8x2_x2right,				// 横倍角右
						pcg8x2_halfx4right,			// 潰して４倍角右

						pcg8x2_x4left,				// ４倍角左
						draw_nop,					// ありえない
						pcg8x2_x4right,				// ４倍角右
						draw_nop					// ありえない
};

