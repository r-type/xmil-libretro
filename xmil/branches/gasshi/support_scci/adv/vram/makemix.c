#include	"compiler.h"
#include	"vram.h"
#include	"makescrn.h"
#include	"makesub.h"


// ’x‚¢‚Ì‚ªŒ™‚È‚çŒã‚ÅƒCƒ“ƒ‰ƒCƒ“‚É‚·‚ê‚Î‚¢‚¢

// to256colex
// bit0 = normal left
// bit3 = normal right
// bit4 = interleaf 0
// bit7 = interleaf 1

static	UINT	vramstep = ADVV_WIDTH;


void makemix_mixstep(BRESULT x2) {

	vramstep = (x2)?(ADVV_WIDTH * 2):(ADVV_WIDTH * 1);
}

void makemix_mixtext40(UINT8 *dst, const UINT8 *txt, UINT count) {

	UINT32	datl;
	UINT32	datr;
	REG8	dat;

	do {
		datl = (*(UINT32 *)(dst + 0)) & 0x38383838;
		datr = (*(UINT32 *)(dst + 4)) & 0x38383838;
		dat = txt[MAKETEXT_ROW * 0];
		datl |= (to256colex[dat] & 0x01010101) << 0;
		datr |= (to256colex[dat] & 0x08080808) >> 3;
		dat = txt[MAKETEXT_ROW * 1];
		datl |= (to256colex[dat] & 0x01010101) << 1;
		datr |= (to256colex[dat] & 0x08080808) >> 2;
		dat = txt[MAKETEXT_ROW * 2];
		datl |= (to256colex[dat] & 0x01010101) << 2;
		datr |= (to256colex[dat] & 0x08080808) >> 1;
		*(UINT32 *)(dst + 0) = datl;
		*(UINT32 *)(dst + 4) = datr;
		txt++;
		dst += vramstep;
	} while(--count);
}

void makemix_mixgrph40(UINT8 *dst, const UINT8 *grp, UINT count) {

	UINT	pos;
	UINT32	datl;
	UINT32	datr;
	REG8	dat;

	pos = 0;
	do {
		datl = (*(UINT32 *)(dst + 0)) & 0x07070707;
		datr = (*(UINT32 *)(dst + 4)) & 0x07070707;
		dat = grp[pos + GRAM_B];
		datl |= (to256colex[dat] & 0x01010101) << 3;
		datr |= (to256colex[dat] & 0x08080808) << 0;
		dat = grp[pos + GRAM_R];
		datl |= (to256colex[dat] & 0x01010101) << 4;
		datr |= (to256colex[dat] & 0x08080808) << 1;
		dat = grp[pos + GRAM_G];
		datl |= (to256colex[dat] & 0x01010101) << 5;
		datr |= (to256colex[dat] & 0x08080808) << 2;
		*(UINT32 *)(dst + 0) = datl;
		*(UINT32 *)(dst + 4) = datr;
		pos = (pos + GRAM_LINESTEP) & (GRAM_LINESTEP * 7);
		dst += vramstep;
	} while(--count);
}

void makemix_mixtext80(UINT8 *dst, const UINT8 *txt, UINT count) {

	UINT32	dat0;
	UINT32	dat1;
	REG8	dat;

	do {
		dat0 = (*(UINT32 *)(dst + 0x0000)) & 0x38383838;
		dat1 = (*(UINT32 *)(dst + 0xa000)) & 0x38383838;
		dat = txt[MAKETEXT_ROW * 0];
		dat0 |= (to256colex[dat] & 0x10101010) >> 4;
		dat1 |= (to256colex[dat] & 0x80808080) >> 7;
		dat = txt[MAKETEXT_ROW * 1];
		dat0 |= (to256colex[dat] & 0x10101010) >> 3;
		dat1 |= (to256colex[dat] & 0x80808080) >> 6;
		dat = txt[MAKETEXT_ROW * 2];
		dat0 |= (to256colex[dat] & 0x10101010) >> 2;
		dat1 |= (to256colex[dat] & 0x80808080) >> 5;
		*(UINT32 *)(dst + 0x0000) = dat0;
		*(UINT32 *)(dst + 0xa000) = dat1;
		txt++;
		dst += vramstep;
	} while(--count);
}

void makemix_mixgrph80(UINT8 *dst, const UINT8 *grp, UINT count) {

	UINT	pos;
	UINT32	dat0;
	UINT32	dat1;
	REG8	dat;

	pos = 0;
	do {
		dat0 = (*(UINT32 *)(dst + 0x0000)) & 0x07070707;
		dat1 = (*(UINT32 *)(dst + 0xa000)) & 0x07070707;
		dat = grp[pos + GRAM_B];
		dat0 |= (to256colex[dat] & 0x10101010) >> 1;
		dat1 |= (to256colex[dat] & 0x80808080) >> 4;
		dat = grp[pos + GRAM_R];
		dat0 |= (to256colex[dat] & 0x10101010) << 0;
		dat1 |= (to256colex[dat] & 0x80808080) >> 3;
		dat = grp[pos + GRAM_G];
		dat0 |= (to256colex[dat] & 0x10101010) << 1;
		dat1 |= (to256colex[dat] & 0x80808080) >> 2;
		*(UINT32 *)(dst + 0x0000) = dat0;
		*(UINT32 *)(dst + 0xa000) = dat1;
		pos = (pos + GRAM_LINESTEP) & (GRAM_LINESTEP * 7);
		dst += vramstep;
	} while(--count);
}



#if 0
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
#endif

