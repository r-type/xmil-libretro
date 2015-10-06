#include	"compiler.h"
#include	"scrndraw.h"
#include	"makescrn.h"
#include	"makesub.h"


// --------------------------------------------------- 16���C�� ANK �L�����N�^

													// �m�[�}��
static void txt16_nor(UINT8 *dst, UINT8 *term, const UINT8 *src) {

	UINT	pos;

	pos = 0;										// �J�E���^�͖߂�
	do {
		dst[0] = src[pos + 0];
		dst[0 + MAKETEXT_STEP] = src[pos + 1];
		pos = (pos + 2) & 15;
		dst++;
	} while(dst < term);
}

													// ���{�p��
static void txt16_x2left(UINT8 *dst, UINT8 *term, const UINT8 *src) {

	UINT	pos;

	pos = 0;										// �J�E���^�͖߂�
	do {
		dst[0] = PATx2LEFT(src[pos]);
		dst[0 + MAKETEXT_STEP] = PATx2LEFT(src[pos + 1]);
		pos = (pos + 2) & 15;
		dst++;
	} while(dst < term);
}

													// ���{�p�E
static void txt16_x2right(UINT8 *dst, UINT8 *term, const UINT8 *src) {

	UINT	pos;

	pos = 0;										// �J�E���^�͖߂�
	do {
		dst[0] = PATx2RIGHT(src[pos]);
		dst[0 + MAKETEXT_STEP] = PATx2RIGHT(src[pos + 1]);
		pos = (pos + 2) & 15;
		dst++;
	} while(dst < term);
}

													// �c�{�p��
static void txt16_Yx2(UINT8 *dst, UINT8 *term, const UINT8 *src) {

	UINT	pos;
	REG8	dat;

	pos = makescrn.fontycnt;						// ���W���[��
	do {
		dat = src[pos];
		pos = (pos + 1) & 15;
		dst[0] = (UINT8)dat;
		dst[0 + MAKETEXT_STEP] = (UINT8)dat;
		dst++;
	} while(dst < term);
}

													// �S�{�p��
static void txt16_x4left(UINT8 *dst, UINT8 *term, const UINT8 *src) {

	UINT	pos;
	REG8	dat;

	pos = makescrn.fontycnt;						// ���W���[��
	do {
		dat = PATx2LEFT(src[pos]);
		pos = (pos + 1) & 15;
		dst[0] = (UINT8)dat;
		dst[0 + MAKETEXT_STEP] = (UINT8)dat;
		dst++;
	} while(dst < term);
}

													// �S�{�p��
static void txt16_x4right(UINT8 *dst, UINT8 *term, const UINT8 *src) {

	UINT	pos;
	REG8	dat;

	pos = makescrn.fontycnt;						// ���W���[��
	do {
		dat = PATx2RIGHT(src[pos]);
		pos = (pos + 1) & 15;
		dst[0] = (UINT8)dat;
		dst[0 + MAKETEXT_STEP] = (UINT8)dat;
		dst++;
	} while(dst < term);
}

													// �E�S�h�b�g
static void txt16_right4dot(UINT8 *dst, UINT8 *term, const UINT8 *src) {

	UINT	pos;

	pos = 0;										// �J�E���^�͖߂�
	do {
		dst[0] = (UINT8)(src[pos] << 4);
		dst[0 + MAKETEXT_STEP] = (UINT8)(src[pos+1] << 4);
		pos = (pos + 2) & 15;
		dst++;
	} while(dst < term);
}

													// �ׂ�E�S�h�b�g
