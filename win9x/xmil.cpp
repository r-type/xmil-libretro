#include	"compiler.h"
#ifndef __GNUC__
#include	<winnls32.h>
#endif
#include	"resource.h"
#include	"xmil.h"
#include	"dosio.h"
#include	"joymng.h"
#include	"mousemng.h"
#include	"scrnmng.h"
#include	"soundmng.h"
#include	"sysmng.h"
#include	"ddrawbmp.h"
#include	"winloc.h"
#include	"dclock.h"
#include	"winkbd.h"
#include	"menu.h"
#include	"ini.h"
#include	"juliet.h"
#include	"extclass.h"
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


static const OEMCHAR szClassName[] = OEMTEXT("Xmil-MainWindow");

		XMILOSCFG	xmiloscfg = {
							CW_USEDEFAULT, CW_USEDEFAULT,
							1, 0, 0, 0, 1,
							0, 0, 0,
#if defined(SUPPORT_RESUME)
							0,
#endif
#if defined(SUPPORT_STATSAVE)
							1,
#endif
							0, 0, 0xffffff, 0xffbf6a};

		OEMCHAR		szProgName[] = OEMTEXT("X millennium ÇÀÇ±ÇøÇ„Å`ÇÒ");
		HWND		hWndMain;
		HINSTANCE	hInst;
		HINSTANCE	hPreI;
		OEMCHAR		modulefile[MAX_PATH];
		OEMCHAR		fddfolder[MAX_PATH];
		OEMCHAR		bmpfilefolder[MAX_PATH];

static	BRESULT		xmilstopemulate = FALSE;
		UINT8		xmilopening = 1;


static void wincentering(HWND hWnd) {

	RECT	rc;
	int		width;
	int		height;

	GetWindowRect(hWnd, &rc);
	width = rc.right - rc.left;
	height = rc.bottom - rc.top;
	xmiloscfg.winx = (GetSystemMetrics(SM_CXSCREEN) - width) / 2;
	xmiloscfg.winy = (GetSystemMetrics(SM_CYSCREEN) - height) / 2;
	if (xmiloscfg.winx < 0) {
		xmiloscfg.winx = 0;
	}
	if (xmiloscfg.winy < 0) {
		xmiloscfg.winy = 0;
	}
	sysmng_update(SYS_UPDATEOSCFG);
	MoveWindow(hWnd, xmiloscfg.winx, xmiloscfg.winy, width, height, TRUE);
}


// ----

static void winuienter(void) {

//	winui_en = TRUE;
	soundmng_disable(SNDPROC_MAIN);
	scrnmng_topwinui();
}

static void winuileave(void) {

	scrnmng_clearwinui();
	soundmng_enable(SNDPROC_MAIN);
//	winui_en = FALSE;
}

static void dispbmp(HINSTANCE hinst, HDC hdc,
										const OEMCHAR *res, int x, int y) {

	HBITMAP		hbmp;
	BITMAP		bmp;
	HDC			hmdc;

	hbmp = LoadBitmap(hinst, res);
	GetObject(hbmp, sizeof(bmp), &bmp);
	hmdc = CreateCompatibleDC(hdc);
	SelectObject(hmdc, hbmp);
	BitBlt(hdc, x, y, bmp.bmWidth, bmp.bmHeight, hmdc, 0, 0, SRCCOPY);
	DeleteDC(hmdc);
	DeleteObject(hbmp);
}


// ----

#if defined(SUPPORT_RESUME) || defined(SUPPORT_STATSAVE)
static const OEMCHAR xmilresumeext[] = OEMTEXT(".sav");
#endif
#if defined(SUPPORT_STATSAVE)
static const OEMCHAR xmilflagext[] = OEMTEXT(".sv%u");
#endif

#if defined(SUPPORT_RESUME) || defined(SUPPORT_STATSAVE)

static void getstatfilename(OEMCHAR *path, const OEMCHAR *ext, UINT size) {

	file_cpyname(path, modulefile, size);
	file_cutext(path);
	file_catname(path, ext, size);
}

