#include	"compiler.h"
#include	"resource.h"
#include	"strres.h"
#include	"xmil.h"
#include	"dosio.h"
#include	"mousemng.h"
#include	"scrnmng.h"
#include	"soundmng.h"
#include	"sysmng.h"
#include	"palmkbd.h"
#include	"ini.h"
#include	"dialog.h"
#include	"z80core.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"timing.h"
#include	"debugsub.h"
#include	"scrndraw.h"
#include	"makescrn.h"
#include	"diskdrv.h"
#include	"fdd_ini.h"
#include	"x1f.h"

#include	"keystat.h"
#include	"fddfile.h"
#include	"PalmNavigator.h"
#include	"joymng.h"
#include	"statsave.h"


		XMILOSCFG	xmiloscfg = {100, 100,  0, 0, 3, 2, 3, 4, 5, 
#ifdef SUPPORT_RESUME
		0,
#endif
		0, 0, 0};

		BRESULT		xmilrunning;
static	BRESULT		inmenu = FALSE;

#define	DEFAULTPATH		"/Palm/Programs/X1/"
#define MAINLOOPWAIT	100

static	char	target[MAX_PATH] = DEFAULTPATH;

static const char xmilresume[] = "sav";
static const char xmilapp[] = "xmil";

void SetHardKey(UINT32 data) {

	UINT	update = 0;
	switch (data & 0xffff) {
		case IDC_KEYLIST1:
			xmiloscfg.HARDKEY1 = data >> 16;
			update = SYS_UPDATEOSCFG;
			break;
		case IDC_KEYLIST2:
			xmiloscfg.HARDKEY2 = data >> 16;
			update = SYS_UPDATEOSCFG;
			break;
		case IDC_KEYLIST3:
			xmiloscfg.HARDKEY3 = data >> 16;
			update = SYS_UPDATEOSCFG;
			break;
		case IDC_KEYLIST4:
			xmiloscfg.HARDKEY4 = data >> 16;
			update = SYS_UPDATEOSCFG;
			break;
		default:
			break;
	}
	sysmng_update(update);
}

static void setvalue(FormType* frm, UINT16 id, UINT8 cfg) {
	ARM_CtlSetValue(ARM_FrmGetObjectPtr(frm, ARM_FrmGetObjectIndex(frm, id)), cfg);
}

static void setlabel(FormType* frm, UINT16 listid, UINT16 lableid, UINT8 cfg) {

	ListType*		list;
	ControlType*	ctrl = ARM_FrmGetObjectPtr(frm, ARM_FrmGetObjectIndex(frm, lableid));

	if (cfg < 12) {
		list = ARM_FrmGetObjectPtr(frm, ARM_FrmGetObjectIndex(frm, listid));
		ARM_CtlSetLabel(ctrl, ARM_LstGetSelectionText(list, cfg));
	} else {
		ARM_CtlSetLabel(ctrl, "N/A");
	}
}

void FormInit(void) {

	FormType*	frm = ARM_FrmGetActiveForm();
	
	setvalue(frm, IDC_NOWAIT, xmiloscfg.NOWAIT);
	setvalue(frm, IDC_CAPS, keystat.hit[CAPS_CODE]);
	setvalue(frm, IDC_KANA, keystat.hit[KANA_CODE]);
	setvalue(frm, IDC_SHIFT, keystat.hit[SHIFT_CODE]);
	setvalue(frm, IDC_CTRL, keystat.hit[CTRL_CODE]);
	setvalue(frm, IDC_GRAPH, keystat.hit[GRAPH_CODE]);
	setlabel(frm, IDC_KEYLIST1, IDC_HARDKEY1, xmiloscfg.HARDKEY1);
	setlabel(frm, IDC_KEYLIST2, IDC_HARDKEY2, xmiloscfg.HARDKEY2);
	setlabel(frm, IDC_KEYLIST3, IDC_HARDKEY3, xmiloscfg.HARDKEY3);
	setlabel(frm, IDC_KEYLIST4, IDC_HARDKEY4, xmiloscfg.HARDKEY4);

	sysmng_updatecaption(3);
	ARM_FrmDrawForm(frm);
}

