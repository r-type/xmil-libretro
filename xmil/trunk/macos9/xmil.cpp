#include	"compiler.h"
#include	"resource.h"
#include	"strres.h"
#include	"xmil.h"
#include	"dosio.h"
#include	"mousemng.h"
#include	"scrnmng.h"
#include	"soundmng.h"
#include	"sysmng.h"
#include	"taskmng.h"
#include	"mackbd.h"
#include	"menu.h"
#include	"ini.h"
#include	"dialog.h"
#include	"z80core.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"timing.h"
#include	"keystat.h"
#if defined(SUPPORT_RESUME) || defined(SUPPORT_STATSAVE)
#include	"statsave.h"
#endif
#include	"debugsub.h"
#include	"scrndraw.h"
#include	"makescrn.h"
#include	"diskdrv.h"
#include	"fdd_ini.h"
#include	"x1f.h"


// #define	USE_RESUME


		XMILOSCFG	xmiloscfg = {	100, 100,
									0, 0, 0,
									0, 0,
#if defined(SUPPORT_RESUME)
									0,
#endif
#if defined(SUPPORT_STATSAVE)
									1,
#endif
									0};

		WindowPtr	hWndMain;
		BRESULT		xmilrunning;
static	REG8		scrnmode;


#define DRAG_THRESHOLD		5

#ifndef NP2GCC
#define	DEFAULTPATH		":"
#else
#define	DEFAULTPATH		"::::"
#endif
static	char	target[MAX_PATH] = DEFAULTPATH;


static const char xmilapp[] = "xmil";


// ---- stat save...

#if defined(SUPPORT_RESUME)
static const char xmilresumeext[] = ".sav";
#endif
#if defined(SUPPORT_STATSAVE)
static const char xmilflagext[] = ".sv%u";
#endif

#if defined(SUPPORT_RESUME) || defined(SUPPORT_STATSAVE)
static void getstatfilename(char *path, const char *ext, UINT size) {

	file_cpyname(path, file_getcd(xmilapp), size);
	file_catname(path, ext, size);
}

static void flagsave(const char *ext) {

	char	path[MAX_PATH];

	getstatfilename(path, ext, NELEMENTS(path));
	statsave_save(path);
}

static void flagdelete(const char *ext) {

	char	path[MAX_PATH];

	getstatfilename(path, ext, NELEMENTS(path));
	file_delete(path);
}

static int flagload(const char *ext, BRESULT force) {

	char	path[MAX_PATH];
	char	buf[1024];
	int		r;
	int		ret;

	getstatfilename(path, ext, NELEMENTS(path));
	r = statsave_check(path, buf, NELEMENTS(buf));
	if (r & (~STATFLAG_DISKCHG)) {
		ResumeErrorDialogProc();
		ret = IDCANCEL;
	}
	else if ((!force) && (r & STATFLAG_DISKCHG)) {
		ret = ResumeWarningDialogProc(buf);
	}
	else {
		ret = IDOK;
	}
	if (ret == IDOK) {
		statsave_load(path);
	}
	return(ret);
}
#endif


// ---- ‚¨‚Ü‚¶‚È‚¢

#if TARGET_CARBON
#define	AEProc(fn)	NewAEEventHandlerUPP((AEEventHandlerProcPtr)(fn))
#else
#define	AEProc(fn)	NewAEEventHandlerProc(fn)
#endif

#ifdef TARGET_API_MAC_CARBON
static pascal OSErr handleQuitApp(const AppleEvent *event, AppleEvent *reply,
															long refcon) {

	taskmng_exit();

	(void)event, (void)reply, (void)refcon;
	return(noErr);
}
#endif

static void InitToolBox(void) {

#if !TARGET_CARBON
	MaxApplZone();
	InitGraf(&qd.thePort);
	InitFonts();
	InitWindows();
	InitMenus();
	TEInit();
	InitDialogs(0);
#endif
	FlushEvents(everyEvent, 0);
	InitCursor();

#ifdef TARGET_API_MAC_CARBON
	AEInstallEventHandler(kCoreEventClass, kAEQuitApplication,
						AEProc(handleQuitApp), 0L, false);
#endif
}

