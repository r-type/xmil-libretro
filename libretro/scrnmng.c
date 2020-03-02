#include	"compiler.h"
// #include	<sys/time.h>
// #include	<signal.h>
// #include	<unistd.h>
#include	"scrnmng.h"
#include	"scrndraw.h"
#include	"vramhdl.h"
#include	"menubase.h"

extern unsigned short int videoBuffer[640*400];

/*
static SDL_Window *s_sdlWindow;
static SDL_Renderer *s_renderer;
static SDL_Texture *s_texture;
static SDL_Surface *s_surface;
*/

typedef struct {
	BOOL		enable;
	int			width;
	int			height;
	int			bpp;
	int		*surface;
	VRAMHDL		vram;
} SCRNMNG;

typedef struct {
	int		width;
	int		height;
} SCRNSTAT;

static const char app_name[] = "X millennium";

static	SCRNMNG		scrnmng;
static	SCRNSTAT	scrnstat;
static	SCRNSURF	scrnsurf;

typedef struct {
	int		xalign;
	int		yalign;
	int		width;
	int		height;
	int		srcpos;
	int		dstpos;
} DRAWRECT;

static BOOL calcdrawrect(int *surface,
								DRAWRECT *dr, VRAMHDL s, const RECT_T *rt) {

	int		pos;

	dr->xalign = 2;
	dr->yalign = 640*2;
	dr->srcpos = 0;
	dr->dstpos = 0;
	dr->width = min(scrnmng.width, s->width);
	dr->height = min(scrnmng.height, s->height);
	if (rt) {
		pos = max(rt->left, 0);
		dr->srcpos += pos;
		dr->dstpos += pos * dr->xalign;
		dr->width = min(rt->right, dr->width) - pos;

		pos = max(rt->top, 0);
		dr->srcpos += pos * s->width;
		dr->dstpos += pos * dr->yalign;
		dr->height = min(rt->bottom, dr->height) - pos;
	}
	if ((dr->width <= 0) || (dr->height <= 0)) {
		return(FAILURE);
	}
	return(SUCCESS);
}


void scrnmng_initialize(void) {

	scrnstat.width = 640;
	scrnstat.height = 400;
}

BOOL scrnmng_create(int width, int height) {

	scrnmng.enable = TRUE;
	scrnmng.width = width;
	scrnmng.height = height;
	scrnmng.bpp = 16;
printf("wh(%d,%d)\n",width, height);
	return(SUCCESS);
}

void scrnmng_destroy(void) {

	scrnmng.enable = FALSE;
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

void scrnmng_setwidth(int posx, int width) {

	scrnstat.width = width;
}

void scrnmng_setheight(int posy, int height) {

	scrnstat.height = height;
}

const SCRNSURF *scrnmng_surflock(void) {

	if (scrnmng.vram == NULL) {

		scrnmng.surface = NULL;
		scrnsurf.ptr = (BYTE *)videoBuffer;
		scrnsurf.xalign = 2;
		scrnsurf.yalign = 640*2;
		scrnsurf.bpp = 16;
	}
	else {
		scrnsurf.ptr = scrnmng.vram->ptr;
		scrnsurf.xalign = scrnmng.vram->xalign;
		scrnsurf.yalign = scrnmng.vram->yalign;
		scrnsurf.bpp = scrnmng.vram->bpp;
	}
	scrnsurf.width = min(scrnstat.width, 640);
	scrnsurf.height = min(scrnstat.height, 400);
	scrnsurf.extend = 0;
	return(&scrnsurf);
}

void scrnmng_surfunlock(const SCRNSURF *surf) {
}

