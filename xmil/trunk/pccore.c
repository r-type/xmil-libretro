#include	"compiler.h"
#include	"dosio.h"
#include	"scrnmng.h"
#include	"soundmng.h"
#include	"sysmng.h"
#include	"timemng.h"
#include	"xmilver.h"
#include	"z80core.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"nevent.h"
#include	"ievent.h"
#include	"timing.h"
#include	"calendar.h"
#include	"keystat.h"
#include	"palettes.h"
#include	"makescrn.h"
#include	"sound.h"
#include	"sndctrl.h"
#include	"font.h"
#include	"fddfile.h"
#include	"defrom.res"


const OEMCHAR xmilversion[] = OEMTEXT(XMILVER_CORE);

	XMILCFG		xmilcfg = { 2, 1, 0x03,
							1, 0, 0, 0,
							22050, 500, 0, 0, 80,
							0, 0, 0, 0};

	PCCORE		pccore;
	CORESTAT	corestat;
	UINT8		mMAIN[0x10000];
	UINT8		mBIOS[0x8000];
#if defined(SUPPORT_BANKMEM)
	UINT8		mBANK[16][0x8000];
#endif
	BYTE		*RAM0r;
	BYTE		*RAM0w;


// ----

static void ipl_load(void) {

	FILEH	hdl;

	ZeroMemory(mBIOS, sizeof(mBIOS));
	CopyMemory(mBIOS, DEFROM, sizeof(DEFROM));

	if (pccore.ROM_TYPE >= 2) {
		if ((hdl = file_open_c(OEMTEXT("IPLROM.X1T"))) != FILEH_INVALID) {
			file_read(hdl, mBIOS, 0x8000);
			file_close(hdl);
		}
	}
	else if (pccore.ROM_TYPE == 1) {
		if ((hdl = file_open_c(OEMTEXT("IPLROM.X1"))) != FILEH_INVALID) {
			file_read(hdl, mBIOS, 0x8000);
			file_close(hdl);
		}
	}
}


// ----

void pccore_initialize(void) {

	fddfile_initialize();
	sndctrl_initialize();
	makescrn_initialize();

	font_load(NULL, TRUE);

	crtc_initialize();
	pcg_initialize();
	ppi_initialize();
}

void pccore_deinitialize(void) {

	sndctrl_deinitialize();

	fddfile_eject(0);
	fddfile_eject(1);
	fddfile_eject(2);
	fddfile_eject(3);
}

void pccore_reset(void) {

	soundmng_stop();
	if (corestat.soundrenewal) {
		corestat.soundrenewal = 0;
		sndctrl_deinitialize();
		sndctrl_initialize();
	}

	pccore.baseclock = 2000000;
	pccore.multiple = 2;
	pccore.realclock = pccore.baseclock * pccore.multiple;

	pccore.ROM_TYPE = xmilcfg.ROM_TYPE;
	pccore.SOUND_SW = xmilcfg.SOUND_SW;
	pccore.DIP_SW = xmilcfg.DIP_SW;

	// スクリーンモードの変更...
	if (pccore.ROM_TYPE >= 3) {
		if (scrnmng_setcolormode(TRUE) != SUCCESS) {
			pccore.ROM_TYPE = 2;
		}
	}
	else {
		scrnmng_setcolormode(FALSE);
	}

	sysmng_cpureset();

	sound_changeclock();
	sound_reset();

	Z80_RESET();
	nevent_allreset();
	ievent_reset();
	calendar_reset();
	iocore_reset();

	ipl_load();

	RAM0r = mBIOS;
	RAM0w = mMAIN;

	pal_reset();
	makescrn_reset();
	timing_reset();

	soundmng_play();
}


// ----

// #define	IPTRACE			(1 << 14)

#if defined(TRACE) && IPTRACE
static	UINT	trpos = 0;
static	UINT16	treip[IPTRACE];

void iptrace_out(void) {

	FILEH	fh;
	UINT	s;
	UINT16	pc;
	char	buf[32];

	s = trpos;
	if (s > IPTRACE) {
		s -= IPTRACE;
	}
	else {
		s = 0;
	}
	fh = file_create_c("his.txt");
	while(s < trpos) {
		pc = treip[s & (IPTRACE - 1)];
		s++;
		SPRINTF(buf, "%.4x\r\n", pc);
		file_write(fh, buf, strlen(buf));
	}
	file_close(fh);
}
#endif


void neitem_disp(UINT id) {

	corestat.vsync = 1;
	pcg.r.vsync = 1;
	if (xmilcfg.DISPSYNC & 1) {
		scrnupdate();
	}
	nevent_set(id, corestat.syncclock, neitem_vsync, NEVENT_RELATIVE);
}

void neitem_vsync(UINT id) {

	corestat.vsync = 2;
	(void)id;
}


// ----

// #define	SINGLESTEPONLY

void pccore_exec(BRESULT draw) {

	SINT32	frameclock;
	SINT32	dispclock;

	corestat.drawframe = draw;
	soundmng_sync();

	frameclock = 266 * RASTER_CLOCK * pccore.multiple / 2;
	dispclock = min(frameclock, crtc.e.dispclock);
	corestat.dispclock = dispclock;
	corestat.syncclock = frameclock - dispclock;
	corestat.vsync = 0;
	nevent_set(NEVENT_FRAMES, dispclock, neitem_disp, NEVENT_RELATIVE);
	do {
#if !defined(SINGLESTEPONLY)
		if (CPU_REMCLOCK > 0) {
			Z80_EXECUTE();
		}
#else
		while(CPU_REMCLOCK > 0) {
			//	TRACEOUT(("%.4x", Z80_PC));
#if defined(TRACE) && IPTRACE
			treip[trpos & (IPTRACE - 1)] = Z80_PC;
			trpos++;
#endif
			Z80_STEP();
		}
#endif
		nevent_progress();
		ievent_progress();
	} while(corestat.vsync < 2);

	scrnupdate();
	sound_sync();
	fdc_callback();
}

