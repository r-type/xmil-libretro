#include	"compiler.h"
#include	<gx.h>
#if !defined(_WIN32_WCE)
#include	<winnls32.h>
#endif
#include	"resource.h"
#include	"strres.h"
#include	"xmil.h"
#include	"dosio.h"
#include	"inputmng.h"
#include	"scrnmng.h"
#include	"soundmng.h"
#include	"sysmng.h"
#include	"taskmng.h"
#include	"sysmenu.h"
#include	"winkbd.h"
#include	"ini.h"
#include	"z80core.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"timing.h"
#if defined(SUPPORT_RESUME) || defined(SUPPORT_STATSAVE)
#include	"statsave.h"
#endif
#include	"scrndraw.h"
#include	"makescrn.h"
#include	"diskdrv.h"
#include	"x1f.h"
#include	"vramhdl.h"
#include	"menubase.h"


static const OEMCHAR szAppCaption[] = OEMTEXT("X millennium");
static const OEMCHAR szClassName[] = OEMTEXT("Xmil-MainWindow");


		XMILOSCFG	xmiloscfg = {
#if !defined(GX_DLL)
								CW_USEDEFAULT, CW_USEDEFAULT,
#endif
								0, 0,
#if defined(WIN32_PLATFORM_PSPC)
								0, 0,
#endif
					};
		HWND		hWndMain;
		HINSTANCE	hInst;
		HINSTANCE	hPreI;
		OEMCHAR		modulefile[MAX_PATH];
		GXKeyList	gx_keylist;

enum {
	SYSRUNNING_MAIN		= 1,
	SYSRUNNING_FORE		= 2
};

static	UINT		sysrunning;
static	UINT		framecnt;
static	UINT		waitcnt;
static	UINT		framemax = 1;


// ---- resume

#if 0
static void getstatfilename(char *path, const char *ext, int size) {

	file_cpyname(path, modulefile, size);
	file_cutext(path);
	file_catname(path, str_dot, size);
	file_catname(path, ext, size);
}

static int flagsave(const char *ext) {

	int		ret;
	char	path[MAX_PATH];

	getstatfilename(path, ext, sizeof(path));
	ret = statsave_save(path);
	if (ret) {
		file_delete(path);
	}
	return(ret);
}

static void flagdelete(const char *ext) {

	char	path[MAX_PATH];

	getstatfilename(path, ext, sizeof(path));
	file_delete(path);
}

static int flagload(const char *ext, const char *title, BOOL force) {

	int		ret;
	int		id;
	char	path[MAX_PATH];
	char	buf[1024];
	char	buf2[1024 + 256];

	getstatfilename(path, ext, sizeof(path));
	id = DID_YES;
	ret = statsave_check(path, buf, sizeof(buf));
	if (ret & (~STATFLAG_DISKCHG)) {
		menumbox("Couldn't restart", title, MBOX_OK | MBOX_ICONSTOP);
		id = DID_NO;
	}
	else if ((!force) && (ret & STATFLAG_DISKCHG)) {
		SPRINTF(buf2, "Conflict!\n\n%s\nContinue?", buf);
		id = menumbox(buf2, title, MBOX_YESNOCAN | MBOX_ICONQUESTION);
	}
	if (id == DID_YES) {
		statsave_load(path);
	}
	return(id);
}
#endif


