#include	"compiler.h"
#include	"vram.h"
#include	"scrndraw.h"
#include	"makesub.h"


// 遅いのが嫌なら後でインラインにすればいい
void makemix_txt(UINT8 *dst, UINT align, const UINT8 *txt, UINT count) {

	REG8	dat;
	UINT32	datl;
	UINT32	datr;

	do {
		dat = txt[MAKETEXT_ROW * 0];
		datl = TO256COLL(dat, 3);
		datr = TO256COLR(dat, 3);
		dat = txt[MAKETEXT_ROW * 1];
		datl |= TO256COLL(dat, 4);
		datr |= TO256COLR(dat, 4);
		dat = txt[MAKETEXT_ROW * 2];
		datl |= TO256COLL(dat, 5);
		datr |= TO256COLR(dat, 5);
		*(UINT32 *)(dst + 0) = datl;
		*(UINT32 *)(dst + 4) = datr;
		txt++;
		dst += align;
	} while(--count);
}

void makemix_mix(UINT8 *dst, UINT align,
							const UINT8 *txt, const UINT8 *grp, UINT count) {

	REG8	dat;
	UINT32	datl;
	UINT32	datr;

	do {
		dat = txt[MAKETEXT_ROW * 0];
		datl = TO256COLL(dat, 3);
		datr = TO256COLR(dat, 3);
		dat = txt[MAKETEXT_ROW * 1];
		datl |= TO256COLL(dat, 4);
		datr |= TO256COLR(dat, 4);
		dat = txt[MAKETEXT_ROW * 2];
		datl |= TO256COLL(dat, 5);
		datr |= TO256COLR(dat, 5);
		dat = grp[PLANE_B];
		datl |= TO256COLL(dat, 0);
		datr |= TO256COLR(dat, 0);
		dat = grp[PLANE_R];
		datl |= TO256COLL(dat, 1);
		datr |= TO256COLR(dat, 1);
		dat = grp[PLANE_G];
		datl |= TO256COLL(dat, 2);
		datr |= TO256COLR(dat, 2);
		*(UINT32 *)(dst + 0) = datl;
		*(UINT32 *)(dst + 4) = datr;
		txt++;
		grp++;
		dst += align;
	} while(--count);
}

void makemix_doubler(UINT8 *dst, UINT count, UINT32 skipline) {

	do {
		*(UINT32 *)(dst + SURFACE_WIDTH + 0)
										= (*(UINT32 *)(dst + 0)) + skipline;
		*(UINT32 *)(dst + SURFACE_WIDTH + 4)
										= (*(UINT32 *)(dst + 4)) + skipline;
		dst += SURFACE_WIDTH * 2;
	} while(--count);
}

void makemix_remcpy(UINT8 *dst, UINT pos, UINT count) {

	count -= pos;
	pos = pos * SURFACE_WIDTH;
	do {
		*(UINT32 *)(dst + pos + 0) = *(UINT32 *)(dst + 0);
		*(UINT32 *)(dst + pos + 4) = *(UINT32 *)(dst + 4);
		dst += SURFACE_WIDTH;
	} while(--count);
}

void makemix_ul20(UINT8 *dst, UINT pos, UINT32 skipline) {

	UINT32	dat;

	dat = (tram[TRAM_KNJ + pos] & X1KNJ_ULINE)?0x010101010:0x00000000;
	*(UINT32 *)(dst + (SURFACE_WIDTH * 0) + 0) = dat;
	*(UINT32 *)(dst + (SURFACE_WIDTH * 0) + 4) = dat;
	*(UINT32 *)(dst + (SURFACE_WIDTH * 1) + 0) = dat + skipline;
	*(UINT32 *)(dst + (SURFACE_WIDTH * 1) + 4) = dat + skipline;
	*(UINT32 *)(dst + (SURFACE_WIDTH * 2) + 0) = 0x00000000;
	*(UINT32 *)(dst + (SURFACE_WIDTH * 2) + 4) = 0x00000000;
	*(UINT32 *)(dst + (SURFACE_WIDTH * 3) + 0) = skipline;
	*(UINT32 *)(dst + (SURFACE_WIDTH * 3) + 4) = skipline;
}

void makemix_ul10(UINT8 *dst, UINT pos, UINT32 skipline) {

	UINT32	dat;

	dat = (tram[TRAM_KNJ + pos] & X1KNJ_ULINE)?0x010101010:0x00000000;
	*(UINT32 *)(dst + (SURFACE_WIDTH * 0) + 0) = dat;
	*(UINT32 *)(dst + (SURFACE_WIDTH * 0) + 4) = dat;
	*(UINT32 *)(dst + (SURFACE_WIDTH * 1) + 0) = dat + skipline;
	*(UINT32 *)(dst + (SURFACE_WIDTH * 1) + 4) = dat + skipline;
	*(UINT32 *)(dst + (SURFACE_WIDTH * 2) + 0) = dat;
	*(UINT32 *)(dst + (SURFACE_WIDTH * 2) + 4) = dat;
	*(UINT32 *)(dst + (SURFACE_WIDTH * 3) + 0) = dat + skipline;
	*(UINT32 *)(dst + (SURFACE_WIDTH * 3) + 4) = dat + skipline;
	*(UINT32 *)(dst + (SURFACE_WIDTH * 4) + 0) = 0;
	*(UINT32 *)(dst + (SURFACE_WIDTH * 4) + 4) = 0;
	*(UINT32 *)(dst + (SURFACE_WIDTH * 5) + 0) = skipline;
	*(UINT32 *)(dst + (SURFACE_WIDTH * 5) + 4) = skipline;
	*(UINT32 *)(dst + (SURFACE_WIDTH * 6) + 0) = 0;
	*(UINT32 *)(dst + (SURFACE_WIDTH * 6) + 4) = 0;
	*(UINT32 *)(dst + (SURFACE_WIDTH * 7) + 0) = skipline;
	*(UINT32 *)(dst + (SURFACE_WIDTH * 7) + 4) = skipline;
}
