#include	"compiler.h"
#include	"scrndraw.h"
#include	"makescrn.h"
#include	"makesub.h"


// ---------------------------------------------------- 8ライン ANK キャラクタ

													// ノーマル
static void txt8_nor(UINT8 *dst, UINT8 *term, const UINT8 *src) {

	UINT	pos;

	pos = 0;										// カウンタは戻る
	do {
		*dst = src[pos];
		pos = (pos + 1) & 7;
		dst++;
	} while(dst < term);
}

													// 横倍角左
static void txt8_x2left(UINT8 *dst, UINT8 *term, const UINT8 *src) {

	UINT	pos;

	pos = 0;										// カウンタは戻る
	do {
		*dst = PATx2LEFT(src[pos]);
		pos = (pos + 1) & 7;
		dst++;
	} while(dst < term);
}

													// 横倍角右
static void txt8_x2right(UINT8 *dst, UINT8 *term, const UINT8 *src) {

	UINT	pos;

	pos = 0;										// カウンタは戻る
	do {
		*dst = PATx2RIGHT(src[pos]);
		pos = (pos + 1) & 7;
		dst++;
	} while(dst < term);
}

													// 縦倍角上
static void txt8_Yx2(UINT8 *dst, UINT8 *term, const UINT8 *src) {

	UINT	pos;
	REG8	dat;

	pos = makescrn.fontycnt >> 1;					// レジューム
	do {
		dat = src[pos];
		pos = (pos + 1) & 7;
		dst[0] = (UINT8)dat;
		dst[1] = (UINT8)dat;
		dst += 2;
	} while(dst < term);
}

													// ４倍角左
static void txt8_x4left(UINT8 *dst, UINT8 *term, const UINT8 *src) {

	UINT	pos;
	REG8	dat;

	pos = makescrn.fontycnt >> 1;					// レジューム
	do {
		dat = PATx2LEFT(src[pos]);
		pos = (pos + 1) & 7;
		dst[0] = (UINT8)dat;
		dst[1] = (UINT8)dat;
		dst += 2;
	} while(dst < term);
}

													// ４倍角右
static void txt8_x4right(UINT8 *dst, UINT8 *term, const UINT8 *src) {

	UINT	pos;
	REG8	dat;

	pos = makescrn.fontycnt >> 1;					// レジューム
	do {
		dat = PATx2RIGHT(src[pos]);
		pos = (pos + 1) & 7;
		dst[0] = (UINT8)dat;
		dst[1] = (UINT8)dat;
		dst += 2;
	} while(dst < term);
}

													// 右４ドット
static void txt8_right4dot(UINT8 *dst, UINT8 *term, const UINT8 *src) {

	UINT	pos;

	pos = 0;										// カウンタは戻る
	do {
		*dst = (UINT8)(src[pos] << 4);
		pos = (pos + 1) & 7;
		dst++;
	} while(dst < term);
}

													// 潰れ右４ドット
static void txt8_right4half(UINT8 *dst, UINT8 *term, const UINT8 *src) {

	UINT	pos;
	REG8	dat;

	pos = makescrn.fontycnt >> 1;					// レジューム
	if (pos) {										// 最初の１回だけ
		dat = src[pos] << 4;
		pos = 0;
		dst[0] = (UINT8)dat;
		dst[1] = (UINT8)dat;
		dst += 2;
	}
	while(dst < term) {
		dat = src[pos] << 4;
		pos = (pos + 2) & 7;
		dst[0] = (UINT8)dat;
		dst[1] = (UINT8)dat;
		dst += 2;
	}
}

													// 右４ドット縦倍角上
static void txt8_right4x2(UINT8 *dst, UINT8 *term, const UINT8 *src) {

	UINT	pos;
	REG8	dat;

	pos = makescrn.fontycnt >> 1;					// レジューム
	do {
		dat = src[pos] << 4;
		pos = (pos + 1) & 7;
		dst[0] = (UINT)dat;
		dst[1] = (UINT)dat;
		dst += 2;
	} while(dst < term);
}

													// 潰して縦倍角
static void txt8_halfx2(UINT8 *dst, UINT8 *term, const UINT8 *src) {

	UINT	pos;
	REG8	dat;

	pos = makescrn.fontycnt >> 1;					// レジューム
	if (pos) {										// 最初の１回だけ
		dat = src[pos];
		pos = 0;
		dst[0] = (UINT8)dat;
		dst[1] = (UINT8)dat;
		dst += 2;
	}
	while(dst < term) {
		dat = src[pos];
		pos = (pos + 2) & 7;
		dst[0] = (UINT8)dat;
		dst[1] = (UINT8)dat;
		dst += 2;
	}
}

													// 潰して４倍角左