static int flagsave(const OEMCHAR *ext) {

	OEMCHAR	path[MAX_PATH];
	int		ret;

	getstatfilename(path, ext, NELEMENTS(path));
	soundmng_stop();
	ret = statsave_save(path);
	if (ret) {
		file_delete(path);
	}
	soundmng_play();
	return(ret);
}

static void flagdelete(const OEMCHAR *ext) {

	OEMCHAR	path[MAX_PATH];

	getstatfilename(path, ext, NELEMENTS(path));
	file_delete(path);
}

static int flagload(const char *ext, const char *title, BOOL force) {

	int		ret;
	int		id;
	OEMCHAR	path[MAX_PATH];
	OEMCHAR	buf[1024];

	getstatfilename(path, ext, NELEMENTS(path));
	winuienter();
	id = IDYES;
	ret = statsave_check(path, buf, NELEMENTS(buf));
	if (ret & (~STATFLAG_DISKCHG)) {
		MessageBox(hWndMain, "Couldn't restart", title, MB_OK | MB_ICONSTOP);
		id = IDNO;
	}
	else if ((!force) && (ret & STATFLAG_DISKCHG)) {
		char buf2[1024 + 256];
		wsprintf(buf2, "Conflict!\n\n%s\nContinue?", buf);
		id = MessageBox(hWndMain, buf2, title,
										MB_YESNOCANCEL | MB_ICONQUESTION);
	}
	if (id == IDYES) {
		statsave_load(path);
//		toolwin_setfdd(0, fdd_diskname(0));
//		toolwin_setfdd(1, fdd_diskname(1));
	}
	sysmng_workclockreset();
	sysmng_updatecaption(1);
	winuileave();
	return(id);
}
#endif


// ----

static void xmilcmd(HWND hWnd, UINT cmd) {

	UINT	update;
	OEMCHAR ext[16];

	update = 0;
	switch(cmd) {
		case IDM_IPLRESET:
			pccore_reset();
			break;

		case IDM_NMIRESET:
			Z80_NMI();
			break;

		case IDM_CONFIG:
			winuienter();
			DialogBox(hInst, MAKEINTRESOURCE(IDD_CONFIG),
											hWnd, (DLGPROC)CfgDialogProc);
			winuileave();
			break;

		case IDM_NEWDISK:
			winuienter();
			dialog_newdisk(hWnd);
			winuileave();
			break;

		case IDM_EXIT:
			SendMessage(hWnd, WM_CLOSE, 0, 0L);
			break;

		case IDM_FDD0OPEN:
			winuienter();
			dialog_changefdd(hWnd, 0);
			winuileave();
			break;

		case IDM_FDD0EJECT:
			diskdrv_setfdd(0, NULL, 0);
			break;

		case IDM_FDD1OPEN:
			winuienter();
			dialog_changefdd(hWnd, 1);
			winuileave();
			break;

		case IDM_FDD1EJECT:
			diskdrv_setfdd(1, NULL, 0);
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

		case IDM_WINDOW:
			scrnmng_changescreen(scrnmode & (~SCRNMODE_FULLSCREEN));
			break;

		case IDM_FULLSCREEN:
			scrnmng_changescreen(scrnmode | SCRNMODE_FULLSCREEN);
			break;

		case IDM_WIDTH80:
			crtc_forcesetwidth(80);
			break;

		case IDM_WIDTH40:
			crtc_forcesetwidth(40);
			break;

		case IDM_DISPSYNC:
			menu_setdispmode(xmilcfg.DISPSYNC ^ 1);
			update = SYS_UPDATECFG;
			break;

		case IDM_RASTER:
			menu_setraster(xmilcfg.RASTER ^ 1);
			update = SYS_UPDATECFG;
			break;

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
			break;

		case IDM_JOY1:
			menu_setkey(1);
			break;

		case IDM_JOY2:
			menu_setkey(2);
			break;

		case IDM_FMBOARD:
			menu_setsound(xmilcfg.SOUND_SW ^ 1);
			update = SYS_UPDATECFG;
			break;

		case IDM_JOYSTICK:
			menu_setjoystick(xmiloscfg.JOYSTICK ^ 1);
			update = SYS_UPDATECFG;
			break;

		case IDM_MOUSE:
		//	mousemng_toggle(MOUSEPROC_SYSTEM);
			menu_setmouse(xmilcfg.MOUSE_SW ^ 1);
			update = SYS_UPDATECFG;
			break;

		case IDM_SEEKSND:
			menu_setmotorflg(xmilcfg.MOTOR ^ 1);
			update = SYS_UPDATECFG;
			break;

		case IDM_BMPSAVE:
			winuienter();
			bmpsave();
			winuileave();
			break;

		case IDM_OPMLOG:
			winuienter();
			dialog_x1f(hWnd);
			winuileave();
			break;

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

		case IDM_ABOUT:
			winuienter();
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUT),
											hWnd, (DLGPROC)AboutDialogProc);
			winuileave();
			break;

		default:
