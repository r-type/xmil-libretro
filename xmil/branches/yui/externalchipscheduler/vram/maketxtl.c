#include	"compiler.h"
#include	"scrndraw.h"
#include	"makescrn.h"
#include	"makesub.h"


// ---------------------------------------------------- 8���C�� ANK �L�����N�^

													// �m�[�}��
static void txt8_nor(UINT8 *dst, UINT8 *term, const UINT8 *src) {

	UINT	pos;

	pos = 0;										// �J�E���^�͖߂�
	do {
		*dst = src[pos];
		pos = (pos + 1) & 7;
		dst++;
	} while(dst < term);
}

													// ���{�p��
static void txt8_x2left(UINT8 *dst, UINT8 *term, const UINT8 *src) {

	UINT	pos;

	pos = 0;										// �J�E���^�͖߂�
	do {
		*dst = PATx2LEFT(src[pos]);
		pos = (pos + 1) & 7;
		dst++;
	} while(dst < term);
}

													// ���{�p�E
static void txt8_x2right(UINT8 *dst, UINT8 *term, const UINT8 *src) {

	UINT	pos;

	pos = 0;										// �J�E���^�͖߂�
	do {
		*dst = PATx2RIGHT(src[pos]);
		pos = (pos + 1) & 7;
		dst++;
	} while(dst < term);
}

													// �c�{�p��
static void txt8_Yx2(UINT8 *dst, UINT8 *term, const UINT8 *src) {

	UINT	pos;
	REG8	dat;

	pos = makescrn.fontycnt >> 1;					// ���W���[��
	do {
		dat = src[pos];
		pos = (pos + 1) & 7;
		dst[0] = (UINT8)dat;
		dst[1] = (UINT8)dat;
		dst += 2;
	} while(dst < term);
}

													// �S�{�p��
static void txt8_x4left(UINT8 *dst, UINT8 *term, const UINT8 *src) {

	UINT	pos;
	REG8	dat;

	pos = makescrn.fontycnt >> 1;					// ���W���[��
	do {
		dat = PATx2LEFT(src[pos]);
		pos = (pos + 1) & 7;
		dst[0] = (UINT8)dat;
		dst[1] = (UINT8)dat;
		dst += 2;
	} while(dst < term);
}

													// �S�{�p�E
static void txt8_x4right(UINT8 *dst, UINT8 *term, const UINT8 *src) {

	UINT	pos;
	REG8	dat;

	pos = makescrn.fontycnt >> 1;					// ���W���[��
	do {
		dat = PATx2RIGHT(src[pos]);
		pos = (pos + 1) & 7;
		dst[0] = (UINT8)dat;
		dst[1] = (UINT8)dat;
		dst += 2;
	} while(dst < term);
}

													// �E�S�h�b�g
static void txt8_right4dot(UINT8 *dst, UINT8 *term, const UINT8 *src) {

	UINT	pos;

	pos = 0;										// �J�E���^�͖߂�
	do {
		*dst = (UINT8)(src[pos] << 4);
		pos = (pos + 1) & 7;
		dst++;
	} while(dst < term);
}

													// �ׂ�E�S�h�b�g