static void txt16_right4half(UINT8 *dst, UINT8 *term, const UINT8 *src) {

	UINT	pos;
	REG8	dat;

	pos = makescrn.fontycnt;						// ���W���[��
	if (pos) {										// �ŏ��̂P�񂾂�
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

													// �E�S�h�b�g�c�{�p��
static void txt16_right4x2(UINT8 *dst, UINT8 *term, const UINT8 *src) {

	UINT	pos;
	REG8	dat;

	pos = makescrn.fontycnt;						// ���W���[��
	do {
		dat = src[pos] << 4;
		pos = (pos + 2) & 15;
		dst[0] = (UINT)dat;
		dst[0 + MAKETEXT_STEP] = (UINT)dat;
		dst++;
	} while(dst < term);
}

													// �ׂ��ďc�{�p
static void txt16_halfx2(UINT8 *dst, UINT8 *term, const UINT8 *src) {

	UINT	pos;
	REG8	dat;

	pos = makescrn.fontycnt;						// ���W���[�� ?
	if (pos) {										// �ŏ��̂P�񂾂�
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

													// �ׂ��ĂS�{�p��
static void txt16_halfx4left(UINT8 *dst, UINT8 *term, const UINT8 *src) {

	UINT	pos;
	REG8	dat;

	pos = makescrn.fontycnt;						// ���W���[��
	if (pos) {										// �ŏ��̂P�񂾂�
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

													// �ׂ��ĂS�{�p�E
static void txt16_halfx4right(UINT8 *dst, UINT8 *term, const UINT8 *src) {

	UINT	pos;
	REG8	dat;

	pos = makescrn.fontycnt;						// ���W���[��
	if (pos) {										// �ŏ��̂P�񂾂�
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


// -------------------------------------------------- 8���C�� PCG �L�����N�^x2

													// �m�[�}��
static void pcg8x2_nor(UINT8 *dst, UINT8 *term, const UINT8 *src) {

	UINT	pos;
	REG8	dat;

	pos = 0;										// �J�E���^�͖߂�
	do {
		dat = src[pos];
		pos = (pos + 1) & 7;
		dst[0] = (UINT8)dat;
		dst[0 + MAKETEXT_STEP] = (UINT8)dat;
		dst++;
	} while(dst < term);
}

													// ���{�p��
static void pcg8x2_x2left(UINT8 *dst, UINT8 *term, const UINT8 *src) {

	UINT	pos;
	REG8	dat;

	pos = 0;										// �J�E���^�͖߂�
	do {
		dat = PATx2LEFT(src[pos]);
		pos = (pos + 1) & 7;
		dst[0] = (UINT8)dat;
		dst[0 + MAKETEXT_STEP] = (UINT8)dat;
		dst++;
	} while(dst < term);
}

													// ���{�p��
static void pcg8x2_x2right(UINT8 *dst, UINT8 *term, const UINT8 *src) {

	UINT	pos;
	REG8	dat;

	pos = 0;										// �J�E���^�͖߂�
	do {
		dat = PATx2RIGHT(src[pos]);
		pos = (pos + 1) & 7;
		dst[0] = (UINT8)dat;
		dst[0 + MAKETEXT_STEP] = (UINT8)dat;
		dst++;
	} while(dst < term);
}

													// �c�{�p��
static void pcg8x2_Yx2(UINT8 *dst, UINT8 *term, const UINT8 *src) {

	UINT	pos;
	REG8	dat;

	pos = makescrn.fontycnt >> 1;					// ���W���[��
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

													// �S�{�p��
static void pcg8x2_x4left(UINT8 *dst, UINT8 *term, const UINT8 *src) {

	UINT	pos;
	REG8	dat;

	pos = makescrn.fontycnt >> 1;					// ���W���[��
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

													// �S�{�p�E
static void pcg8x2_x4right(UINT8 *dst, UINT8 *term, const UINT8 *src) {

	UINT	pos;
	REG8	dat;

	pos = makescrn.fontycnt >> 1;					// ���W���[��
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

													// �E�S�h�b�g
static void pcg8x2_right4dot(UINT8 *dst, UINT8 *term, const UINT8 *src) {

	UINT	pos;
	REG8	dat;

	pos = 0;										// �J�E���^�͖߂�
	do {
		dat = src[pos] << 4;
		pos = (pos + 1) & 7;
		dst[0] = (UINT8)dat;
		dst[0 + MAKETEXT_STEP] = (UINT8)dat;
		dst++;
	} while(dst < term);
}

													// �ׂ�E�S�h�b�g
static void pcg8x2_right4half(UINT8 *dst, UINT8 *term, const UINT8 *src) {

	UINT	pos;
	REG8	dat;

	pos = makescrn.fontycnt >> 1;					// ���W���[��
	if (pos) {										// �ŏ��̂P�񂾂�
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

													// �E�S�h�b�g�c�{�p��
static void pcg8x2_right4x2(UINT8 *dst, UINT8 *term, const UINT8 *src) {

	UINT	pos;
	REG8	dat;

	pos = makescrn.fontycnt >> 1;					// ���W���[��
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

													// �ׂ��ďc�{�p
static void pcg8x2_halfx2(UINT8 *dst, UINT8 *term, const UINT8 *src) {

	UINT	pos;
	REG8	dat;

	pos = makescrn.fontycnt >> 1;					// ���W���[��
	if (pos) {										// �ŏ��̂P�񂾂�
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

													// �ׂ��ĂS�{�p��
static void pcg8x2_halfx4left(UINT8 *dst, UINT8 *term, const UINT8 *src) {

	UINT	pos;
	REG8	dat;

	pos = makescrn.fontycnt >> 1;					// ���W���[��
	if (pos) {										// �ŏ��̂P�񂾂�
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

													// �ׂ��ĂS�{�p�E
static void pcg8x2_halfx4right(UINT8 *dst, UINT8 *term, const UINT8 *src) {

	UINT	pos;
	REG8	dat;

	pos = makescrn.fontycnt >> 1;					// ���W���[��
	if (pos) {										// �ŏ��̂P�񂾂�
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
						txt16_nor,					// �m�[�}��
						txt16_halfx2,				// �ׂ��ďc�{�p
						txt16_right4dot,			// �E�S�h�b�g
						txt16_right4half,			// �ׂ�E�S�h�b�g

						txt16_Yx2,					// �c�{�p
						draw_nop,					// ���肦�Ȃ�
						txt16_right4x2,				// �E�S�h�b�g�c�{�p
						draw_nop,					// ���肦�Ȃ�

						txt16_x2left,				// ���{�p��
						txt16_halfx4left,			// �ׂ��ĂS�{�p��
						txt16_x2right,				// ���{�p�E
						txt16_halfx4right,			// �ׂ��ĂS�{�p�E

						txt16_x4left,				// �S�{�p��
						draw_nop,					// ���肦�Ȃ�
						txt16_x4right,				// �S�{�p�E
						draw_nop					// ���肦�Ȃ�
};

const MAKETXTFN makepcg16fn[16] = {
						pcg8x2_nor,					// �m�[�}��
						pcg8x2_halfx2,				// �ׂ��ďc�{�p
						pcg8x2_right4dot,			// �E�S�h�b�g
						pcg8x2_right4half,			// �ׂ�E�S�h�b�g

						pcg8x2_Yx2,					// �c�{�p
						draw_nop,					// ���肦�Ȃ�
						pcg8x2_right4x2,			// �E�S�h�b�g�c�{�p
						draw_nop,					// ���肦�Ȃ�

						pcg8x2_x2left,				// ���{�p��
						pcg8x2_halfx4left,			// �ׂ��ĂS�{�p��
						pcg8x2_x2right,				// ���{�p�E
						pcg8x2_halfx4right,			// �ׂ��ĂS�{�p�E

						pcg8x2_x4left,				// �S�{�p��
						draw_nop,					// ���肦�Ȃ�
						pcg8x2_x4right,				// �S�{�p�E
						draw_nop					// ���肦�Ȃ�
};

