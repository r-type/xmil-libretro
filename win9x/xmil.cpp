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
#include	"winkbd.h"
#include	"menu.h"
#include	"ini.h"
#include "dialog\d_about.h"
#include "dialog\d_bmp.h"
#include "dialog\d_cfg.h"
#include "dialog\d_disk.h"
#include "dialog\d_sound.h"
#include	"extclass.h"
#include "misc\wndloc.h"
#include "misc\WndProc.h"
#include	"z80core.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"timing.h"
#if defined(SUPPORT_RESUME) || defined(SUPPORT_STATSAVE)
#include	"statsave.h"
#endif
#include	"debugsub.h"
#include	"scrndraw.h"
#include	"makescrn.h"
#include	"diskdrv.h"
#include	"fdd_ini.h"
#include	"x1f.h"
#if defined(SUPPORT_DCLOCK)
#include "subwnd/dclock.h"
#endif	// defined(SUPPORT_DCLOCK)
#include "subwnd/kdispwnd.h"
#include "subwnd/skbdwnd.h"

static const OEMCHAR szClassName[] = OEMTEXT("Xmil-MainWindow");

		XMILOSCFG	xmiloscfg = {
							CW_USEDEFAULT, CW_USEDEFAULT,
							1, 0, 0, 0, 0,
							0, 0, 0,
							1,
#if defined(SUPPORT_RESUME)
							0,
#endif
#if defined(SUPPORT_STATSAVE)
							1,
#endif
#if defined(SUPPORT_DCLOCK)
							0, 0, 0xffffff, 0xffbf6a,
#endif	// defined(SUPPORT_DCLOCK)
						};

		OEMCHAR		szProgName[] = OEMTEXT("X millennium ÇÀÇ±ÇøÇ„Å`ÇÒ");
		HWND		hWndMain;
		UINT8		g_scrnmode;
		OEMCHAR		modulefile[MAX_PATH];
		OEMCHAR		fddfolder[MAX_PATH];
		OEMCHAR		bmpfilefolder[MAX_PATH];

static	BOOL		xmilstopemulate = FALSE;
static	UINT8		xmilopening = 1;

static	CWndLoc		s_wndloc;

BRESULT xmil_changescreen(REG8 newmode) {

	REG8	change;
	REG8	renewal;
	BRESULT	r;

	change = g_scrnmode ^ newmode;
	renewal = (change & SCRNMODE_FULLSCREEN);
	if (g_scrnmode & (SCRNMODE_SYSHIGHCOLOR | SCRNMODE_COREHIGHCOLOR)) {
		renewal ^= SCRNMODE_COREHIGHCOLOR;
	}
	if (newmode & (SCRNMODE_SYSHIGHCOLOR | SCRNMODE_COREHIGHCOLOR)) {
		renewal ^= SCRNMODE_COREHIGHCOLOR;
	}
	r = SUCCESS;
	if (renewal) {
		if (renewal & SCRNMODE_FULLSCREEN) {
			kdispwin_destroy();
			skbdwin_destroy();
		}
		soundmng_stop();
		mousemng_disable(MOUSEPROC_WINUI);
		scrnmng_destroy();
		if (scrnmng_create(newmode) == SUCCESS) {
			g_scrnmode = newmode;
		}
		else {
			r = FAILURE;
			if (scrnmng_create(g_scrnmode) != SUCCESS) {
				PostQuitMessage(0);
				return r;
			}
		}
		scrndraw_redraw();
		mousemng_enable(MOUSEPROC_WINUI);
		soundmng_play();
	}
	else {
		g_scrnmode = newmode;
	}
	return r;
}

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

	HBITMAP	hbmp;
	BITMAP	bmp;
	HDC		hmdc;

	hbmp = LoadBitmap(hinst, res);
	GetObject(hbmp, sizeof(bmp), &bmp);
	hmdc = CreateCompatibleDC(hdc);
	SelectObject(hmdc, hbmp);
	BitBlt(hdc, x, y, bmp.bmWidth, bmp.bmHeight, hmdc, 0, 0, SRCCOPY);
	DeleteDC(hmdc);
	DeleteObject(hbmp);
}