static void txt8_right4half(UINT8 *dst, UINT8 *term, const UINT8 *src) {

	UINT	pos;
	REG8	dat;

	pos = makescrn.fontycnt >> 1;					// ���W���[��
	if (pos) {										// �ŏ��̂P�񂾂�
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

													// �E�S�h�b�g�c�{�p��
static void txt8_right4x2(UINT8 *dst, UINT8 *term, const UINT8 *src) {

	UINT	pos;
	REG8	dat;

	pos = makescrn.fontycnt >> 1;					// ���W���[��
	do {
		dat = src[pos] << 4;
		pos = (pos + 1) & 7;
		dst[0] = (UINT)dat;
		dst[1] = (UINT)dat;
		dst += 2;
	} while(dst < term);
}

													// �ׂ��ďc�{�p
static void txt8_halfx2(UINT8 *dst, UINT8 *term, const UINT8 *src) {

	UINT	pos;
	REG8	dat;

	pos = makescrn.fontycnt >> 1;					// ���W���[��
	if (pos) {										// �ŏ��̂P�񂾂�
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

													// �ׂ��ĂS�{�p��
static void txt8_halfx4left(UINT8 *dst, UINT8 *term, const UINT8 *src) {

	UINT	pos;
	REG8	dat;

	pos = makescrn.fontycnt >> 1;					// ���W���[��
	if (pos) {										// �ŏ��̂P�񂾂�
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

													// �ׂ��ĂS�{�p�E
static void txt8_halfx4right(UINT8 *dst, UINT8 *term, const UINT8 *src) {

	UINT	pos;
	REG8	dat;

	pos = makescrn.fontycnt >> 1;					// ���W���[��
	if (pos) {										// �ŏ��̂P�񂾂�
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


// ---------------------------------------------------- 8���C�� �����L�����N�^

													// �m�[�}��
static void knj8_nor(UINT8 *dst, UINT8 *term, const UINT8 *src) {

	UINT	pos;

	pos = 0;										// �J�E���^�͖߂�
	do {
		*dst = src[pos];
		pos = (pos + 2) & 15;
		dst++;
	} while(dst < term);
}

													// ���{�p��
static void knj8_x2left(UINT8 *dst, UINT8 *term, const UINT8 *src) {

	UINT	pos;

	pos = 0;										// �J�E���^�͖߂�
	do {
		*dst = PATx2LEFT(src[pos]);
		pos = (pos + 2) & 15;
		dst++;
	} while(dst < term);
}

													// ���{�p�E
static void knj8_x2right(UINT8 *dst, UINT8 *term, const UINT8 *src) {

	UINT	pos;

	pos = 0;										// �J�E���^�͖߂�
	do {
		*dst = PATx2RIGHT(src[pos]);
		pos = (pos + 2) & 15;
		dst++;
	} while(dst < term);
}

													// �c�{�p
static void knj8_Yx2(UINT8 *dst, UINT8 *term, const UINT8 *src) {

	UINT	pos;
	REG8	dat;

	pos = makescrn.fontycnt;						// ���W���[��
	do {
		dat = src[pos];
		pos = (pos + 1) & 15;
		*dst = (UINT8)dat;
		dst++;
	} while(dst < term);
}

													// �S�{�p��
static void knj8_x4left(UINT8 *dst, UINT8 *term, const UINT8 *src) {

	UINT	pos;
	REG8	dat;

	pos = makescrn.fontycnt;						// ���W���[��
	do {
		dat = PATx2LEFT(src[pos]);
		pos = (pos + 1) & 15;
		*dst = (UINT8)dat;
		dst++;
	} while(dst < term);
}

													// �S�{�p�E
static void knj8_x4right(UINT8 *dst, UINT8 *term, const UINT8 *src) {

	UINT	pos;
	REG8	dat;

	pos = makescrn.fontycnt;						// ���W���[��
	do {
		dat = PATx2RIGHT(src[pos]);
		pos = (pos + 1) & 15;
		*dst = (UINT8)dat;
		dst++;
	} while(dst < term);
}

													// �E�S�h�b�g
static void knj8_right4dot(UINT8 *dst, UINT8 *term, const UINT8 *src) {

	UINT	pos;

	pos = 0;										// �J�E���^�͖߂�
	do {
		*dst = (UINT8)(src[pos] << 4);
		pos = (pos + 2) & 15;
		dst++;
	} while(dst < term);
}

													// �ׂ�E�S�h�b�g
static void knj8_right4half(UINT8 *dst, UINT8 *term, const UINT8 *src) {

	UINT	pos;
	REG8	dat;

	pos = makescrn.fontycnt;						// ���W���[��
	if (pos) {										// �ŏ��̂P�񂾂�
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

													// �E�S�h�b�g�c�{�p
static void knj8_right4x2(UINT8 *dst, UINT8 *term, const UINT8 *src) {

	UINT	pos;
	REG8	dat;

	pos = makescrn.fontycnt;						// ���W���[��
	do {
		dat = src[pos] << 4;
		pos = (pos + 1) & 15;
		*dst = (UINT)dat;
		dst++;
	} while(dst < term);
}

													// �ׂ��ďc�{�p
static void knj8_halfx2(UINT8 *dst, UINT8 *term, const UINT8 *src) {

	UINT	pos;
	REG8	dat;

	pos = makescrn.fontycnt;						// ���W���[��
	if (pos) {										// �ŏ��̂P�񂾂�
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

													// �ׂ��ĂS�{�p��
static void knj8_halfx4left(UINT8 *dst, UINT8 *term, const UINT8 *src) {

	UINT	pos;
	REG8	dat;

	pos = makescrn.fontycnt;						// ���W���[��
	if (pos) {										// �ŏ��̂P�񂾂�
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

													// �ׂ��ĂS�{�p�E
static void knj8_halfx4right(UINT8 *dst, UINT8 *term, const UINT8 *src) {

	UINT	pos;
	REG8	dat;

	pos = makescrn.fontycnt;						// ���W���[�� ?
	if (pos) {										// �ŏ��̂P�񂾂�
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
						txt8_nor,					// �m�[�}��
						txt8_halfx2,				// �ׂ��ďc�{�p
						txt8_right4dot,				// �E�S�h�b�g
						txt8_right4half,			// �ׂ�E�S�h�b�g

						txt8_Yx2,					// �c�{�p
						draw_nop,					// ���肦�Ȃ�
						txt8_right4x2,				// �E�S�h�b�g�c�{�p
						draw_nop,					// ���肦�Ȃ�

						txt8_x2left,				// ���{�p��
						txt8_halfx4left,			// �ׂ��ĂS�{�p��
						txt8_x2right,				// ���{�p�E
						txt8_halfx4right,			// �ׂ��ĂS�{�p�E

						txt8_x4left,				// �S�{�p��
						draw_nop,					// ���肦�Ȃ�
						txt8_x4right,				// �S�{�p�E
						draw_nop					// ���肦�Ȃ�
};


const MAKETXTFN makeknj8fn[16] = {
						knj8_nor,					// �m�[�}��
						knj8_halfx2,				// �ׂ��ďc�{�p
						knj8_right4dot,				// �E�S�h�b�g
						knj8_right4half,			// �ׂ�E�S�h�b�g

						knj8_Yx2,					// �c�{�p
						draw_nop,					// 
						knj8_right4x2,				// �E�S�h�b�g�c�{�p
						draw_nop,					//

						knj8_x2left,				// ���{�p��
						knj8_halfx4left,			// �ׂ��ĂS�{�p��
						knj8_x2right,				// ���{�p�E
						knj8_halfx4right,			// �ׂ��ĂS�{�p�E

						knj8_x4left,				// �S�{�p��
						draw_nop,					//
						knj8_x4right,				// �S�{�p�E
						draw_nop					//
};