static void MenuBarInit(void) {

	Handle		hdl;
	MenuHandle	hmenu;

	hdl = GetNewMBar(IDM_MAINMENU);
	if (hdl == NULL) {
		ExitToShell();
	}
	SetMenuBar(hdl);
	hmenu = GetMenuHandle(IDM_APPLE);
	if (hmenu) {
		AppendResMenu(hmenu, 'DRVR');
	}

	if (!xmiloscfg.Z80SAVE) {
		hmenu = GetMenuHandle(IDM_OTHER);
		if (hmenu) {
			DeleteMenuItem(hmenu, 7);
		}
	}

#if TARGET_API_MAC_CARBON
	hmenu = GetMenuHandle(IDM_FDD1);
	SetItemCmd(hmenu, LoWord(IDM_FDD1OPEN), 'D');
	SetMenuItemModifiers(hmenu, LoWord(IDM_FDD1OPEN), kMenuShiftModifier);
	SetItemCmd(hmenu, LoWord(IDM_FDD1EJECT), 'E');
	SetMenuItemModifiers(hmenu, LoWord(IDM_FDD1EJECT), kMenuShiftModifier);
#else
	EnableItem(GetMenuHandle(IDM_DEVICE), LoWord(IDM_MOUSE));
#endif

	if (!(xmilcfg.fddequip & (1 << 3))) {
		DeleteMenu(IDM_FDD3);
	}
	if (!(xmilcfg.fddequip & (1 << 2))) {
		DeleteMenu(IDM_FDD2);
	}
	if (!(xmilcfg.fddequip & (1 << 1))) {
		DeleteMenu(IDM_FDD1);
	}
	if (!(xmilcfg.fddequip & (1 << 0))) {
		DeleteMenu(IDM_FDD0);
	}

#if defined(SUPPORT_STATSAVE)
	if (!xmiloscfg.statsave) {
#endif
		DeleteMenu(IDM_STATSAVE);
#if defined(SUPPORT_STATSAVE)
	}
#endif

	DrawMenuBar();
}

static void HandleMenuChoice(long wParam) {

	UINT	update;
	Str255	applname;
#if defined(SUPPORT_STATSAVE)
	UINT	num;
	char	ext[16];
#endif

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

		case IDM_TURBOZ:
			menu_setiplrom(3);
			update = SYS_UPDATECFG;
			break;

		case IDM_TURBO:
			menu_setiplrom(2);
			update = SYS_UPDATECFG;
			break;

		case IDM_X1ROM:
			menu_setiplrom(1);
			update = SYS_UPDATECFG;
			break;

		case IDM_BOOT2D:
			menu_setbootmedia(0);
			update = SYS_UPDATECFG;
			break;

		case IDM_BOOT2HD:
			menu_setbootmedia(DIPSW_BOOTMEDIA);
			update = SYS_UPDATECFG;
			break;

		case IDM_HIGHRES:
			menu_setresolute(0);
			update = SYS_UPDATECFG;
			break;

		case IDM_LOWRES:
			menu_setresolute(DIPSW_RESOLUTE);
			update = SYS_UPDATECFG;
			break;

		case IDM_WIDTH80:
			crtc_forcesetwidth(80);
			break;

		case IDM_WIDTH40:
			crtc_forcesetwidth(40);
			break;

		case IDM_DISPSYNC:
			menu_setdispmode(xmilcfg.DISPSYNC ^ 1);
			break;

//		case IDM_RASTER:
//			menu_setraster(xmilcfg.RASTER ^ 1);
//			break;

		case IDM_NOWAIT:
			menu_setwaitflg(xmiloscfg.NOWAIT ^ 1);
			update = SYS_UPDATECFG;
			break;

		case IDM_AUTOFPS:
			menu_setframe(0);
			update = SYS_UPDATECFG;
			break;

		case IDM_60FPS:
			menu_setframe(1);
			update = SYS_UPDATECFG;
			break;

		case IDM_30FPS:
			menu_setframe(2);
			update = SYS_UPDATECFG;
			break;

		case IDM_20FPS:
			menu_setframe(3);
			update = SYS_UPDATECFG;
			break;

		case IDM_15FPS:
			menu_setframe(4);
			update = SYS_UPDATECFG;
			break;

		case IDM_KEY:
			menu_setkey(0);
//			keystat_resetjoykey();
			update = SYS_UPDATECFG;
			break;

		case IDM_JOY1:
			menu_setkey(1);
//			keystat_resetjoykey();
			update = SYS_UPDATECFG;
			break;

		case IDM_JOY2:
			menu_setkey(2);
//			keystat_resetjoykey();
			update = SYS_UPDATECFG;
			break;

//		case IDM_MOUSEKEY:
//			menu_setkey(3);
//			keystat_resetjoykey();
//			update = SYS_UPDATECFG;
//			break;

		case IDM_FMBOARD:
			menu_setsound(xmilcfg.SOUND_SW ^ 1);
			update = SYS_UPDATECFG;
			break;

		case IDM_MOUSE:
			menu_setmouse(xmilcfg.MOUSE_SW ^ 1);
			update = SYS_UPDATECFG;
			break;

		case IDM_SEEKSND:
			menu_setmotorflg(xmilcfg.MOTOR ^ 1);
			update = SYS_UPDATECFG;
			break;

//		case IDM_BMPSAVE:
//			dialog_writebmp();
//			break;

		case IDM_DISPCLOCK:
			menu_setdispclk(xmiloscfg.DISPCLK ^ 1);
			update = SYS_UPDATECFG;
			break;

		case IDM_DISPFRAME:
			menu_setdispclk(xmiloscfg.DISPCLK ^ 2);
			update = SYS_UPDATECFG;
			break;

		case IDM_JOYX:
			menu_setbtnmode(xmilcfg.BTN_MODE ^ 1);
			update = SYS_UPDATECFG;
			break;

		case IDM_RAPID:
			menu_setbtnrapid(xmilcfg.BTN_RAPID ^ 1);
			update = SYS_UPDATECFG;
			break;

		case IDM_Z80SAVE:
			debugsub_status();
			break;

		default:
			if (HiWord(wParam) == IDM_APPLE) {
				GetMenuItemText(GetMenuHandle(IDM_APPLE), 
											LoWord(wParam), applname);
#if !TARGET_API_MAC_CARBON
				(void)OpenDeskAcc(applname);
#endif
			}
#if defined(SUPPORT_STATSAVE)
			else if (HiWord(wParam) == IDM_STATSAVE) {
				num = LoWord(wParam);
				if ((num >= 1) && (num < (1 + 10))) {
					OEMSPRINTF(ext, xmilflagext, num - 1);
					flagsave(ext);
				}
				else if ((num >= 12) && (num < (12 + 10))) {
					OEMSPRINTF(ext, xmilflagext, num - 12);
					flagload(ext, TRUE);
				}
			}
#endif
			break;
	}
	sysmng_update(update);
	HiliteMenu(0);
}