// ----

#if defined(SUPPORT_RESUME)
static const OEMCHAR xmilresumeext[] = OEMTEXT(".sav");
static const OEMCHAR str_resume[] = OEMTEXT("Resume");
#endif
#if defined(SUPPORT_STATSAVE)
static const OEMCHAR xmilflagext[] = OEMTEXT(".sv%u");
static const OEMCHAR str_statload[] = OEMTEXT("Status Load");
#endif

#if defined(SUPPORT_RESUME) || defined(SUPPORT_STATSAVE)
static const OEMCHAR str_loaderr[] = OEMTEXT("Couldn't restart");
static const OEMCHAR str_conflict[] = OEMTEXT("Conflict!\n\n%s\nContinue?");

static void getstatfilename(OEMCHAR *path, const OEMCHAR *ext, UINT size) {

	file_cpyname(path, modulefile, size);
	file_cutext(path);
	file_catname(path, ext, size);
}

static int flagsave(const OEMCHAR *ext) {

	OEMCHAR	path[MAX_PATH];
	int		ret;

	getstatfilename(path, ext, _countof(path));
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

	getstatfilename(path, ext, _countof(path));
	file_delete(path);
}

static int flagload(const OEMCHAR *ext, const OEMCHAR *title, BOOL force) {

	int		ret;
	int		id;
	OEMCHAR	path[MAX_PATH];
	OEMCHAR	buf[1024];
	OEMCHAR buf2[1024 + 32];

	getstatfilename(path, ext, _countof(path));
	winuienter();
	id = IDYES;
	ret = statsave_check(path, buf, _countof(buf));
	if (ret & (~STATFLAG_DISKCHG)) {
		MessageBox(hWndMain, str_loaderr, title, MB_OK | MB_ICONSTOP);
		id = IDNO;
	}
	else if ((!force) && (ret & STATFLAG_DISKCHG)) {
		OEMSPRINTF(buf2, str_conflict, buf);
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
#if defined(SUPPORT_STATSAVE)
	OEMCHAR ext[16];
#endif

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
			CConfigDlg::Config(hWnd);
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

		case IDM_FDD2OPEN:
			winuienter();
			dialog_changefdd(hWnd, 2);
			winuileave();
			break;

		case IDM_FDD2EJECT:
			diskdrv_setfdd(2, NULL, 0);
			break;

		case IDM_FDD3OPEN:
			winuienter();
			dialog_changefdd(hWnd, 3);
			winuileave();
			break;

		case IDM_FDD3EJECT:
			diskdrv_setfdd(3, NULL, 0);
			break;

		case IDM_X1ROM:
			xmilcfg.ROM_TYPE = 1;
			update = SYS_UPDATECFG;
			break;

		case IDM_TURBO:
			xmilcfg.ROM_TYPE = 2;
			update = SYS_UPDATECFG;
			break;

#if defined(SUPPORT_TURBOZ)
		case IDM_TURBOZ:
			xmilcfg.ROM_TYPE = 3;
			update = SYS_UPDATECFG;
			break;
#endif

		case IDM_BOOT2D:
			xmilcfg.DIP_SW &= ~DIPSW_BOOTMEDIA;
			update = SYS_UPDATECFG;
			break;

		case IDM_BOOT2HD:
			xmilcfg.DIP_SW |= DIPSW_BOOTMEDIA;
			update = SYS_UPDATECFG;
			break;

		case IDM_HIGHRES:
			xmilcfg.DIP_SW &= ~DIPSW_RESOLUTE;
			update = SYS_UPDATECFG;
			break;

		case IDM_LOWRES:
			xmilcfg.DIP_SW |= DIPSW_RESOLUTE;
			update = SYS_UPDATECFG;
			break;

		case IDM_WINDOW:
			xmil_changescreen(g_scrnmode & (~SCRNMODE_FULLSCREEN));
			break;

		case IDM_FULLSCREEN:
			xmil_changescreen(g_scrnmode | SCRNMODE_FULLSCREEN);
			break;

		case IDM_DISPSYNC:
			xmilcfg.DISPSYNC = !xmilcfg.DISPSYNC;
			update = SYS_UPDATECFG;
			break;

		case IDM_RASTER:
			xmilcfg.RASTER = !xmilcfg.RASTER;
			if (xmilcfg.RASTER) {
				xmil_changescreen(g_scrnmode | SCRNMODE_SYSHIGHCOLOR);
			}
			else {
				xmil_changescreen(g_scrnmode & (~SCRNMODE_SYSHIGHCOLOR));
			}
			update = SYS_UPDATECFG;
			break;

		case IDM_NOWAIT:
			xmiloscfg.NOWAIT = !xmiloscfg.NOWAIT;
			update = SYS_UPDATECFG;
			break;

		case IDM_AUTOFPS:
			xmiloscfg.DRAW_SKIP = 0;
			update = SYS_UPDATECFG;
			break;

		case IDM_60FPS:
			xmiloscfg.DRAW_SKIP = 1;
			update = SYS_UPDATECFG;
			break;

		case IDM_30FPS:
			xmiloscfg.DRAW_SKIP = 2;
			update = SYS_UPDATECFG;
			break;

		case IDM_20FPS:
			xmiloscfg.DRAW_SKIP = 3;
			update = SYS_UPDATECFG;
			break;

		case IDM_15FPS:
			xmiloscfg.DRAW_SKIP = 4;
			update = SYS_UPDATECFG;
			break;

		case IDM_KEY:
			xmilcfg.KEY_MODE = 0;
			update = SYS_UPDATECFG;
			break;

		case IDM_JOY1:
			xmilcfg.KEY_MODE = 1;
			update = SYS_UPDATECFG;
			break;

		case IDM_JOY2:
			xmilcfg.KEY_MODE = 2;
			update = SYS_UPDATECFG;
			break;

#if defined(SUPPORT_TURBOZ) || defined(SUPPORT_OPM)
#if defined(SUPPORT_OPMx2)
		case IDM_OPM_NONE:
		case IDM_OPM_1:
		case IDM_OPM_2:
			xmilcfg.SOUND_SW = static_cast<UINT8>(cmd - IDM_OPM_NONE);
			update = SYS_UPDATECFG;
			break;
#else	// defined(SUPPORT_OPMx2)
		case IDM_FMBOARD:
			xmilcfg.SOUND_SW = !xmilcfg.SOUND_SW;
			update = SYS_UPDATECFG;
			break;
#endif	// defined(SUPPORT_OPMx2)
#endif	// defined(SUPPORT_TURBOZ) || defined(SUPPORT_OPM)

		case IDM_JOYSTICK:
			xmiloscfg.JOYSTICK = !xmiloscfg.JOYSTICK;
			update = SYS_UPDATECFG;
			break;

		case IDM_MOUSE:
		//	mousemng_toggle(MOUSEPROC_SYSTEM);
			xmilcfg.MOUSE_SW = !xmilcfg.MOUSE_SW;
			update = SYS_UPDATECFG;
			break;

#if !defined(DISABLE_SOUND)
		case IDM_SEEKSND:
			xmilcfg.MOTOR = !xmilcfg.MOTOR;
			update = SYS_UPDATECFG;
			break;
#endif	// !defined(DISABLE_SOUND)

		case IDM_WIDTH40:
			crtc_forcesetwidth(40);
			break;

		case IDM_WIDTH80:
			crtc_forcesetwidth(80);
			break;

		case IDM_Z80SAVE:
			debugsub_status();
			break;

		case IDM_BMPSAVE:
			winuienter();
			dialog_writebmp(hWnd);
			winuileave();
			break;

#if defined(SUPPORT_X1F)
		case IDM_OPMLOG:
			winuienter();
			dialog_x1f(hWnd);
			winuileave();
			break;
#endif	// defined(SUPPORT_X1F)

		case IDM_ALTENTER:
			xmiloscfg.shortcut ^= 1;
			update |= SYS_UPDATECFG;
			break;

		case IDM_ALTF4:
			xmiloscfg.shortcut ^= 2;
			update |= SYS_UPDATECFG;
			break;

		case IDM_DISPCLOCK:
			xmiloscfg.DISPCLK ^= 1;
			update = SYS_UPDATECFG;
			sysmng_workclockrenewal();
			sysmng_updatecaption(3);
			break;

		case IDM_DISPFRAME:
			xmiloscfg.DISPCLK ^= 2;
			update = SYS_UPDATECFG;
			sysmng_workclockrenewal();
			sysmng_updatecaption(3);
			break;

		case IDM_JOYX:
			xmilcfg.BTN_MODE = !xmilcfg.BTN_MODE;
			update = SYS_UPDATECFG;
			break;

		case IDM_RAPID:
			xmilcfg.BTN_RAPID = !xmilcfg.BTN_RAPID;
			update = SYS_UPDATECFG;
			break;

		case IDM_ABOUT:
			winuienter();
			CAboutDlg::About(hWnd);
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
				flagload(ext, str_statload, TRUE);
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
#if defined(SUPPORT_KEYDISP)
				case IDM_KEYDISP:
					if (kdispwin_gethwnd() == NULL)
					{
						kdispwin_create();
					}
					else
					{
						kdispwin_destroy();
					}
					break;
#endif	// defined(SUPPORT_KEYDISP)

#if defined(SUPPORT_SOFTKBD)
				case IDM_SOFTKBD:
					if (skbdwin_gethwnd() == NULL)
					{
						skbdwin_create();
					}
					else
					{
						skbdwin_destroy();
					}
					break;
#endif	// defined(SUPPORT_SOFTKBD)

				case IDM_SCREENCENTER:
					if (!scrnmng_isfullscreen()) {
						wincentering(hWnd);
					}
					break;

				case IDM_SNAPENABLE:
					xmiloscfg.WINSNAP = !xmiloscfg.WINSNAP;
					updateflag = SYS_UPDATEOSCFG;
					break;

				case IDM_BACKGROUND:
					xmiloscfg.background ^= 1;
					updateflag = SYS_UPDATEOSCFG;
					break;

				case IDM_BGSOUND:
					xmiloscfg.background ^= 2;
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

				hinst = reinterpret_cast<HINSTANCE>(GetWindowLong(hWnd, GWLP_HINSTANCE));
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
			sysmenu_update(GetSystemMenu(hWnd, FALSE));
			menu_update(extclass_gethmenu(hWnd));
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
			s_wndloc.Start();
			break;

		case WM_EXITSIZEMOVE:
			mousemng_enable(MOUSEPROC_WINUI);
			soundmng_enable(SNDPROC_MAIN);
			break;

		case WM_MOVING:
			if (xmiloscfg.WINSNAP)
			{
				s_wndloc.Moving(reinterpret_cast<RECT*>(lParam));
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
			if (lParam & 0x20000000) {
				if ((xmiloscfg.shortcut & 1) && (wParam == VK_RETURN)) {
					xmil_changescreen(g_scrnmode ^ SCRNMODE_FULLSCREEN);
					break;
				}
				if ((xmiloscfg.shortcut & 2) && (wParam == VK_F4)) {
					SendMessage(hWnd, WM_CLOSE, 0, 0L);
					break;
				}
			}
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
#if defined(SUPPORT_DCLOCK)
				if (scrnmng_isfullscreen()) {
					POINT p;
					if ((GetCursorPos(&p)) && (p.y >= 466)) {
						xmiloscfg.clockx++;
						DispClock::GetInstance()->Reset();
						sysmng_update(SYS_UPDATEOSCFG);
					}
				}
#endif	/* defined(SUPPORT_DCLOCK) */
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
#if defined(SUPPORT_DCLOCK)
				if (scrnmng_isfullscreen()) {
					POINT p;
					if ((GetCursorPos(&p)) && (p.y >= 466) &&
												(xmiloscfg.clockx)) {
						xmiloscfg.clockfnt++;
						DispClock::GetInstance()->Reset();
						sysmng_update(SYS_UPDATEOSCFG);
					}
				}
#endif	/* defined(SUPPORT_DCLOCK) */
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
#if defined(SUPPORT_DCLOCK)
	scrnmng_dispclock();
#endif	/* defined(SUPPORT_DCLOCK) */
	kdispwin_draw((UINT8)cnt);
	skbdwin_process();
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
#if defined(SUPPORT_DCLOCK)
	DispClock::GetInstance()->Update();
#endif	/* defined(SUPPORT_DCLOCK) */
	framecnt++;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInst,
										LPSTR lpszCmdLine, int nCmdShow) {

	HWND		hWnd;
	WNDCLASS	wc;
	MSG			msg;

	CWndProc::Initialize(hInstance);
	CSubWndBase::Initialize(hInstance);

	GetModuleFileName(NULL, modulefile, _countof(modulefile));
	dosio_init();
	file_setcd(modulefile);
	initload();
	kdispwin_readini();
	skbdwin_readini();

	hWnd = FindWindow(szClassName, NULL);
	if (hWnd != NULL) {
		ShowWindow(hWnd, SW_RESTORE);
		SetForegroundWindow(hWnd);
		dosio_term();
		return(FALSE);
	}

	TRACEINIT();

//	keystat_initialize();

	if (!hPrevInst) {
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

		kdispwin_initialize();
		skbdwin_initialize();
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

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	sysmenu_initialize(GetSystemMenu(hWnd, FALSE));
	menu_initialize(GetMenu(hWnd));
	DrawMenuBar(hWnd);

	scrndraw_initialize();
	g_scrnmode = 0;
	if (xmilcfg.RASTER) {
		g_scrnmode |= SCRNMODE_SYSHIGHCOLOR;
	}
	if (scrnmng_create(g_scrnmode) != SUCCESS) {
		g_scrnmode ^= SCRNMODE_FULLSCREEN;
		if (scrnmng_create(g_scrnmode) != SUCCESS) {
			MessageBox(hWnd, OEMTEXT("Couldn't create DirectDraw Object"),
													szProgName, MB_OK);
			return(FALSE);
		}
	}

	if (soundmng_initialize() == SUCCESS) {
#if !defined(DISABLE_SOUND)
		soundmng_pcmload(SOUND_PCMSEEK, OEMTEXT("fddseek.wav"), 0);
		soundmng_pcmload(SOUND_PCMSEEK1, OEMTEXT("fddseek1.wav"), 0);
		soundmng_pcmvolume(SOUND_PCMSEEK, xmilcfg.MOTORVOL);
		soundmng_pcmvolume(SOUND_PCMSEEK1, xmilcfg.MOTORVOL);
#endif	// !defined(DISABLE_SOUND)
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

	kdispwin_destroy();
	skbdwin_destroy();

	mousemng_disable(MOUSEPROC_WINUI);

	x1f_close();
	pccore_deinitialize();

	soundmng_deinitialize();

	scrnmng_destroy();

	if (sys_updates	& (SYS_UPDATECFG | SYS_UPDATEOSCFG)) {
		initsave();
		kdispwin_writeini();
		skbdwin_writeini();
	}

	TRACETERM();

	return static_cast<int>(msg.wParam);
}