static void txt8_halfx4left(UINT8 *dst, UINT8 *term, const UINT8 *src) {

	UINT	pos;
	REG8	dat;

	pos = makescrn.fontycnt >> 1;					// レジューム
	if (pos) {										// 最初の１回だけ
		dat = PATx2LEFT(src[pos]);
		pos = 0;
		dst[0] = (UINT8)dat;
		dst[1] = (UINT8)dat;
		dst += 2;
	}
	while(dst < term) {
		dat = PATx2LEFT(src[pos]);
		pos = (pos + 2) & 7;
		dst[0] = (UINT8)dat;
		dst[1] = (UINT8)dat;
		dst += 2;
	}
}

													// 潰して４倍角右
static void txt8_halfx4right(UINT8 *dst, UINT8 *term, const UINT8 *src) {

	UINT	pos;
	REG8	dat;

	pos = makescrn.fontycnt >> 1;					// レジューム
	if (pos) {										// 最初の１回だけ
		dat = PATx2RIGHT(src[pos]);
		pos = 0;
		dst[0] = (UINT8)dat;
		dst[1] = (UINT8)dat;
		dst += 2;
	}
	while(dst < term) {
		dat = PATx2RIGHT(src[pos]);
		pos = (pos + 2) & 7;
		dst[0] = (UINT8)dat;
		dst[1] = (UINT8)dat;
		dst += 2;
	}
}


// ---------------------------------------------------- 8ライン 漢字キャラクタ

													// ノーマル
static void knj8_nor(UINT8 *dst, UINT8 *term, const UINT8 *src) {

	UINT	pos;

	pos = 0;										// カウンタは戻る
	do {
		*dst = src[pos];
		pos = (pos + 2) & 15;
		dst++;
	} while(dst < term);
}

													// 横倍角左
static void knj8_x2left(UINT8 *dst, UINT8 *term, const UINT8 *src) {

	UINT	pos;

	pos = 0;										// カウンタは戻る
	do {
		*dst = PATx2LEFT(src[pos]);
		pos = (pos + 2) & 15;
		dst++;
	} while(dst < term);
}

													// 横倍角右
static void knj8_x2right(UINT8 *dst, UINT8 *term, const UINT8 *src) {

	UINT	pos;

	pos = 0;										// カウンタは戻る
	do {
		*dst = PATx2RIGHT(src[pos]);
		pos = (pos + 2) & 15;
		dst++;
	} while(dst < term);
}

													// 縦倍角
static void knj8_Yx2(UINT8 *dst, UINT8 *term, const UINT8 *src) {

	UINT	pos;
	REG8	dat;

	pos = makescrn.fontycnt;						// レジューム
	do {
		dat = src[pos];
		pos = (pos + 1) & 15;
		*dst = (UINT8)dat;
		dst++;
	} while(dst < term);
}

													// ４倍角左
static void knj8_x4left(UINT8 *dst, UINT8 *term, const UINT8 *src) {

	UINT	pos;
	REG8	dat;

	pos = makescrn.fontycnt;						// レジューム
	do {
		dat = PATx2LEFT(src[pos]);
		pos = (pos + 1) & 15;
		*dst = (UINT8)dat;
		dst++;
	} while(dst < term);
}

													// ４倍角右
static void knj8_x4right(UINT8 *dst, UINT8 *term, const UINT8 *src) {

	UINT	pos;
	REG8	dat;

	pos = makescrn.fontycnt;						// レジューム
	do {
		dat = PATx2RIGHT(src[pos]);
		pos = (pos + 1) & 15;
		*dst = (UINT8)dat;
		dst++;
	} while(dst < term);
}

													// 右４ドット
static void knj8_right4dot(UINT8 *dst, UINT8 *term, const UINT8 *src) {

	UINT	pos;

	pos = 0;										// カウンタは戻る
	do {
		*dst = (UINT8)(src[pos] << 4);
		pos = (pos + 2) & 15;
		dst++;
	} while(dst < term);
}

													// 潰れ右４ドット
