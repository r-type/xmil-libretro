#include	"compiler.h"
#include	"xmil.h"
#include	"dosio.h"
#include	"sysmng.h"
#include	"z80core.h"
#include	"pccore.h"
#include	"fddfile.h"
#include	"makescrn.h"


	UINT	sys_updates;


static	char	strtitle[256];
static	char	strclock[64];

static struct {
	UINT32	tick;
	UINT32	clock;
	UINT32	draws;
	SINT32	fps;
	SINT32	khz;
} workclock;

void sysmng_workclockreset(void) {

	workclock.tick = GETTICK();
	workclock.clock = h_cnt + h_cntbase;	// CPU_CLOCK;
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
	workclock.khz = (h_cnt + h_cntbase - workclock.clock) / tick;
	workclock.clock = h_cnt + h_cntbase;
	return(TRUE);
}

void sysmng_updatecaption(BYTE flag) {

	char	work[256];
	Str255	str;

	if (flag & 1) {
		strtitle[0] = '\0';
		if (fdd_diskready(0)) {
			milstr_ncat(strtitle, "  FDD0:", sizeof(strtitle));
			milstr_ncat(strtitle, file_getname((char *)fdd_diskname(0)),
															sizeof(strtitle));
		}
		if (fdd_diskready(1)) {
			milstr_ncat(strtitle, "  FDD1:", sizeof(strtitle));
			milstr_ncat(strtitle, file_getname((char *)fdd_diskname(1)),
															sizeof(strtitle));
		}
	}
	if (flag & 2) {
		strclock[0] = '\0';
		if (xmiloscfg.DISPCLK & 2) {
			if (workclock.fps) {
				SPRINTF(strclock, " - %u.%1uFPS",
								workclock.fps / 10, workclock.fps % 10);
			}
			else {
				milstr_ncpy(strclock, " - 0FPS", sizeof(strclock));
			}
		}
		if (xmiloscfg.DISPCLK & 1) {
			SPRINTF(work, " %2u.%03uMHz",
								workclock.khz / 1000, workclock.khz % 1000);
			if (strclock[0] == '\0') {
				milstr_ncpy(strclock, " -", sizeof(strclock));
			}
			milstr_ncat(strclock, work, sizeof(strclock));
		}
	}
	milstr_ncpy(work, "Nekoe System", sizeof(work));
	milstr_ncat(work, strtitle, sizeof(work));
	milstr_ncat(work, strclock, sizeof(work));

	mkstr255(str, work);
	SetWTitle(hWndMain, str);
}

