#include	"compiler.h"
#include	"scrnmng.h"
#include	"scrndraw.h"
#include	"sdraw.h"
#include	"palettes.h"


#if defined(SIZE_QVGA) && defined(SUPPORT_16BPP)


// ---- 200line

static void SCRNCALL qvga16_32(SDRAW sdraw, int maxy) {

const UINT8	*p;
	UINT8	*q;
	int		y;
	int		x;
	UINT32	c;

	p = sdraw->src;
	q = sdraw->dst;
	y = sdraw->y;
	do {
		if (sdraw->dirty[y]) {
			for (x=0; x<sdraw->width; x++) {
				c = xmil_pal16[p[x]];
				*(UINT16 *)q = (UINT16)(c | (c >> 16));
				q += sdraw->xalign;
			}
			q -= sdraw->xbytes;
		}
		p += SURFACE_WIDTH * 2;
		q += sdraw->yalign;
		y += 2;
	} while(y < maxy);

	sdraw->src = p;
	sdraw->dst = q;
	sdraw->y = y;
}

static void SCRNCALL qvga16_62(SDRAW sdraw, int maxy) {

const UINT8	*p;
	UINT8	*q;
	int		y;
	int		x;
	UINT32	c;

	p = sdraw->src;
	q = sdraw->dst;
	y = sdraw->y;
	do {
		if (sdraw->dirty[y]) {
			for (x=0; x<sdraw->width; x++) {
				c = xmil_pal16[p[x*2+0]] + xmil_pal16[p[x*2+1]];
				c = c & (0x07e0f81f << 1);
				*(UINT16 *)q = (UINT16)((c >> 1) | (c >> 17));
				q += sdraw->xalign;
			}
			q -= sdraw->xbytes;
		}
		p += SURFACE_WIDTH * 2;
		q += sdraw->yalign;
		y += 2;
	} while(y < maxy);

	sdraw->src = p;
	sdraw->dst = q;
	sdraw->y = y;
}


// ---- 400line

static void SCRNCALL qvga16_34(SDRAW sdraw, int maxy) {

const UINT8	*p;
	UINT8	*q;
	int		y;
	int		x;
	UINT32	c;

	p = sdraw->src;
	q = sdraw->dst;
	y = sdraw->y;
	do {
		if (sdraw->dirty[y]) {
			for (x=0; x<sdraw->width; x++) {
				c = xmil_pal16[p[0]] + xmil_pal16[p[SURFACE_WIDTH]];
				c = c & (0x07e0f81f << 1);
				*(UINT16 *)q = (UINT16)((c >> 1) | (c >> 17));
				q += sdraw->xalign;
			}
			q -= sdraw->xbytes;
		}
		p += SURFACE_WIDTH * 2;
		q += sdraw->yalign;
		y += 2;
	} while(y < maxy);

	sdraw->src = p;
	sdraw->dst = q;
	sdraw->y = y;
}

static void SCRNCALL qvga16_64(SDRAW sdraw, int maxy) {

const UINT8	*p;
	UINT8	*q;
	int		y;
	int		x;
	UINT32	c;

	p = sdraw->src;
	q = sdraw->dst;
	y = sdraw->y;
	do {
		if (sdraw->dirty[y]) {
			for (x=0; x<sdraw->width; x++) {
				c = xmil_pal16[p[x*2+0]];
				c += xmil_pal16[p[x*2+1]];
				c += xmil_pal16[p[x*2+SURFACE_WIDTH+0]];
				c += xmil_pal16[p[x*2+SURFACE_WIDTH+1]];
				c = c & (0x07e0f81f << 2);
				*(UINT16 *)q = (UINT16)((c >> 2) | (c >> 18));
				q += sdraw->xalign;
			}
			q -= sdraw->xbytes;
		}
		p += SURFACE_WIDTH * 2;
		q += sdraw->yalign;
		y += 2;
	} while(y < maxy);

	sdraw->src = p;
	sdraw->dst = q;
	sdraw->y = y;
}


// ----

static const SDRAWFN qvga16p[] = {
		qvga16_32,		qvga16_62,
		qvga16_32,		qvga16_62,
		qvga16_34,		qvga16_64,
#if defined(SUPPORT_TURBOZ)
		NULL
#endif
	};

const SDRAWFN *sdraw_getproctbl(const SCRNSURF *surf) {

	if (surf->bpp == 16) {
		return(qvga16p);
	}
	else {
		return(NULL);
	}
}

#endif