static const UINT hardkey[] = {0, 0,0x20,0x0d,0x1b,0x8a,0x8b,0x91,0x92,0x93,0x94,0x95};

void HandleMenuChoice(UINT wParam) {

	UINT	update;

	update = 0;
	switch(wParam) {
		case IDM_ABOUT:
			AboutDialogProc();
			break;

		case IDM_IPLRESET:
			pccore_reset();
			break;

		case IDM_NMIRESET:
			Z80_NMI();
			break;

		case IDM_CONFIG:
			ConfigDialogProc();
			break;

		case IDM_NEWDISK:
			dialog_newdisk();
			break;

		case IDM_EXIT:
			xmilrunning = FALSE;
			break;

		case IDM_FDD0OPEN:
			dialog_changefdd(0);
			break;

		case IDM_FDD0EJECT:
			diskdrv_setfdd(0, NULL, 0);
			break;

		case IDM_FDD1OPEN:
			dialog_changefdd(1);
			break;

		case IDM_FDD1EJECT:
			diskdrv_setfdd(1, NULL, 0);
			break;

		case IDM_FDD2OPEN:
			dialog_changefdd(2);
			break;

		case IDM_FDD2EJECT:
			diskdrv_setfdd(2, NULL, 0);
			break;

		case IDM_FDD3OPEN:
			dialog_changefdd(3);
			break;

		case IDM_FDD3EJECT:
			diskdrv_setfdd(3, NULL, 0);
			break;

		case IDM_WIDTH80:
			crtc_forcesetwidth(80);
			break;

		case IDM_WIDTH40:
			crtc_forcesetwidth(40);
			break;
#if 0
		case IDM_DISPSYNC:
			xmilcfg.DISPSYNC = (xmilcfg.DISPSYNC ^ 1) & 1;
			update = SYS_UPDATECFG;
			break;

		case IDM_RASTER:
			xmilcfg.RASTER = (xmilcfg.RASTER ^ 1) & 1;
			update = SYS_UPDATECFG;
			break;
#endif
		case IDM_NOWAIT:
			xmiloscfg.NOWAIT = (xmiloscfg.NOWAIT ^ 1) & 1;
			update = SYS_UPDATEOSCFG;
			break;

		case IDM_AUTOFPS:
			xmiloscfg.DRAW_SKIP = 0;
			update = SYS_UPDATEOSCFG;
			break;

		case IDM_60FPS:
			xmiloscfg.DRAW_SKIP = 1;
			update = SYS_UPDATEOSCFG;
			break;

		case IDM_30FPS:
			xmiloscfg.DRAW_SKIP = 2;
			update = SYS_UPDATEOSCFG;
			break;

		case IDM_20FPS:
			xmiloscfg.DRAW_SKIP = 3;
			update = SYS_UPDATEOSCFG;
			break;

		case IDM_15FPS:
			xmiloscfg.DRAW_SKIP = 4;
			update = SYS_UPDATEOSCFG;
			break;
#if 0
		case IDM_MOUSE:
			menu_setmouse(xmilcfg.MOUSE_SW ^ 1);
			update = SYS_UPDATECFG;
			break;

		case IDM_SEEKSND:
			menu_setmotorflg(xmilcfg.MOTOR ^ 1);
			update = SYS_UPDATECFG;
			break;
#endif
		case IDM_BMPSAVE:
			dialog_writebmp();
			break;

		case IDM_DISPCLOCK:
			xmiloscfg.DISPCLK = (xmiloscfg.DISPCLK ^ 1) & 3;
			update = SYS_UPDATEOSCFG;
			sysmng_workclockrenewal();
			sysmng_updatecaption(2);
			break;

		case IDM_DISPFRAME:
			xmiloscfg.DISPCLK = (xmiloscfg.DISPCLK ^ 2) & 3;
			update = SYS_UPDATEOSCFG;
			sysmng_workclockrenewal();
			sysmng_updatecaption(2);
			break;

		case IDM_DIPSW:
			DipswDialogProc();
			break;
			
		case IDM_OPTION:
			OptionDialogProc();
			break;
			
		case IDM_SOUNDOPT:
			SoundDialogProc();
			break;
			
		case IDM_SCRNOPT:
			ScrnOptionDialogProc();
			break;
#if 0
		case IDM_Z80SAVE:
			debugsub_status();
			break;
#endif
		default:
			return;
	}
	sysmng_update(update);
	inmenu = FALSE;
	scrndraw_redraw();
	soundmng_play();
}

