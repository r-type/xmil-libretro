
/* #define	SINGLESTEPONLY */
/* #define	IPTRACE			(1 << 14) */
/* #define	NEVENT_COUNTER */
/* #define	PCCOUNTER */
/* #define	IOCOUNTER */
/* #define	CTCCOUNTER */

#include	"compiler.h"
#if defined(TRACE) && IPTRACE
#include	"dosio.h"
#endif
#if defined(SUPPORT_TURBOZ)
#include	"scrnmng.h"
#endif
#if !defined(DISABLE_SOUND)
#include	"soundmng.h"
#endif
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


const OEMCHAR xmilversion[] = OEMTEXT(XMILVER_CORE);

	XMILCFG		xmilcfg = { 2, 1, 0x03,
							1, 0, 0, 0,
#if !defined(DISABLE_SOUND)
							22050, 500,
#if defined(SUPPORT_TURBOZ) || defined(SUPPORT_OPM)
							64,
#endif
							64, 0, 80,
#endif
#if defined(SUPPORT_TURBOZ) || defined(SUPPORT_OPM)
							0,
#endif
							0, 0, 0, 0};

	PCCORE		pccore;
	CORESTAT	corestat;


/* ---- */

void pccore_initialize(void) {

	Z80_INITIALIZE();
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

#if 0
void neitem_dummy(UINT id) {

	nevent_repeat(id);
}
#endif

void pccore_reset(void) {

#if !defined(DISABLE_SOUND)
	soundmng_stop();
	if (corestat.soundrenewal) {
		corestat.soundrenewal = 0;
		sndctrl_deinitialize();
		sndctrl_initialize();
	}
#endif

#if !defined(FIX_Z80A)
	pccore.baseclock = 2000000;
	pccore.multiple = 2;
	pccore.realclock = pccore.baseclock * pccore.multiple;
#endif

	pccore.ROM_TYPE = xmilcfg.ROM_TYPE;
#if defined(SUPPORT_TURBOZ) || defined(SUPPORT_OPM)
	if (xmilcfg.ROM_TYPE >= 3) {
		pccore.SOUND_SW = 1;					/* 無条件で搭載 */
	}
	else {
		pccore.SOUND_SW = xmilcfg.SOUND_SW;
	}
#endif
	pccore.DIP_SW = xmilcfg.DIP_SW;

	/* スクリーンモードの変更... */
#if defined(SUPPORT_TURBOZ)
	if (pccore.ROM_TYPE >= 3) {
		if (scrnmng_setcolormode(TRUE) != SUCCESS) {
			pccore.ROM_TYPE = 2;
		}
	}
	else {
		scrnmng_setcolormode(FALSE);
	}
#endif

	sysmng_cpureset();

	sound_changeclock();
	sound_reset();

	Z80_RESET();
	nevent_allreset();
	ievent_reset();
	calendar_reset();
	iocore_reset();
/*	nevent_set(15, 4000000 / (60 * 50), neitem_dummy, NEVENT_RELATIVE); */

	pal_reset();
	makescrn_reset();
	timing_reset();

#if !defined(DISABLE_SOUND)
	soundmng_play();
#endif
}


/* ---- */

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


#if !defined(MAINFRAMES_OLD)
void neitem_mainframes(UINT id) {

	SINT32	clock;
	REG8	ppib;
	SINT32	next;

	clock = CPU_CLOCKCOUNT - iocore.e.framestartclock;
	ppib = iocore.s.ppib & (~0x84);
	do {
		next = iocore.e.dispclock;
		if (clock < next) {
			ppib |= 0x80;
			break;
		}
		next = iocore.e.dispclock;
		if (clock < next) {
			if (xmilcfg.DISPSYNC & 1) {
				scrnupdate();
			}
			break;
		}
		next = iocore.e.vsyncend;
		if (clock < next) {
			ppib |= 0x04;
			break;
		}
		next = corestat.framebaseclock;
		if (clock >= next) {
			corestat.vsync = 2;
			return;
		}
	} while(0);
	iocore.s.ppib = ppib;
	nevent_set(NEVENT_FRAMES,
							next - clock, neitem_mainframes, NEVENT_ABSOLUTE);
}
#else
void neitem_disp(UINT id) {

	corestat.vsync = 1;
	if (xmilcfg.DISPSYNC & 1) {
		scrnupdate();
	}
	nevent_set(id, corestat.syncclock, neitem_vsync, NEVENT_RELATIVE);
}

void neitem_vsync(UINT id) {

	corestat.vsync = 2;
	(void)id;
}
#endif


/* ---- */

#if defined(TRACE) && defined(IOCOUNTER)
static	UINT	iocounter = 0;
		UINT	icounter[0x2008];
		UINT	ocounter[0x2008];
#endif
#if defined(TRACE) && defined(CTCCOUNTER)
		UINT	ctccnt;
#endif
#if defined(TRACE) && defined(PCCOUNTER)
		UINT	pccnt;
		UINT	pccnt2;
		UINT	pccnt3;
		UINT	lastpc;
#endif


void pccore_exec(BRESULT draw) {

	SINT32	frameclock;
#if defined(MAINFRAMES_OLD)
	SINT32	dispclock;
#endif
#if defined(TRACE) && defined(NEVENT_COUNTER)
	UINT	ncounter = 0;
#endif

	corestat.drawframe = draw;
	pal_eventclear();
#if !defined(DISABLE_SOUND)
	soundmng_sync();
#endif

	frameclock = crtc.e.frameclock;
	if (corestat.framebaseclock != frameclock) {
		corestat.framebaseclock = frameclock;
		timing_setrate(frameclock);
	}
	corestat.vsync = 0;

	iocore.e.framestartclock = CPU_CLOCKCOUNT;

#if !defined(MAINFRAMES_OLD)
	neitem_mainframes(NEVENT_FRAMES);
#else
#if !defined(FIX_Z80A)
	frameclock = frameclock * pccore.multiple / 2;
#endif
	dispclock = min(frameclock, iocore.e.dispclock);
	corestat.syncclock = frameclock - dispclock;
	nevent_set(NEVENT_FRAMES, dispclock, neitem_disp, NEVENT_RELATIVE);
#endif

	do {
#if defined(TRACE) && defined(NEVENT_COUNTER)
		ncounter++;
#endif
#if !defined(SINGLESTEPONLY)
		if (CPU_REMCLOCK > 0) {
			Z80_EXECUTE();
		}
#else
		while(CPU_REMCLOCK > 0) {
			/* TRACEOUT(("%.4x", Z80_PC)); */
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

#if defined(TRACE) && defined(NEVENT_COUNTER)
	TRACEOUT(("loop = %d", ncounter));
#endif
#if defined(TRACE) && defined(CTCCOUNTER)
	TRACEOUT(("ctc = %d", ctccnt));
	ctccnt = 0;
#endif
#if defined(TRACE) && defined(PCCOUNTER)
	TRACEOUT(("pccnt = %d %d %d", pccnt, pccnt2, pccnt3));
	pccnt = 0;
	pccnt2 = 0;
	pccnt3 = 0;
#endif
#if defined(TRACE) && defined(IOCOUNTER)
	iocounter++;
	if (iocounter >= 60) {
		UINT i, j, cnt, pos;
		iocounter = 0;
		for (i=0; i<10; i++) {
			cnt = 0;
			pos = 0;
			for (j=0; j<0x2004; j++) {
				if (cnt < ocounter[j]) {
					cnt = ocounter[j];
					pos = j;
				}
			}
			ocounter[pos] = 0;
			TRACEOUT(("o%2d - %.4x %8dtimes", i, pos, cnt));
		}
		for (i=0; i<10; i++) {
			cnt = 0;
			pos = 0;
			for (j=0; j<0x2004; j++) {
				if (cnt < icounter[j]) {
					cnt = icounter[j];
					pos = j;
				}
			}
			icounter[pos] = 0;
			TRACEOUT(("i%2d - %.4x %8dtimes", i, pos, cnt));
		}
	}
#endif
}