// ---- proc

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {

	PAINTSTRUCT	ps;
	HDC			hdc;

	switch (msg) {
		case WM_CREATE:
#if !defined(_WIN32_WCE)
			WINNLSEnableIME(hWnd, FALSE);
#else
			ImmAssociateContext(hWnd, NULL);
#endif
			break;

		case WM_PAINT:
			hdc = BeginPaint(hWnd, &ps);
			if (!sysrunning) {
				scrnmng_clear(TRUE);
			}
			else {
				scrndraw_redraw();
			}
			EndPaint(hWnd, &ps);
			break;

#if defined(WIN32_PLATFORM_PSPC)
		case WM_ERASEBKGND:
			if (sysrunning) {
				scrndraw_redraw();
			}
			break;
#endif

		case WM_KEYDOWN:
			winkbd_keydown(wParam, lParam);
			break;

		case WM_KEYUP:
			winkbd_keyup(wParam, lParam);
			break;

		case WM_SYSKEYDOWN:
			winkbd_keydown(wParam, lParam);
			break;

		case WM_SYSKEYUP:
			winkbd_keyup(wParam, lParam);
			break;

		case WM_MOUSEMOVE:
			if (scrnmng_mousepos(&lParam) == SUCCESS) {
				if (menuvram) {
					menubase_moving(LOWORD(lParam), HIWORD(lParam), 0);
				}
			}
			break;

		case WM_LBUTTONDOWN:
			if (scrnmng_mousepos(&lParam) == SUCCESS) {
				if (menuvram) {
					menubase_moving(LOWORD(lParam), HIWORD(lParam), 1);
				}
				else if (scrnmng_kbdpos(&lParam) == SUCCESS) {
#if defined(SUPPORT_SOFTKBD)
					softkbd_down(LOWORD(lParam), HIWORD(lParam));
#endif
				}
			}
			break;

		case WM_LBUTTONUP:
			if (scrnmng_mousepos(&lParam) == SUCCESS) {
#if defined(SUPPORT_SOFTKBD)
				softkbd_up();
#endif
				if (menuvram) {
					menubase_moving(LOWORD(lParam), HIWORD(lParam), 2);
				}
				else if (scrnmng_ismenu(lParam)) {
					sysmenu_menuopen(0, LOWORD(lParam), HIWORD(lParam));
				}
			}
			break;

#if !defined(GX_DLL)
		case WM_MOVE:
#if !defined(_WIN32_WCE)
			if (!(GetWindowLong(hWnd, GWL_STYLE) &
											(WS_MAXIMIZE | WS_MINIMIZE)))
#endif
			{
				RECT rc;
				GetWindowRect(hWnd, &rc);
				xmiloscfg.winx = rc.left;
				xmiloscfg.winy = rc.top;
				sysmng_update(SYS_UPDATEOSCFG);
			}
			break;
#endif

#if !defined(_WIN32_WCE)
		case WM_ENTERSIZEMOVE:
			soundmng_stop();
			break;

		case WM_EXITSIZEMOVE:
			soundmng_play();
			break;
#endif

		case WM_CLOSE:
#if !defined(WIN32_PLATFORM_PSPC)
			taskmng_exit();
#endif
			break;

		case WM_DESTROY:
			break;

		case WM_ACTIVATE:
			if (sysrunning) {
				if (LOWORD(wParam) != WA_INACTIVE) {
					GXResume();
#if defined(GX_DLL)
					scrnmng_enable(TRUE);
#endif
					scrndraw_redraw();
					soundmng_enable(SNDPROC_MAIN);
					sysrunning |= SYSRUNNING_FORE;
				}
				else {
					sysrunning &= ~SYSRUNNING_FORE;
					soundmng_disable(SNDPROC_MAIN);
#if defined(GX_DLL)
					scrnmng_enable(FALSE);
#endif
					GXSuspend();
				}
			}
			break;

#if defined(WAVEMNG_CBMAIN)
		case MM_WOM_DONE:
			soundmng_cb(MM_WOM_DONE, (HWAVEOUT)wParam, (WAVEHDR *)lParam);
			break;
#endif

		default:
			return(DefWindowProc(hWnd, msg, wParam, lParam));
	}
	return(0L);
}


#define	framereset(cnt)		framecnt = 0

static void processwait(UINT cnt) {

	if (timing_getcount() >= cnt) {
		timing_setcount(0);
		framereset(cnt);
	}
#if !defined(_WIN32_WCE)
	else {
		Sleep(1);
	}
#endif
}


// ----

#if defined(UNICODE) && defined(OSLANG_SJIS)
static DWORD _GetModuleFileName(HMODULE hModule,
										OEMCHAR *lpFileName, DWORD nSize) {

	UINT16	ucs2[MAX_PATH];

	GetModuleFileName(hModule, ucs2, NELEMENTS(ucs2));
	nSize = WideCharToMultiByte(CP_ACP, 0, ucs2, -1,
										lpFileName, nSize, NULL, NULL);
	if (nSize) {
		nSize--;
	}
	return(nSize);
}
#elif defined(OSLANG_UTF8)
static DWORD _GetModuleFileName(HMODULE hModule,
										OEMCHAR *lpFileName, DWORD nSize) {

	UINT16	ucs2[MAX_PATH];

	GetModuleFileName(hModule, ucs2, NELEMENTS(ucs2));
	nSize = ucscnv_ucs2toutf8(lpFileName, nSize, ucs2, (UINT)-1);
	if (nSize) {
		nSize--;
	}
	return(nSize);
}
#else
#define	_GetModuleFileName(a, b, c)		GetModuleFileName(a, b, c)
#endif


#if defined(_WIN32_WCE)
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPreInst,
										LPWSTR lpszCmdLine, int nCmdShow)
#elif defined(UNICODE)
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPreInst,
										LPSTR lpszCmdLine, int nCmdShow)
#else
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPreInst,
										LPSTR lpszCmdLine, int nCmdShow)
