#include	"compiler.h"
#include	"xmil.h"
#include	"dosio.h"
#include	"sysmng.h"
#include	"z80core.h"
#include	"pccore.h"
#include	"fddfile.h"
#include	"makescrn.h"
#include	"resource.h"


	UINT	sys_updates;


static	char	strtitle0[256];
static	char	strtitle1[256];
static	char	strclock[64];
static	char	strframe[64];

static struct {
	UINT32	tick;
	UINT32	clock;
	UINT32	draws;
	SINT32	fps;
	SINT32	khz;
} workclock;

void sysmng_workclockreset(void) {

	workclock.tick = GETTICK();
	workclock.clock = CPU_CLOCK;	// CPU_CLOCK;
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

static void changelabel(FormType* frm, UINT16 id, char* text) {

	RectangleType	bounds;
	UINT16			index;
	
	index = ARM_FrmGetObjectIndex(frm, id);
	ARM_FrmGetObjectBounds(frm, index, &bounds);
	ARM_FrmHideObject(frm, index);
	ARM_FrmCopyLabel(frm, id, text);
	ARM_FrmSetObjectBounds(frm, index, &bounds);
	ARM_FrmShowObject(frm, index);
}

static void adjustlabel(char* text, UINT length) {

	UINT16 textlen, titlelen;
	
	textlen = ARM_StrLen(text);
	titlelen = ARM_FntWidthToOffset(text, textlen, length, NULL, NULL);
	if (titlelen != textlen) {
		text[titlelen-1] = chrEllipsis;
		text[titlelen] = '\0';
	}
}

void sysmng_updatecaption(BYTE flag) {

	const char nodisk[] = "No Disk Image";
	FormType*	frm = ARM_FrmGetActiveForm();
	
	if (flag & 1) {
		strtitle0[0] = '\0';
		milstr_ncat(strtitle0, "0:", sizeof(strtitle0));
		if (fddfile_diskready(0)) {
			milstr_ncat(strtitle0, file_getname((char *)fddfile_diskname(0)),
															sizeof(strtitle0));
		} else {
			milstr_ncat(strtitle0, nodisk, sizeof(strtitle0));
		}
		adjustlabel(strtitle0, 102);
		changelabel(frm, IDC_FDD0, strtitle0);

		strtitle1[0] = '\0';
		milstr_ncat(strtitle1, "1:", sizeof(strtitle1));
		if (fddfile_diskready(1)) {
			milstr_ncat(strtitle1, file_getname((char *)fddfile_diskname(1)),
															sizeof(strtitle1));
		} else {
			milstr_ncat(strtitle1, nodisk, sizeof(strtitle1));
		}
		adjustlabel(strtitle1, 102);
		changelabel(frm, IDC_FDD1, strtitle1);
	}
	if (flag & 2) {
		strframe[0] = '\0';
		if (xmiloscfg.DISPCLK & 2) {
			if (workclock.fps) {
				ARM_StrPrintF_2(strframe, "%u.%1uFPS",
								workclock.fps / 10, workclock.fps % 10);
			}
			else {
				milstr_ncpy(strframe, "0.0FPS", sizeof(strframe));
			}
		}
		else {
			milstr_ncpy(strframe, "-.-FPS", sizeof(strframe));
		}
		changelabel(frm, IDC_DISPFRAME, strframe);
		
		strclock[0] = '\0';
		if (xmiloscfg.DISPCLK & 1) {
			ARM_StrPrintF_2(strclock, "%2u.%03uMHz",
								workclock.khz / 1000, workclock.khz % 1000);
		}
		else {
			milstr_ncpy(strclock, "-.---MHz", sizeof(strclock));
		}
		changelabel(frm, IDC_DISPCLOCK, strclock);
	}
}