static int ApplicationHandleEvent(EventPtr eventP) {

    int		handled = 0;
	UINT32*	idp = (UINT32*)eventP;
	UINT16  topdata = ByteSwap16(*(idp+2) & 0xffff);
	UINT	keycode;
	UINT32	keystate, rockerstate, updownstate, navstate;

	keystate = ARM_KeyCurrentState();
	rockerstate = (keystate & 0x001f0000) >> 16;
	updownstate = (keystate & (keyBitPageUp | keyBitPageDown)) >> 1;
	navstate = (keystate & keyBitNavLRS) >> 22;
	joymng_5way(rockerstate | updownstate | navstate);
	joymng_hardkey(keystate);

	
	switch (ByteSwap16(eventP->eType)) {
		case keyDownEvent:
			if (inmenu) break;
			switch (topdata) {
				case vchrNavChange:
				case vchrPageUp:
				case vchrPageDown:
				case vchrRockerLeft:
				case vchrRockerRight:
				case vchrHard1:
				case vchrHard2:
				case vchrHard3:
				case vchrHard4:
					handled = 1;
					break;
				default:
					if (!(topdata & 0xff00)) {
						keycode =  topdata & 0x7f;
						palmkbd_keydown(keycode, TRUE);
						handled = 1;
					}
			}
			break;
		case keyUpEvent:
			if (inmenu) break;
			if (!(topdata & 0xff00)) {
				keycode =  topdata & 0x7f;
				palmkbd_keyup(keycode);
				handled = 1;
			}
			break;
		case menuEvent:
			HandleMenuChoice(topdata);
			handled = 1;
			break;
		case appStopEvent:
			xmilrunning = FALSE;
			handled = 1;
			break;
		case penUpEvent:
#if 0
			{
				UINT16	x = ByteSwap16(*(idp+1));
				UINT16	y = ByteSwap32(*(idp+1)) & 0xffff;

				if (y < 14+100){
					menuout = true;
				}
				break;
			}
#endif
		case penMoveEvent:
		case penDownEvent:
			if (xmiloscfg.graffiti) {
				handled = -1;
			}
			break;
				
 		default:
			break;
	}
    return(handled);
}

#ifdef SUPPORT_RESUME
// ----

static void getstatfilename(char *path, const char *ext, int size) {

	file_cpyname(path, file_getcd(xmilapp), size);
	file_catname(path, str_dot, size);
	file_catname(path, ext, size);
}

static void flagsave(const char *ext) {

	char	path[MAX_PATH];

	getstatfilename(path, ext, sizeof(path));
	statsave_save(path);
}

static void flagdelete(const char *ext) {

	char	path[MAX_PATH];

	getstatfilename(path, ext, sizeof(path));
	file_delete(path);
}

static int flagload(const char *ext) {

	int		ret;
	char	path[MAX_PATH];
	char	buf[1024];
	int		r;

	ret = IDC_OK;
	getstatfilename(path, ext, sizeof(path));
	r = statsave_check(path, buf, sizeof(buf));
	if (r & (~STATFLAG_DISKCHG)) {
		ResumeErrorDialogProc();
		ret = IDC_CANCEL;
	}
	else if (r & STATFLAG_DISKCHG) {
		ret = ResumeWarningDialogProc(buf);
		ret = IDC_CANCEL;
	}
	if (ret == IDC_OK) {
		statsave_load(path);
	}
	return(ret);
}
#endif


// ----

static	UINT	framecnt = 0;
static	UINT	waitcnt = 0;
static	UINT	framemax = 1;

