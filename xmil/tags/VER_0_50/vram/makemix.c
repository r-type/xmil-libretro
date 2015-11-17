#include	"compiler.h"
#include	"vram.h"
#include	"scrndraw.h"
#include	"makesub.h"


// ’x‚¢‚Ì‚ªŒ™‚È‚çŒã‚ÅƒCƒ“ƒ‰ƒCƒ“‚É‚·‚ê‚Î‚¢‚¢
void makemix_mixtext(UINT8 *dst, UINT align, const UINT8 *txt, UINT count) {

	UINT32	datl;
	UINT32	datr;
	REG8	dat;

	do {
		datl = (*(UINT32 *)(dst + 0)) & 0x07070707;
		datr = (*(UINT32 *)(dst + 4)) & 0x07070707;
		dat = txt[MAKETEXT_ROW * 0];
		datl |= TO256COLL(dat, 3);
		datr |= TO256COLR(dat, 3);
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

void makemix_mixgrph(UINT8 *dst, UINT align, const UINT8 *grp, UINT count) {

	UINT	pos;
	UINT32	datl;
	UINT32	datr;
	REG8	dat;

	pos = 0;
	do {
		datl = (*(UINT32 *)(dst + 0)) & 0x38383838;
		datr = (*(UINT32 *)(dst + 4)) & 0x38383838;
		dat = grp[pos + GRAM_B];
		datl |= TO256COLL(dat, 0);
		datr |= TO256COLR(dat, 0);
		dat = grp[pos + GRAM_R];
		datl |= TO256COLL(dat, 1);
		datr |= TO256COLR(dat, 1);
		dat = grp[pos + GRAM_G];
		datl |= TO256COLL(dat, 2);
		datr |= TO256COLR(dat, 2);
		*(UINT32 *)(dst + 0) = datl;
		*(UINT32 *)(dst + 4) = datr;
		pos = (pos + GRAM_LINESTEP) & (GRAM_LINESTEP * 7);
		dst += align;
	} while(--count);
}


void makemix_settext(UINT8 *dst, UINT align, const UINT8 *txt, UINT count) {

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

void makemix_ul20(UINT8 *dst, UINT pos) {

	UINT32	dat;

	dat = (TRAM_KNJ(pos) & TRAMKNJ_ULINE)?0x01010101:0x00000000;
	*(UINT32 *)(dst + (SURFACE_WIDTH * 0) + 0) = dat;
	*(UINT32 *)(dst + (SURFACE_WIDTH * 0) + 4) = dat;
	*(UINT32 *)(dst + (SURFACE_WIDTH * 1) + 0) = dat;
	*(UINT32 *)(dst + (SURFACE_WIDTH * 1) + 4) = dat;
	*(UINT32 *)(dst + (SURFACE_WIDTH * 2) + 0) = 0;
	*(UINT32 *)(dst + (SURFACE_WIDTH * 2) + 4) = 0;
	*(UINT32 *)(dst + (SURFACE_WIDTH * 3) + 0) = 0;
	*(UINT32 *)(dst + (SURFACE_WIDTH * 3) + 4) = 0;
}

void makemix_ul10(UINT8 *dst, UINT pos) {

	UINT32	dat;

	dat = (TRAM_KNJ(pos) & TRAMKNJ_ULINE)?0x01010101:0x00000000;
	*(UINT32 *)(dst + (SURFACE_WIDTH * 0) + 0) = dat;
	*(UINT32 *)(dst + (SURFACE_WIDTH * 0) + 4) = dat;
//	*(UINT32 *)(dst + (SURFACE_WIDTH * 1) + 0) = dat;
//	*(UINT32 *)(dst + (SURFACE_WIDTH * 1) + 4) = dat;
	*(UINT32 *)(dst + (SURFACE_WIDTH * 2) + 0) = dat;
	*(UINT32 *)(dst + (SURFACE_WIDTH * 2) + 4) = dat;
//	*(UINT32 *)(dst + (SURFACE_WIDTH * 3) + 0) = dat;
//	*(UINT32 *)(dst + (SURFACE_WIDTH * 3) + 4) = dat;
	*(UINT32 *)(dst + (SURFACE_WIDTH * 4) + 0) = 0;
	*(UINT32 *)(dst + (SURFACE_WIDTH * 4) + 4) = 0;
//	*(UINT32 *)(dst + (SURFACE_WIDTH * 5) + 0) = 0;
//	*(UINT32 *)(dst + (SURFACE_WIDTH * 5) + 4) = 0;
	*(UINT32 *)(dst + (SURFACE_WIDTH * 6) + 0) = 0;
	*(UINT32 *)(dst + (SURFACE_WIDTH * 6) + 4) = 0;
//	*(UINT32 *)(dst + (SURFACE_WIDTH * 7) + 0) = 0;
//	*(UINT32 *)(dst + (SURFACE_WIDTH * 7) + 4) = 0;
}


void makemix_cpy200(UINT8 *dst, UINT pos, UINT count) {

	count -= pos;
	pos = pos * SURFACE_WIDTH * 2;
	do {
		*(UINT32 *)(dst + pos + 0) = *(UINT32 *)(dst + 0);
		*(UINT32 *)(dst + pos + 4) = *(UINT32 *)(dst + 4);
		dst += SURFACE_WIDTH * 2;
	} while(--count);
}

void makemix_cpy400(UINT8 *dst, UINT pos, UINT count) {

	count -= pos;
	pos = pos * SURFACE_WIDTH;
	do {
		*(UINT32 *)(dst + pos + 0) = *(UINT32 *)(dst + 0);
		*(UINT32 *)(dst + pos + 4) = *(UINT32 *)(dst + 4);
		dst += SURFACE_WIDTH;
	} while(--count);
}