static void HandleUpdateEvent(EventRecord *pevent) {

	WindowPtr	hWnd;

	hWnd = (WindowPtr)pevent->message;
	BeginUpdate(hWnd);
	if (xmilrunning) {
		scrndraw_redraw();
	}
	else {
//		np2open();
	}
	EndUpdate(hWnd);
}

static void HandleMouseDown(EventRecord *pevent) {

	WindowPtr	hWnd;
	Rect		rDrag;

	switch(FindWindow(pevent->where, &hWnd)) {
		case inMenuBar:
			if (xmilrunning) {
				soundmng_stop();
				mousemng_disable(MOUSEPROC_MACUI);
				HandleMenuChoice(MenuSelect(pevent->where));
				mousemng_enable(MOUSEPROC_MACUI);
				soundmng_play();
			}
			break;

		case inDrag:
#if TARGET_API_MAC_CARBON
		{
			BitMap	gscreenBits;
			GetQDGlobalsScreenBits(&gscreenBits);
			rDrag = gscreenBits.bounds;
			InsetRect(&rDrag, DRAG_THRESHOLD, DRAG_THRESHOLD);
			DragWindow(hWnd, pevent->where, &rDrag);
		}
#else
			rDrag = qd.screenBits.bounds;
			InsetRect(&rDrag, DRAG_THRESHOLD, DRAG_THRESHOLD);
			DragWindow(hWnd, pevent->where, &rDrag);
#endif
			break;

		case inContent:
			mousemng_buttonevent((pevent->modifiers & (1 << 12))
									?MOUSEMNG_RIGHTDOWN:MOUSEMNG_LEFTDOWN);
			break;

		case inGoAway:
			if (TrackGoAway(hWnd, pevent->where)) { }
			xmilrunning = FALSE;
			break;
	}
}

static void eventproc(EventRecord *event) {

	int		keycode;

	keycode = (event->message & keyCodeMask) >> 8;
	switch(event->what) {
		case mouseDown:
			HandleMouseDown(event);
			break;

		case updateEvt:
			HandleUpdateEvent(event);
			break;

		case keyDown:
		case autoKey:
			if (!xmilrunning) {
				break;
			}
#if !TARGET_API_MAC_CARBON
			if ((keycode == 0x6f) && (xmiloscfg.F12KEY == 0)) {
				HandleMenuChoice(IDM_MOUSE);
				break;
			}
#endif
			if (event->modifiers & cmdKey) {
#if !TARGET_API_MAC_CARBON
				if (mackbd_keydown(keycode, TRUE)) {
					break;
				}
#endif
				soundmng_stop();
				mousemng_disable(MOUSEPROC_MACUI);
#if TARGET_API_MAC_CARBON
				HandleMenuChoice(MenuEvent(event));
#else
				HandleMenuChoice(MenuKey(event->message & charCodeMask));
#endif
				mousemng_enable(MOUSEPROC_MACUI);
				soundmng_play();
			}
			else {
				mackbd_keydown(keycode, FALSE);
			}
			break;

		case keyUp:
			mackbd_keyup(keycode);
			break;

		case mouseUp:
			mousemng_buttonevent(MOUSEMNG_LEFTUP);
			mousemng_buttonevent(MOUSEMNG_RIGHTUP);
			break;

		case activateEvt:
			mackbd_activate((event->modifiers & activeFlag)?TRUE:FALSE);
			break;
	}
}


