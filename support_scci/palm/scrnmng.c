#include	"compiler.h"
#include	"xmil.h"
#include	"scrnmng.h"


typedef struct {
	UInt32			width;
	UInt32			height;
	UInt16*			bits;
	BOOL			exist;
} _QDRAW, *QDRAW;


static	_QDRAW		qdraw;
static	SCRNSURF	scrnsurf;

void scrnmng_initialize(void) {

	QDRAW	qd;
	
	qd = &qdraw;
	qd->width = 640;
	qd->height = 200;
	qd->bits = NULL;
	qd->exist = FALSE;
}

BRESULT scrnmng_create(REG8 scrnmode) {

	QDRAW	qd;

	qd = &qdraw;
	m68k_scrnmng_create(qd);
	if (qd->bits) {
		qd->exist = TRUE;
		return(SUCCESS);
	}
	else {
		(void)scrnmode;
		return(FAILURE);
	}
}

void scrnmng_destroy(void) {

	QDRAW	qd;

	qd = &qdraw;
	if (qd->exist) {
		qd->exist = FALSE;
	}
}

void scrnmng_setwidth(int posx, int width) {
	(void)posx;
	(void)width;
}

void scrnmng_setextend(int extend) {

	(void)extend;
}

void scrnmng_setheight(int posy, int height) {
	(void)posy;
	(void)height;
}

const SCRNSURF *scrnmng_surflock(void) {

	QDRAW		qd;

	qd = &qdraw;
	if (!qd->exist) {
		return(NULL);
	}

	scrnsurf.ptr = (UINT8*)qd->bits;
	scrnsurf.xalign = 2;
	scrnsurf.yalign = 2 * (qd->width >> 1);
	scrnsurf.width = qd->width;
	scrnsurf.height = qd->height;
	scrnsurf.extend = 0;
	scrnsurf.bpp = 16;
	return(&scrnsurf);
}

RGB16 scrnmng_makepal16(RGB32 pal32) {

	RGB16	ret;
	ret = (pal32.p.r & 0xf8) << 8;
#if defined(SIZE_QVGA)
	ret += (pal32.p.g & 0xfc) << (3 + 16);
#else
	ret += (pal32.p.g & 0xfc) << 3;
#endif
	ret += pal32.p.b >> 3;

	return(ret);
}