#if defined(SUPPORT_STATSAVE)
			if ((cmd >= IDM_FLAGSAVE) &&
				(cmd < (IDM_FLAGSAVE + SUPPORT_STATSAVE))) {
				OEMSPRINTF(ext, xmilflagext, cmd - IDM_FLAGSAVE);
				flagsave(ext);
			}
			else if ((cmd >= IDM_FLAGLOAD) &&
				(cmd < (IDM_FLAGLOAD + SUPPORT_STATSAVE))) {
				OEMSPRINTF(ext, xmilflagext, cmd - IDM_FLAGLOAD);
				flagload(ext, "Status Load", TRUE);
			}
#endif
			break;
	}
	sysmng_update(update);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {

	PAINTSTRUCT	ps;
	RECT		rc;
	HDC			hdc;
	UINT		updateflag;

	switch (msg) {
		case WM_CREATE:
			extclass_wmcreate(hWnd);
#ifndef __GNUC__
			WINNLSEnableIME(hWnd, FALSE);
#endif
			break;

		case WM_SYSCOMMAND:
			updateflag = 0;
			switch(wParam) {
				case IDM_SCREENCENTER:
					if (!scrnmng_isfullscreen()) {
						wincentering(hWnd);
					}
					break;

				case IDM_SNAPENABLE:
					sysmenu_setwinsnap(xmiloscfg.WINSNAP ^ 1);
					updateflag = SYS_UPDATEOSCFG;
					break;

				case IDM_BACKGROUND:
					sysmenu_setbackground(xmiloscfg.background ^ 1);
					updateflag = SYS_UPDATEOSCFG;
					break;

				case IDM_BGSOUND:
					sysmenu_setbgsound(xmiloscfg.background ^ 2);
					updateflag = SYS_UPDATEOSCFG;
					break;

				default:
					return(DefWindowProc(hWnd, msg, wParam, lParam));
			}
			sysmng_update(updateflag);
			break;

		case WM_COMMAND:
			xmilcmd(hWnd, LOWORD(wParam));
			break;

		case WM_ACTIVATE:
			if (LOWORD(wParam) != WA_INACTIVE) {
				xmilstopemulate = FALSE;
				scrnmng_update();
				mousemng_enable(MOUSEPROC_BG);
				soundmng_enable(SNDPROC_MASTER);
			}
			else {
				mousemng_disable(MOUSEPROC_BG);
				xmilstopemulate = (xmiloscfg.background & 1)?TRUE:FALSE;
				if (xmiloscfg.background) {
					soundmng_disable(SNDPROC_MASTER);
				}
			}
			break;

		case WM_PAINT:
			hdc = BeginPaint(hWnd, &ps);
			if (xmilopening) {
				HINSTANCE	hinst;
				RECT		rect;
				int			width;
				int			height;
				HBRUSH		hbrush;

				hinst = (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE);
				GetClientRect(hWnd, &rect);
				width = rect.right - rect.left;
				height = rect.bottom - rect.top;
				hbrush = (HBRUSH)SelectObject(hdc,
												GetStockObject(BLACK_BRUSH));
				PatBlt(hdc, 0, 0, width, height, PATCOPY);
				SelectObject(hdc, hbrush);

				dispbmp(hinst, hdc, OEMTEXT("XMILBMP"),
							(width - 201) / 2, (height - 31) / 2);
				dispbmp(hinst, hdc, OEMTEXT("ZILOG"),
							width - (101 + 8 + 41 + 8 + 68 + 1), height - 38);

				dispbmp(hinst, hdc, OEMTEXT("TURBOZ"),
							width - (101 + 8 + 68 + 1), height - 18);

				dispbmp(hinst, hdc, OEMTEXT("OPMSOUND"),
							width - (68 + 1), height - (25 + 1));
			}
			else {
//				scrnmng_update();
				scrndraw_redraw();
			}
			EndPaint(hWnd, &ps);
			break;

		case WM_QUERYNEWPALETTE:
			scrnmng_querypalette();
			break;

		case WM_MOVE:
			if ((!scrnmng_isfullscreen()) &&
				(!(GetWindowLong(hWnd, GWL_STYLE) &
									(WS_MAXIMIZE | WS_MINIMIZE)))) {
				GetWindowRect(hWnd, &rc);
				xmiloscfg.winx = rc.left;
				xmiloscfg.winy = rc.top;
				sysmng_update(SYS_UPDATEOSCFG);
			}
			break;

		case WM_ENTERMENULOOP:
			winuienter();
			if (scrnmng_isfullscreen()) {
				DrawMenuBar(hWnd);
			}
			break;

		case WM_EXITMENULOOP:
			winuileave();
			break;

		case WM_ENTERSIZEMOVE:
			soundmng_disable(SNDPROC_MAIN);
			mousemng_disable(MOUSEPROC_WINUI);
			win_movingstart();
			break;

		case WM_EXITSIZEMOVE:
			mousemng_enable(MOUSEPROC_WINUI);
			soundmng_enable(SNDPROC_MAIN);
			break;

		case WM_MOVING:
			if (xmiloscfg.WINSNAP) {
				win_movingproc((RECT *)lParam);
			}
			break;

		case WM_KEYDOWN:
			if (wParam == VK_F12) {
				mousemng_toggle(MOUSEPROC_SYSTEM);
			//	menu_setmouse(np2oscfg.MOUSE_SW ^ 1);
			//	sysmng_update(SYS_UPDATECFG);
			}
			else {
				winkbd_keydown(wParam, lParam);
			}
			break;

		case WM_KEYUP:
			winkbd_keyup(wParam, lParam);
			break;

		case WM_SYSKEYDOWN:
			if (wParam == VK_F10) {
				return(DefWindowProc(hWnd, WM_SYSKEYDOWN, wParam, lParam));
			}
			winkbd_keydown(wParam, lParam);
			break;

		case WM_SYSKEYUP:
			if (wParam == VK_F10) {
				return(DefWindowProc(hWnd, WM_SYSKEYUP, wParam, lParam));
			}
			else {
				winkbd_keyup(wParam, lParam);
			}
			break;

		case WM_MOUSEMOVE:
			if (scrnmng_isfullscreen()) {
				POINT p;
				if (GetCursorPos(&p)) {
					scrnmng_fullscrnmenu(p.y);
				}
			}
			break;

		case WM_LBUTTONDOWN:
			if (!mousemng_buttonevent(MOUSEMNG_LEFTDOWN)) {
				return(DefWindowProc(hWnd, msg, wParam, lParam));
			}
			break;

		case WM_LBUTTONUP:
			if (!mousemng_buttonevent(MOUSEMNG_LEFTUP)) {
				if (scrnmng_isfullscreen()) {
					POINT p;
					if ((GetCursorPos(&p)) && (p.y >= 466)) {
						xmiloscfg.clockx++;
						dclock_reset();
						sysmng_update(SYS_UPDATEOSCFG);
					}
				}
				return(DefWindowProc(hWnd, msg, wParam, lParam));
			}
			break;

		case WM_RBUTTONDOWN:
			if (!mousemng_buttonevent(MOUSEMNG_RIGHTDOWN)) {
				return(DefWindowProc(hWnd, msg, wParam, lParam));
			}
			break;

		case WM_RBUTTONUP:
			if (!mousemng_buttonevent(MOUSEMNG_RIGHTUP)) {
				if (scrnmng_isfullscreen()) {
					POINT p;
					if ((GetCursorPos(&p)) && (p.y >= 466) &&
												(xmiloscfg.clockx)) {
						xmiloscfg.clockfnt++;
						dclock_reset();
						sysmng_update(SYS_UPDATEOSCFG);
					}
				}
				return(DefWindowProc(hWnd, msg, wParam, lParam));
			}
			break;

		case WM_MBUTTONDOWN:
			mousemng_toggle(MOUSEPROC_SYSTEM);
		//	menu_setmouse(np2oscfg.MOUSE_SW ^ 1);
		//	sysmng_update(SYS_UPDATECFG);
			break;

#if 0
		case WM_IME_NOTIFY:
			if (wParam != IMN_OPENSTATUSWINDOW) {
				return(DefWindowProc(hWnd, msg, wParam, lParam));
			}
			break;
#endif

#if 0
		case WM_DROPFILES:
			{
				HDROP		hDrop;
				char		DropFileName[MAX_PATH];
				int			uFileNo;
				int			i;

//				DSound_Stop();
				hDrop = (HDROP)wParam;
				uFileNo = DragQueryFile((HDROP)wParam, 0xFFFFFFFF, NULL, 0);
				for (i=0; i<uFileNo; i++) {
					DragQueryFile(hDrop, i, DropFileName, MAX_PATH);
					if (is_d8ufile(DropFileName)) {
						fdd_ini(DropFileName);
						sysmng_updatecaption(1);
						break;
					}
				}
				DragFinish(hDrop);
//				DSound_Play();
			}
			break;
#endif

		case WM_DESTROY:
			extclass_wmdestroy(hWnd);
			PostQuitMessage(0);
			break;

		default:
			return(DefWindowProc(hWnd, msg, wParam, lParam));
	}
	return(0L);
}