// ----

static	UINT	framecnt = 0;
static	UINT	waitcnt = 0;
static	UINT	framemax = 1;

static void framereset(void) {

	framecnt = 0;
	if (xmiloscfg.DISPCLK & 3) {
		if (sysmng_workclockrenewal()) {
			sysmng_updatecaption(3);
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

	mackbd_callback();
	mousemng_callback();
	pccore_exec(framecnt == 0);
	framecnt++;
}

int main(int argc, char *argv[]) {

	Rect		wRect;
	EventRecord	event;
	UINT		t;
	GrafPtr		saveport;
	Point		pt;

	dosio_init();
	file_setcd(target);

	InitToolBox();
	macossub_init();
	initload();

	MenuBarInit();

	TRACEINIT();

//	keystat_initialize();

	SetRect(&wRect, xmiloscfg.posx, xmiloscfg.posy, 100, 100);
	hWndMain = NewWindow(0, &wRect, "\pNekoe System", FALSE,
								noGrowDocProc, (WindowPtr)-1, TRUE, 0);
	if (!hWndMain) {
		TRACETERM();
		macossub_term();
		dosio_term();
		return(0);
	}
	scrnmng_initialize();
	SizeWindow(hWndMain, 640, 400, TRUE);
	ShowWindow(hWndMain);

	menu_setiplrom(xmilcfg.ROM_TYPE);
	menu_setbootmedia(xmilcfg.DIP_SW);
	menu_setresolute(xmilcfg.DIP_SW);
	menu_setdispmode(xmilcfg.DISPSYNC);
	menu_setraster(xmilcfg.RASTER);
	menu_setwaitflg(xmiloscfg.NOWAIT);
	menu_setframe(xmiloscfg.DRAW_SKIP);
	menu_setkey(0);
	menu_setsound(xmilcfg.SOUND_SW);
	menu_setmouse(xmilcfg.MOUSE_SW);
	menu_setmotorflg(xmilcfg.MOTOR);
	menu_setdispclk(xmiloscfg.DISPCLK);
	menu_setbtnmode(xmilcfg.BTN_MODE);
	menu_setbtnrapid(xmilcfg.BTN_RAPID);

	scrndraw_initialize();
	scrnmode = 0;
	if (scrnmng_create(scrnmode) != SUCCESS) {
		TRACETERM();
		macossub_term();
		dosio_term();
		DisposeWindow(hWndMain);
		return(0);
	}

//	np2open();
	t = GETTICK();
	while((GETTICK() - t) < 100) {
		if (WaitNextEvent(everyEvent, &event, 0, 0)) {
			eventproc(&event);
		}
	}

	sysmng_initialize();
//	joymng_initialize();
	mackbd_initialize();

	mousemng_initialize();
//	if (np2oscfg.MOUSE_SW) {
//		mousemng_enable(MOUSEPROC_SYSTEM);
//	}

	pccore_initialize();
	pccore_reset();

#if defined(SUPPORT_RESUME)
	if (xmiloscfg.resume) {
		flagload(xmilresumeext, FALSE);
	}
#endif

	scrndraw_redraw();

	SetEventMask(everyEvent);

	xmilrunning = TRUE;
	while(xmilrunning) {
		if (WaitNextEvent(everyEvent, &event, 0, 0)) {
			eventproc(&event);
		}
		else {
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

	GetPort(&saveport);
#if TARGET_API_MAC_CARBON
	SetPortWindowPort(hWndMain);
#else
	SetPort(hWndMain);
#endif
	pt.h = 0;
	pt.v = 0;
	LocalToGlobal(&pt);
	SetPort(saveport);
	if ((xmiloscfg.posx != pt.h) || (xmiloscfg.posy != pt.v)) {
		xmiloscfg.posx = pt.h;
		xmiloscfg.posy = pt.v;
		sysmng_update(SYS_UPDATEOSCFG);
	}

	xmilrunning = FALSE;

#if defined(SUPPORT_RESUME)
	if (xmiloscfg.resume) {
		flagsave(xmilresumeext);
	}
	else {
		flagdelete(xmilresumeext);
	}
#endif

	mousemng_disable(MOUSEPROC_SYSTEM);

	x1f_close();
	pccore_deinitialize();
//	soundmng_deinitialize();
	scrnmng_destroy();

	if (sys_updates	& (SYS_UPDATECFG | SYS_UPDATEOSCFG)) {
		initsave();
	}
	TRACETERM();
	macossub_term();
	dosio_term();

	DisposeWindow(hWndMain);

	(void)argc;
	(void)argv;
	return(0);
}

