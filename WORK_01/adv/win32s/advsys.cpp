#include	"compiler.h"
#ifndef __GNUC__
#include	<winnls32.h>
#endif
#include	"dosio.h"
#include	"advuser.h"

#ifdef __cplusplus
extern "C" {
#endif
extern int advmain(int argc, char **argv, char **envp);
#ifdef __cplusplus
}
#endif

	BRESULT	__adv_avail;
	HWND	__adv_hWnd;
	OEMCHAR	__adv_modulefile[MAX_PATH];
	UINT8	__adv_reg[0x1000];

static const OEMCHAR szClassName[] = OEMTEXT("Windebug-ADV");
static const OEMCHAR szCaptionName[] = OEMTEXT("ADV-Window");


LRESULT CALLBACK AdvProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {

	PAINTSTRUCT	ps;
	HDC			hdc;

	switch (msg) {
		case WM_CREATE:
#ifndef __GNUC__
			WINNLSEnableIME(hWnd, FALSE);
#endif
			break;

		case WM_PAINT:
			hdc = BeginPaint(hWnd, &ps);
			advv_update();
			EndPaint(hWnd, &ps);
			break;

		case WM_ENTERMENULOOP:
			advv_topwinui();
			break;

		case WM_EXITMENULOOP:
			advv_clearwinui();
			break;

		case WM_KEYDOWN:
			advkey_keydown((UINT16)LOWORD(wp));
			break;

		case WM_KEYUP:
			advkey_keyup((UINT16)LOWORD(wp));
			break;

		case WM_CLOSE:
			DestroyWindow(hWnd);
			break;

		case WM_DESTROY:
			PostQuitMessage(0);
			break;

		default:
			return(DefWindowProc(hWnd, msg, wp, lp));
	}
	return(0);
}


// ----

int main(int argc, char **argv, char **envp) {

	HINSTANCE	hInstance;
	WNDCLASS	advwc;
	HWND		hWnd;

	GetModuleFileName(NULL, __adv_modulefile, sizeof(__adv_modulefile));
	dosio_init();
	file_setcd(__adv_modulefile);

	hWnd = FindWindow(szClassName, NULL);
	if (hWnd != NULL) {
		ShowWindow(hWnd, SW_RESTORE);
		SetForegroundWindow(hWnd);
		dosio_term();
		return(0);
	}

	hInstance = (HINSTANCE)GetWindowLong(HWND_DESKTOP, GWL_HINSTANCE);
	advwc.style = CS_BYTEALIGNCLIENT | CS_HREDRAW | CS_VREDRAW;
	advwc.lpfnWndProc = advuser_proc;
	advwc.cbClsExtra = 0;
	advwc.cbWndExtra = 0;
	advwc.hInstance = hInstance;
	advwc.hIcon = NULL;
	advwc.hCursor = LoadCursor(NULL, IDC_ARROW);
	advwc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	advwc.lpszMenuName = NULL;
	advwc.lpszClassName = szClassName;
	if (!RegisterClass(&advwc)) {
		return(0);
	}
	hWnd = CreateWindow(szClassName, szCaptionName,
						WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION |
						WS_MINIMIZEBOX,
						CW_USEDEFAULT, CW_USEDEFAULT, 240, 160,
						NULL, NULL, hInstance, NULL);
	__adv_hWnd = hWnd;
	if (hWnd == NULL) {
		return(-1);
	}
	ShowWindow(hWnd, SW_SHOW);
	UpdateWindow(hWnd);
	__adv_avail = TRUE;
	if (advv_initialize(hWnd) != SUCCESS) {
		DestroyWindow(hWnd);
		goto advsys_err;
	}
	if (advuser_initialize() != SUCCESS) {
		DestroyWindow(hWnd);
		goto advsys_err;
	}
	advkey_initialize();
	advmain(argc, argv, envp);
	advuser_deinitialize();

advsys_err:
	dosio_term();
	return(0);
}

BRESULT advsys_task(void) {

	MSG		msg;

	if (__adv_avail) {
		while(PeekMessage(&msg, 0, 0, 0, PM_NOREMOVE)) {
			if (!GetMessage(&msg, NULL, 0, 0)) {
				__adv_avail = FALSE;
				break;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return(__adv_avail);
}

