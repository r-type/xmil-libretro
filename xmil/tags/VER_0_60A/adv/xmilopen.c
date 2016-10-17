#include	"compiler.h"
#include	"parts.h"
#include	"xmilopen.res"


static void lzx(UINT8 *dst, int dstsize, const UINT8 *src) {

	int		level;
	UINT8	ctrl;
	UINT8	bit;
	UINT	mask;
	UINT	tmp;
	int		pos;
	int		leng;

	level = *src++;
	ctrl = 0;
	bit = 0;
	mask = (1 << level) - 1;
	while(dstsize > 0) {
		if (!bit) {
			ctrl = *src++;
			bit = 0x80;
		}
		if (ctrl & bit) {
			tmp = *src++;
			tmp <<= 8;
			tmp |= *src++;
			pos = -1 - (tmp >> level);
			leng = (tmp & mask) + 1;
			leng = min(leng, dstsize);
			dstsize -= leng;
			while(leng--) {
				*dst = *(dst + pos);
				dst++;
			}
		}
		else {
			*dst++ = *src++;
			dstsize--;
		}
		bit >>= 1;
	}
}


static void vsyncwait(void) {

	while (*REG_VCOUNT >= 160) { }
	while (*REG_VCOUNT < 160) { }
}

static void setbg16(UINT16 *map, UINT8 *tile, UINT patnum,
										const UINT8 *pat, UINT w, UINT h) {

	UINT	x;
	UINT	y;

	tile += patnum * 32;
	do {
		for (x=0; x<w; x++) {
			*map++ = (UINT16)(patnum + x);
		}
		patnum += w;
		for (y=0; y<8; y++) {
			for (x=0; x<w; x++) {
				*(UINT32 *)(tile + (x * 32)) = *(UINT32 *)pat;
				pat += 4;
			}
			tile += 4;
		}
		map += (32 - w);
		tile += ((w - 1) * 32);
	} while(--h);
}

void xmilopen(void) {

	UINT	i;
	UINT8	*pat;
	UINT16	*map;

	bset16(ADVVIDEOPAL, 256, 0x7fff);
	advv_setmode(ADVV_BG0 | ADVV_MODE0);

	*REG_BG0CNT = (16 << ADVV_BGSCRNBASESFT) + (0 << ADVV_BGCHRBASESFT);
	*REG_BG0HOFS = 0;
	*REG_BG0VOFS = 32;

	*REG_BLDCNT = ADVV_EABG0 + ADVV_EABD + ADVV_EWHITE;
	*REG_BLDY = 16;

	bcopy16(ADVVIDEOPAL, xmopal, 16);

	pat = malloc(224*32/2);
	if (pat == NULL) {
		return;
	}

	map = (UINT16 *)(ADVVIDEOBUFFER + 0x8000);
	bzero32(ADVVIDEOBUFFER, 8);
	bzero16(map, 32*32);

	lzx(pat, 224*32/2, xm2a_bmp);
	setbg16(map + 10*32 + 1, ADVVIDEOBUFFER, 1, pat, 224/8, 32/8);

	for (i=0; i<16; i++) {
		vsyncwait();
		*REG_BLDY = (UINT16)(15 - i);
	}

	lzx(pat, 128*16/2, xm2d_bmp);
	setbg16(map + 14*32 + 8, ADVVIDEOBUFFER, 28*4+1, pat, 128/8, 16/8);

	free(pat);

	for (i=0; i<64; i++) {
		vsyncwait();
	}

	*REG_BLDCNT = ADVV_EABG0 + ADVV_EABD + ADVV_EBLACK;

	for (i=0; i<16; i++) {
		vsyncwait();
		*REG_BG0VOFS = (UINT16)(32 - i);
		*REG_BLDY = (UINT16)(i + 1);
	}

	bzero16(ADVVIDEOPAL, 256);
	bzero32(ADVVIDEOBUFFER, 0x18000 / 4);

	*REG_BG0VOFS = 0;
	*REG_BLDCNT = 0;
	*REG_BLDY = 0;
}

