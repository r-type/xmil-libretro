#include	"compiler.h"
#include	"scrnmng.h"
#include	"sysmng.h"
#include	"pccore.h"
#include	"iocore.h"
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
	ZeroMemory(xmil_pal32, sizeof(xmil_pal32));
	xmil_palettes = 0;

	scrn.widthmode = SCRNWIDTHMODE_WIDTH40;

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


// ----

#if 0
#if defined(SUPPORT_RASTER)
static REG8 rasterdraw(SDRAWFN sdrawfn, SDRAW sdraw, int maxy) {

	SINT32		rasterclock;
	SINT32		clock;
	PAL1EVENT	*event;
	PAL1EVENT	*eventterm;
	int			nextupdate;
	int			y;

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
			(*sdrawfn)(sdraw, y);
			nextupdate = y;
			// ‚¨•Ù“–‚ðH‚×‚é
			while(clock > (event->clock << 8)) {
//				((BYTE *)pal)[event->color] = event->value;
				event++;
				if (event >= eventterm) {
					break;
				}
			}
		}
	}
	if (y < maxy) {
		if (!(np2cfg.LCD_MODE & 1)) {
			pal_makeanalog(pal, 0xffff);
		}
		else {
			pal_makeanalog_lcd(pal, 0xffff);
		}
		if (np2cfg.skipline) {
			np2_pal32[0].d = np2_pal32[NP2PAL_SKIP].d;
#if defined(SUPPORT_16BPP)
			np2_pal16[0] = np2_pal16[NP2PAL_SKIP];
#endif
		}
		(*sdrawfn)(sdraw, maxy);
	}
	if (palevent.vsyncpal) {
		return(2);
	}
	else if (nextupdate) {
		for (y=0; y<nextupdate; y+=2) {
			*(UINT16 *)(renewal_line + y) |= 0x8080;
		}
		return(1);
	}
	else {
		return(0);
	}
}
#endif
#endif


// ----

REG8 scrndraw_draw(REG8 redraw) {

const SCRNSURF	*surf;
const SDRAWFN	*sdrawfn;
	SDRAWFN		fn;
	_SDRAW		sdraw;
	UINT		i;
	REG8		ret;

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
	ret = 0;
#if 1
	(*fn)(&sdraw, 400);
#else
#if !defined(SUPPORT_PALEVENT)
	(*fn)(&sdraw, 400);
#else
	if (((dispmode & SCRN64_MASK) != SCRN64_INVALID) ||
		(palevent.events >= PALEVENTMAX)) {
		(*fn)(&sdraw, 400);
	}
	else {
		ret = rasterdraw(*sdrawfn, &sdraw, 400);
	}
#endif
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