// ----

static	UINT	framecnt = 0;
static	UINT	waitcnt = 0;
static	UINT	framemax = 1;

static void framereset(UINT cnt) {

	framecnt = 0;
	scrnmng_dispclock();
//	kdispwin_draw((BYTE)cnt);
//	skbdwin_process();
//	mdbgwin_process();
//	toolwin_draw((BYTE)cnt);
//	viewer_allreload(FALSE);
	if (xmiloscfg.DISPCLK & 3) {
		if (sysmng_workclockrenewal()) {
			sysmng_updatecaption(3);
		}
	}
}

static void processwait(UINT cnt) {

	if (timing_getcount() >= cnt) {
		timing_setcount(0);
		framereset(cnt);
	}
	else {
		Sleep(1);
	}
	soundmng_sync();
}

static void exec1frame(void) {

	joymng_sync();
	mousemng_sync();
	pccore_exec(framecnt == 0);
	dclock_callback();
	framecnt++;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPreInst,
										LPSTR lpszCmdLine, int nCmdShow) {

	HWND		hWnd;
	WNDCLASS	wc;
	MSG			msg;

	GetModuleFileName(NULL, modulefile, sizeof(modulefile));
	dosio_init();
	file_setcd(modulefile);
	initload();

	hWnd = FindWindow(szClassName, NULL);
	if (hWnd != NULL) {
		ShowWindow(hWnd, SW_RESTORE);
		SetForegroundWindow(hWnd);
		dosio_term();
		return(FALSE);
	}

	hInst = hInstance;
	hPreI = hPreInst;
	TRACEINIT();

//	keystat_initialize();

	if (!hPreInst) {
		wc.style = CS_BYTEALIGNCLIENT | CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc = WndProc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = EXTGWL_SIZE;
		wc.hInstance = hInstance;
		wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
		wc.lpszMenuName = MAKEINTRESOURCE(IDM_MAIN);
		wc.lpszClassName = szClassName;
		if (!RegisterClass(&wc)) {
			return(FALSE);
		}
	}

	mousemng_initialize();

	hWnd = CreateWindowEx(0,
						szClassName, szProgName,
						WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION |
						WS_MINIMIZEBOX,
						xmiloscfg.winx, xmiloscfg.winy,
						SURFACE_WIDTH, SURFACE_HEIGHT,
						NULL, NULL, hInstance, NULL);
	hWndMain = hWnd;
	scrnmng_initialize();

	menu_setiplrom(xmilcfg.ROM_TYPE);
	menu_setbootmedia(xmilcfg.DIP_SW);
	menu_setresolute(xmilcfg.DIP_SW);
	menu_setdispmode(xmilcfg.DISPSYNC);
	menu_setraster(xmilcfg.RASTER);
	menu_setwaitflg(xmiloscfg.NOWAIT);
	menu_setframe(xmiloscfg.DRAW_SKIP);
	menu_setkey(0);
	menu_setsound(xmilcfg.SOUND_SW);
	menu_setjoystick(xmiloscfg.JOYSTICK);
	menu_setmouse(xmilcfg.MOUSE_SW);
	menu_setmotorflg(xmilcfg.MOTOR);
	menu_setdispclk(xmiloscfg.DISPCLK);
	menu_setbtnmode(xmilcfg.BTN_MODE);
	menu_setbtnrapid(xmilcfg.BTN_RAPID);

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	sysmenu_initialize();
	menu_initialize();
	DrawMenuBar(hWnd);

	sysmenu_setwinsnap(xmiloscfg.WINSNAP);
	sysmenu_setbackground(xmiloscfg.background);
	sysmenu_setbgsound(xmiloscfg.background);

	scrndraw_initialize();
	scrnmode = 0;
	if (scrnmng_create(scrnmode) != SUCCESS) {
		scrnmode ^= SCRNMODE_FULLSCREEN;
		if (scrnmng_create(scrnmode) != SUCCESS) {
			MessageBox(hWnd, OEMTEXT("Couldn't create DirectDraw Object"),
													szProgName, MB_OK);
			return(FALSE);
		}
	}

	juliet_load();
	juliet_prepare();
//	juliet2_reset();
	if (soundmng_initialize() == SUCCESS) {
		soundmng_pcmload(SOUND_PCMSEEK, OEMTEXT("fddseek.wav"), 0);
		soundmng_pcmload(SOUND_PCMSEEK1, OEMTEXT("fddseek1.wav"), 0);
		soundmng_pcmvolume(SOUND_PCMSEEK, xmilcfg.MOTORVOL);
		soundmng_pcmvolume(SOUND_PCMSEEK1, xmilcfg.MOTORVOL);
	}

	sysmng_initialize();
	joymng_initialize();

	pccore_initialize();
	pccore_reset();

#if !defined(UNICODE)
	if (__argc > 1) {
		for (int i=1; i<__argc; i++) {
			if (is_d8ufile(__argv[i])) {
				fdd_ini(__argv[i]);
				sysmng_updatecaption(1);
				break;
			}
		}
	}
#endif

	scrndraw_redraw();

	xmilopening = 0;

	while(1) {
		if (!xmilstopemulate) {
			if (PeekMessage(&msg, 0, 0, 0, PM_NOREMOVE)) {
				if (!GetMessage(&msg, NULL, 0, 0)) {
					break;
				}
				if ((msg.hwnd != hWnd) ||
					((msg.message != WM_SYSKEYDOWN) &&
					(msg.message != WM_SYSKEYUP))) {
					TranslateMessage(&msg);
				}
				DispatchMessage(&msg);
			}
			else {
				if (xmiloscfg.NOWAIT) {
					exec1frame();
					if (xmiloscfg.DRAW_SKIP) {		// nowait frame skip
						if (framecnt >= xmiloscfg.DRAW_SKIP) {
							processwait(0);
						}
					}
					else {							// nowait auto skip
						if (timing_getcount()) {
							processwait(0);
						}
					}
				}
				else if (xmiloscfg.DRAW_SKIP) {		// frame skip
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
							framereset(0);
						}
					}
					else {
						processwait(waitcnt);
						waitcnt = framecnt;
					}
				}
			}
		}
		else {											// background sleep
			if (!GetMessage(&msg, NULL, 0, 0)) {
				break;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	mousemng_disable(MOUSEPROC_WINUI);

	x1f_close();
	pccore_deinitialize();

	soundmng_deinitialize();

	juliet_YM2151Reset();
	juliet_YMF288Reset();
	juliet_unload();

	scrnmng_destroy();

	if (sys_updates	& (SYS_UPDATECFG | SYS_UPDATEOSCFG)) {
		initsave();
	}

	TRACETERM();

	return(msg.wParam);
}

