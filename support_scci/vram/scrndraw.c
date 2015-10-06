#include	"compiler.h"
#include	"scrnmng.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"scrndraw.h"
#include	"sdraw.h"
#include	"palettes.h"
#include	"makescrn.h"


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
	ZeroMemory(xmil_pal32, sizeof(xmil_pal32));
	xmil_palettes = 0;

	updateallline(0x03030303);			// updateallline(0x01010101);
	scrnmng_allflash();
}

void scrndraw_changepalette(void) {

#if defined(SUPPORT_8BPP)
	if (scrnmng_getbpp() == 8) {
		scrnmng_palchanged();
		return;
	}
#endif
	updateallline(0x01010101);					// fillrenewalline(0x03030303)
}


// ----

#if defined(SUPPORT_PALEVENT)
static REG8 rasterdraw(SDRAWFN sdrawfn, SDRAW sdraw, int maxy) {

	UINT8		rgbp[8];
	SINT32		rasterclock;
	SINT32		clock;
	PAL1EVENT	*event;
	PAL1EVENT	*eventterm;
	int			nextupdate;
	int			y;

	CopyMemory(rgbp, palevent.rgbp, 8);
	rasterclock = crtc.e.rasterclock8;
	if (crtc.s.SCRN_BITS & SCRN_24KHZ) {
		rasterclock = rasterclock * 2;
	}
	clock = 0;
	event = palevent.event;
	eventterm = event + palevent.events;
	nextupdate = 0;

	for (y=2; y<maxy; y+=2) {
		if (event >= eventterm) {
			break;
		}
		clock += rasterclock;
		// ‚¨•Ù“–‚Í‚ ‚Á‚½H
		if (clock > (event->clock << 8)) {
			pal_update1(rgbp);
			(*sdrawfn)(sdraw, y);
			nextupdate = y;
			// ‚¨•Ù“–‚ðH‚×‚é
			while(clock > (event->clock << 8)) {
				((UINT8 *)rgbp)[event->rgbp] = event->value;
				event++;
				if (event >= eventterm) {
					break;
				}
			}
		}
	}
	if (y < maxy) {
		pal_update1(rgbp);
		(*sdrawfn)(sdraw, maxy);
	}
	// –ß‚·‚·‚·‚·
	pal_update1(crtc.s.rgbp);

	if (nextupdate) {
		for (y=0; y<nextupdate; y+=2) {
			*(UINT16 *)(renewalline + y) |= 0x0101;
		}
		return(SCRNUPD_FLASH);
	}
	else {
		return(0);
	}
}
#endif


// ----

REG8 scrndraw_draw(REG8 redraw) {

	REG8		ret;
const SCRNSURF	*surf;
const SDRAWFN	*sdrawfn;
	SDRAWFN		fn;
	_SDRAW		sdraw;
	UINT		i;

	if (redraw) {
		updateallline(0x01010101);
	}

	ret = 0;
	surf = scrnmng_surflock();
	if (surf == NULL) {
		goto sddr_exit1;
	}
	sdrawfn = sdraw_getproctbl(surf);
	if (sdrawfn == NULL) {
		goto sddr_exit2;
	}
	sdrawfn += makescrn.drawmode;
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
#if defined(SIZE_QVGA)
	sdraw.width = surf->width >> 1;
#else
	sdraw.width = surf->width;
#endif
	sdraw.xbytes = surf->xalign * sdraw.width;
	sdraw.y = 0;
	sdraw.xalign = surf->xalign;
	sdraw.yalign = surf->yalign;
#if !defined(SUPPORT_PALEVENT)
	(*fn)(&sdraw, 400);
#else
	if ((crtc.e.dispmode & SCRN64_ENABLE) ||
		(palevent.events >= SUPPORT_PALEVENT)) {
		(*fn)(&sdraw, 400);
	}
	else {
		ret = rasterdraw(*sdrawfn, &sdraw, 400);
	}
#endif

sddr_exit2:
	scrnmng_surfunlock(surf);

sddr_exit1:
	return(ret);
}

void scrndraw_redraw(void) {

	scrnmng_allflash();
	pal_update();
	scrndraw_draw(TRUE);
}