static void framereset(void) {

	framecnt = 0;
	if (xmiloscfg.DISPCLK & 3) {
		if (sysmng_workclockrenewal()) {
			sysmng_updatecaption(2);
		}
	}
}

static void processwait(UINT waitcnt) {

	if (timing_getcount() >= waitcnt) {
		timing_setcount(0);
		framereset();
	}
}

static void exec1frame(void) {

	palmkbd_callback();
	joymng_callback();
	pccore_exec(framecnt == 0);
	framecnt++;
}


int x1main(int argc, char *argv[]) {

	EventType   event;
	UINT16		error;
	UINT32		wait = 0;
	int			result;

	show_progress();
	dosio_init();
	file_setcd(target);
	palmossub_init();
	initload();
	TRACEINIT();
	scrnmng_initialize();
	scrndraw_initialize();
	if (scrnmng_create(0) != SUCCESS) {
		TRACETERM();
		palmossub_term();
		dosio_term();
		return(2);
	}

	sysmng_initialize();
	joymng_initialize();
	palmkbd_initialize();

#if 0
	mousemng_initialize();
	if (np2oscfg.MOUSE_SW) {
		mousemng_enable(MOUSEPROC_SYSTEM);
	}
#endif

	pccore_initialize();
	pccore_reset();
#ifdef SUPPORT_RESUME
	if (xmiloscfg.resume) {
		flagload(xmilresume);
	}
#endif

	FormInit();
	erase_progress();
	scrndraw_redraw();
	
	xmilrunning = TRUE;
	while(xmilrunning) {
		if (inmenu) {
			wait = evtWaitForever;
		}
		else {
			int	count;
			wait = 0;
			for (count = 0;count < MAINLOOPWAIT;count++) {
			if (xmiloscfg.NOWAIT) {
				exec1frame();
				if (xmiloscfg.DRAW_SKIP) {			// nowait frame skip
					if (framecnt >= xmiloscfg.DRAW_SKIP) {
						processwait(0);
					}
				}
				else {								// nowait auto skip
					if (timing_getcount()) {
						processwait(0);
					}
				}
			}
			else if (xmiloscfg.DRAW_SKIP) {			// frame skip
				if (framecnt < xmiloscfg.DRAW_SKIP) {
					exec1frame();
				}
				else {
					processwait(xmiloscfg.DRAW_SKIP);
				}
			}
			else {								// auto skip
				if (!waitcnt) {
					UINT cnt;
					exec1frame();
					cnt = timing_getcount();
					if (framecnt > cnt) {
						waitcnt = framecnt;
						if (framemax > 1) {
							framemax--;
						}
					}
					else if (framecnt >= framemax) {
						if (framemax < 12) {
							framemax++;
						}
						if (cnt >= 12) {
							timing_reset();
						}
						else {
							timing_setcount(cnt - framecnt);
						}
						framereset();
					}
				}
				else {
					processwait(waitcnt);
					waitcnt = framecnt;
				}
			}
			}
		}
		ARM_EvtGetEvent(&event, wait);
		result = ApplicationHandleEvent(&event);
		if (result == 1) {
			continue;
		}
		if (result == 0) {
			if (ARM_SysHandleEvent(&event)) {
				continue;
			}
		}
		if (ARM_MenuHandleEvent(NULL, &event, &error)) {
			inmenu = TRUE;
			soundmng_stop();
		} else {
			ARM_FrmDispatchEvent(&event);
		}
	}

	xmilrunning = FALSE;
	show_progress();
	soundmng_stop();
#ifdef SUPPORT_RESUME
	if (xmiloscfg.resume) {
		flagsave(xmilresume);
	}
	else {
		flagdelete(xmilresume);
	}
#endif
	mousemng_disable(MOUSEPROC_SYSTEM);
	x1f_close();
	pccore_deinitialize();
	scrnmng_destroy();
	soundmng_destroy();
	if (sys_updates	& (SYS_UPDATECFG | SYS_UPDATEOSCFG)) {
		initsave();
	}
	TRACETERM();
	palmossub_term();
	dosio_term();
	erase_progress();

	(void)argc;
	(void)argv;
	return(0);
}
