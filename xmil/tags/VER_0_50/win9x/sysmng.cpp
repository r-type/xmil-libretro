#include	"compiler.h"
#include	"xmil.h"
#include	"dosio.h"
#include	"sysmng.h"
#include	"menu.h"
#include	"z80core.h"
#include	"pccore.h"
#include	"fddfile.h"
#include	"makescrn.h"


	UINT	sys_updates;


void sysmng_scrnwidth(REG8 mode) {

	menu_setwidth(mode);
}


// ----

static	OEMCHAR	title[512];
static	OEMCHAR	clock[64];

static struct {
	UINT32	tick;
	UINT32	clock;
	UINT32	draws;
	SINT32	fps;
	SINT32	khz;
} workclock;

void sysmng_workclockreset(void) {

	workclock.tick = GETTICK();
	workclock.clock = CPU_CLOCK;
	workclock.draws = drawtime;				// drawcount;
}

BOOL sysmng_workclockrenewal(void) {

	SINT32	tick;

	tick = GETTICK() - workclock.tick;
	if (tick < 2000) {
		return(FALSE);
	}
	workclock.tick += tick;
	workclock.fps = ((drawtime - workclock.draws) * 10000) / tick;
	workclock.draws = drawtime;
	workclock.khz = (CPU_CLOCK - workclock.clock) / tick;
	workclock.clock = CPU_CLOCK;
	return(TRUE);
}

void sysmng_updatecaption(REG8 flag) {

	OEMCHAR	work[512];

	if (flag & 1) {
		title[0] = '\0';
		if (fddfile_diskready(0)) {
			milstr_ncat(title, OEMTEXT("  FDD0:"), _countof(title));
			milstr_ncat(title, file_getname(fddfile_diskname(0)), _countof(title));
		}
		if (fddfile_diskready(1)) {
			milstr_ncat(title, OEMTEXT("  FDD1:"), _countof(title));
			milstr_ncat(title, file_getname(fddfile_diskname(1)), _countof(title));
		}
	}
	if (flag & 2) {
		clock[0] = '\0';
		if (xmiloscfg.DISPCLK & 2) {
			if (workclock.fps) {
				OEMSPRINTF(clock, OEMTEXT(" - %u.%1uFPS"), workclock.fps / 10, workclock.fps % 10);
			}
			else {
				milstr_ncpy(clock, OEMTEXT(" - 0FPS"), _countof(clock));
			}
		}
		if (xmiloscfg.DISPCLK & 1) {
			OEMSPRINTF(work, OEMTEXT(" %u.%03uMHz"), workclock.khz / 1000, workclock.khz % 1000);
			if (clock[0] == '\0') {
				milstr_ncpy(clock, OEMTEXT(" -"), _countof(clock));
			}
			milstr_ncat(clock, work, _countof(clock));
		}
	}
	milstr_ncpy(work, szProgName, _countof(work));
	milstr_ncat(work, title, _countof(work));
	milstr_ncat(work, clock, _countof(work));
	SetWindowText(hWndMain, work);
}