static void knj8_right4half(UINT8 *dst, UINT8 *term, const UINT8 *src) {

	UINT	pos;
	REG8	dat;

	pos = makescrn.fontycnt;						// レジューム
	if (pos) {										// 最初の１回だけ
		dat = src[pos] << 4;
		pos = 0;
		dst[0] = (UINT8)dat;
		dst[1] = (UINT8)dat;
		dst += 2;
	}
	while(dst < term) {
		dat = src[pos] << 4;
		pos = (pos + 4) & 15;
		dst[0] = (UINT8)dat;
		dst[1] = (UINT8)dat;
		dst += 2;
	}
}

													// 右４ドット縦倍角
static void knj8_right4x2(UINT8 *dst, UINT8 *term, const UINT8 *src) {

	UINT	pos;
	REG8	dat;

	pos = makescrn.fontycnt;						// レジューム
	do {
		dat = src[pos] << 4;
		pos = (pos + 1) & 15;
		*dst = (UINT)dat;
		dst++;
	} while(dst < term);
}

													// 潰して縦倍角
static void knj8_halfx2(UINT8 *dst, UINT8 *term, const UINT8 *src) {

	UINT	pos;
	REG8	dat;

	pos = makescrn.fontycnt;						// レジューム
	if (pos) {										// 最初の１回だけ
		dat = src[pos];
		pos = 0;
		dst[0] = (UINT8)dat;
		dst[1] = (UINT8)dat;
		dst += 2;
	}
	while(dst < term) {
		dat = src[pos];
		pos = (pos + 4) & 15;
		dst[0] = (UINT8)dat;
		dst[1] = (UINT8)dat;
		dst += 2;
	}
}

													// 潰して４倍角左
static void knj8_halfx4left(UINT8 *dst, UINT8 *term, const UINT8 *src) {

	UINT	pos;
	REG8	dat;

	pos = makescrn.fontycnt;						// レジューム
	if (pos) {										// 最初の１回だけ
		dat = PATx2LEFT(src[pos]);
		pos = 0;
		dst[0] = (UINT8)dat;
		dst[1] = (UINT8)dat;
		dst += 2;
	}
	while(dst < term) {
		dat = PATx2LEFT(src[pos]);
		pos = (pos + 4) & 15;
		dst[0] = (UINT8)dat;
		dst[1] = (UINT8)dat;
		dst += 2;
	}
}

													// 潰して４倍角右
static void knj8_halfx4right(UINT8 *dst, UINT8 *term, const UINT8 *src) {

	UINT	pos;
	REG8	dat;

	pos = makescrn.fontycnt;						// レジューム ?
	if (pos) {										// 最初の１回だけ
		dat = PATx2RIGHT(src[pos]);
		pos = 0;
		dst[0] = (UINT8)dat;
		dst[1] = (UINT8)dat;
		dst += 2;
	}
	while(dst < term) {
		dat = PATx2RIGHT(src[pos]);
		pos = (pos + 4) & 15;
		dst[0] = (UINT8)dat;
		dst[1] = (UINT8)dat;
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

const MAKETXTFN maketxt8fn[16] = {
						txt8_nor,					// ノーマル
						txt8_halfx2,				// 潰して縦倍角
						txt8_right4dot,				// 右４ドット
						txt8_right4half,			// 潰れ右４ドット

						txt8_Yx2,					// 縦倍角
						draw_nop,					// ありえない
						txt8_right4x2,				// 右４ドット縦倍角
						draw_nop,					// ありえない

						txt8_x2left,				// 横倍角左
						txt8_halfx4left,			// 潰して４倍角左
						txt8_x2right,				// 横倍角右
						txt8_halfx4right,			// 潰して４倍角右

						txt8_x4left,				// ４倍角左
						draw_nop,					// ありえない
						txt8_x4right,				// ４倍角右
						draw_nop					// ありえない
};


const MAKETXTFN makeknj8fn[16] = {
						knj8_nor,					// ノーマル
						knj8_halfx2,				// 潰して縦倍角
						knj8_right4dot,				// 右４ドット
						knj8_right4half,			// 潰れ右４ドット

						knj8_Yx2,					// 縦倍角
						draw_nop,					// 
						knj8_right4x2,				// 右４ドット縦倍角
						draw_nop,					//

						knj8_x2left,				// 横倍角左
						knj8_halfx4left,			// 潰して４倍角左
						knj8_x2right,				// 横倍角右
						knj8_halfx4right,			// 潰して４倍角右

						knj8_x4left,				// ４倍角左
						draw_nop,					//
						knj8_x4right,				// ４倍角右
						draw_nop					//
};