#endif
{
	HWND		hWnd;
	WNDCLASS	np2;
	MSG			msg;

	hWnd = FindWindow(szClassName, NULL);
	if (hWnd != NULL) {
#if !defined(WIN32_PLATFORM_PSPC)
		ShowWindow(hWnd, SW_RESTORE);
		SetForegroundWindow(hWnd);
#else
		ShowWindow(hWnd, SW_SHOW);
		SetForegroundWindow((HWND)((ULONG)hWnd | 1));
#endif
		return(0);
	}

	_GetModuleFileName(NULL, modulefile, sizeof(modulefile));
	dosio_init();
	file_setcd(modulefile);
//	initload();

	hInst = hInstance;
	hPreI = hPreInst;
	TRACEINIT();

	inputmng_initialize();
//	keystat_initialize();

//	if (!hPreInst) {
		np2.style = CS_HREDRAW | CS_VREDRAW;
		np2.lpfnWndProc = WndProc;
		np2.cbClsExtra = 0;
		np2.cbWndExtra = 0;
		np2.hInstance = hInstance;
		np2.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
		np2.hCursor = LoadCursor(NULL, IDC_ARROW);
		np2.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
		np2.lpszMenuName = NULL;
		np2.lpszClassName = szClassName;
		if (!RegisterClass(&np2)) {
			return(FALSE);
		}
//	}

#if !defined(GX_DLL)
	hWnd = CreateWindow(szClassName, szAppCaption,
						WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION |
						WS_MINIMIZEBOX,
						xmiloscfg.winx, xmiloscfg.winy,
						FULLSCREEN_WIDTH, FULLSCREEN_HEIGHT,
						NULL, NULL, hInstance, NULL);
#else
	hWnd = CreateWindow(szClassName, szAppCaption,
						WS_VISIBLE,
						0, 0,
		      			GetSystemMetrics(SM_CXSCREEN),
		      			GetSystemMetrics(SM_CYSCREEN),
						NULL, NULL, hInstance, NULL);
#endif
	hWndMain = hWnd;
	if (hWnd == NULL) {
		goto np2main_err1;
	}

	scrnmng_initialize();

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	if (sysmenu_create() != SUCCESS) {
		DestroyWindow(hWnd);
		goto np2main_err1;
	}
	if (scrnmng_create(hWnd, FULLSCREEN_WIDTH, FULLSCREEN_HEIGHT)
																!= SUCCESS) {
		MessageBox(hWnd, STRLITERAL("Couldn't create DirectDraw Object"),
									szAppCaption, MB_OK | MB_ICONSTOP);
		DestroyWindow(hWnd);
		goto np2main_err2;
	}

	if (GXOpenInput() == 0) {
		DestroyWindow(hWnd);
		goto np2main_err3;
	}
	gx_keylist = GXGetDefaultKeys(GX_NORMALKEYS);
	scrnmng_keybinds();
#if defined(WIN32_PLATFORM_PSPC)
	winkbd_bindinit();
#endif

	soundmng_initialize();
	sysmng_initialize();
	taskmng_initialize();

	pccore_initialize();
	pccore_reset();

	scrndraw_redraw();

#if defined(WIN32_PLATFORM_PSPC)
	winkbd_bindcur(xmiloscfg.bindcur);
	winkbd_bindbtn(xmiloscfg.bindbtn);
#endif

	sysrunning |= SYSRUNNING_MAIN | SYSRUNNING_FORE;

#if defined(SUPPORT_RESUME)
	if (np2oscfg.resume) {
		if (flagload(str_sav, str_resume, FALSE) == DID_CANCEL) {
			DestroyWindow(hWnd);
			goto np2main_err4;
		}
	}
#endif

	while(taskmng_isavail()) {
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
		else if (sysrunning & SYSRUNNING_FORE) {
			if (xmiloscfg.NOWAIT) {
				pccore_exec(framecnt == 0);
				if (xmiloscfg.DRAW_SKIP) {			// nowait frame skip
					framecnt++;
					if (framecnt >= xmiloscfg.DRAW_SKIP) {
						processwait(0);
					}
				}
				else {							// nowait auto skip
					framecnt = 1;
					if (timing_getcount()) {
						processwait(0);
					}
				}
			}
			else if (xmiloscfg.DRAW_SKIP) {		// frame skip
				if (framecnt < xmiloscfg.DRAW_SKIP) {
					pccore_exec(framecnt == 0);
					framecnt++;
				}
				else {
					processwait(xmiloscfg.DRAW_SKIP);
				}
			}
			else {								// auto skip
				if (!waitcnt) {
					UINT cnt;
					pccore_exec(framecnt == 0);
					framecnt++;
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
		else {
			Sleep(100);
		}
	}

	soundmng_disable(SNDPROC_MAIN);
	sysrunning = FALSE;

	DestroyWindow(hWnd);

//	pccore_cfgupdate();
#if defined(SUPPORT_RESUME)
	if (np2oscfg.resume) {
		flagsave(str_sav);
	}
	else {
		flagdelete(str_sav);
	}
#endif
	pccore_deinitialize();
	x1f_close();
	soundmng_deinitialize();

	GXCloseInput();
	scrnmng_destroy();
	sysmenu_destroy();

//	if (sys_updates	& (SYS_UPDATECFG | SYS_UPDATEOSCFG)) {
//		initsave();
//	}
	TRACETERM();
	dosio_term();

	return(msg.wParam);

np2main_err4:
	pccore_deinitialize();
	x1f_close();
	soundmng_deinitialize();
	GXCloseInput();

np2main_err3:
	scrnmng_destroy();

np2main_err2:
	sysmenu_destroy();

np2main_err1:
	TRACETERM();
	dosio_term();
	return(FALSE);
}

