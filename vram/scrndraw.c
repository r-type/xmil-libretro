#include	"compiler.h"
#include	"scrnmng.h"
#include	"sysmng.h"
#include	"scrndraw.h"
#include	"sdraw.h"
#include	"palettes.h"
#include	"makescrn.h"


	SCRN	scrn;
	UINT8	renewalline[SURFACE_HEIGHT+4];
	UINT8	screenmap[SURFACE_SIZE];


static void updateallline(UINT32 update) {

	UINT	i;

	for (i=0; i<SURFACE_HEIGHT; i+=4) {
		*(UINT32 *)(renewalline + i) |= update;
	}
}


// ----

void scrndraw_initialize(void) {					// ddraws_init

	ZeroMemory(screenmap, sizeof(screenmap));
	ZeroMemory(x1n_pal32, sizeof(x1n_pal32));

	scrn.widthmode = SCRNWIDTHMODE_WIDTH40;

	xm_palettes = 0;
	updateallline(0x03030303);			// updateallline(0x01010101);
	scrnmng_allflash();
	sysmng_scrnwidth(scrn.widthmode);
}

void scrndraw_changewidth(REG8 widthmode) {

	if (scrn.widthmode != widthmode) {
		scrn.widthmode = widthmode;
		sysmng_scrnwidth(widthmode);
		updateallline(0x01010101);				// fillrenewalline(0x03030303)
	}
}

void scrndraw_changepalette(void) {

#if defined(SUPPORT_8BPP)
	if (scrnmng_getbpp() == 8) {
		scrnmng_palchanged();
		updateallline(0x01010101);				// fillrenewalline(0x02020202)
		return;
	}
#endif
	updateallline(0x01010101);					// fillrenewalline(0x03030303)
}

REG8 scrndraw_draw(REG8 redraw) {

const SCRNSURF	*surf;
const SDRAWFN	*sdrawfn;
	SDRAWFN		fn;
	_SDRAW		sdraw;
	UINT		i;

	if (redraw) {
		updateallline(0x01010101);
	}

	surf = scrnmng_surflock();
	if (surf == NULL) {
		goto sddr_exit1;
	}
	sdrawfn = sdraw_getproctbl(surf);
	if (sdrawfn == NULL) {
		goto sddr_exit2;
	}
	switch(scrn.widthmode) {
		case SCRNWIDTHMODE_WIDTH80:
		default:
			break;

		case SCRNWIDTHMODE_WIDTH40:
			sdrawfn += 1;
			break;

		case SCRNWIDTHMODE_4096:
			sdrawfn += 2;
			break;
	}
	fn = *sdrawfn;
	if (fn == NULL) {
		goto sddr_exit2;
	}

	for (i=0; i<SURFACE_HEIGHT; i++) {
		sdraw.dirty[i] = renewalline[i] & 1;
		if (sdraw.dirty[i]) {
			renewalline[i] &= ~1;
		}
	}

	sdraw.src = screenmap;
	sdraw.dst = surf->ptr;
	sdraw.width = surf->width;
	sdraw.xbytes = surf->xalign * surf->width;
	sdraw.y = 0;
	sdraw.xalign = surf->xalign;
	sdraw.yalign = surf->yalign;
	(*fn)(&sdraw, 400);

sddr_exit2:
	scrnmng_surfunlock(surf);

sddr_exit1:
	return(0);
}

void scrndraw_redraw(void) {

	scrnmng_allflash();
	pal_update();
	updateallline(0x01010101);					// fillrenewalline(0x03030303)
	scrndraw_draw(FALSE);
}

