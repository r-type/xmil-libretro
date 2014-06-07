#include	"compiler.h"
#include	<prsht.h>
#include	"resource.h"
#include	"winloc.h"
#include	"extclass.h"


#if 0
const OEMCHAR extclass[] = OEMTEXT("xmildialog");


void extclass_initialize(HINSTANCE hinst) {

	WNDCLASS	wc;

	ZeroMemory(&wc, sizeof(wc));
	wc.style = CS_SAVEBITS | CS_DBLCLKS;
	wc.lpfnWndProc = DefDlgProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = DLGWINDOWEXTRA;
	wc.hInstance = hinst;
	wc.hIcon = LoadIcon(hinst, MAKEINTRESOURCE(IDI_ICON2));
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)NULL;
	wc.lpszMenuName = NULL;
	wc.lpszClassName = extclass;
	RegisterClass(&wc);
}

void extclass_deinitialize(HINSTANCE hinst) {

	UnregisterClass(np2dlgclass, hinst);
}

void extclass_move(HWND hWnd, int posx, int posy, int cx, int cy) {

	RECT	workrc;

	SystemParametersInfo(SPI_GETWORKAREA, 0, &workrc, 0);

	if (workrc.right < (posx + cx)) {
		posx = workrc.right - cx;
	}
	if (workrc.left > posx) {
		posx = workrc.left;
	}
	if (workrc.bottom < (posy + cy)) {
		posy = workrc.bottom - cy;
	}
	if (workrc.top > posy) {
		posy = workrc.top;
	}
	MoveWindow(hWnd, posx, posy, cx, cy, TRUE);
}

int CALLBACK extclass_propetysheet(HWND hWndDlg, UINT uMsg, LPARAM lParam) {

	if (uMsg == PSCB_INITIALIZED) {
		SetWindowLong(hWndDlg, GWL_EXSTYLE,
				GetWindowLong(hWndDlg, GWL_EXSTYLE) & (~WS_EX_CONTEXTHELP));
	}
	return(0);
}
#endif


// ----

void extclass_wmcreate(HWND hWnd) {

	SetWindowLong(hWnd, EXTGWL_HMENU, 0);
}

void extclass_wmdestroy(HWND hWnd) {

	HMENU	hmenu;

	hmenu = (HMENU)GetWindowLong(hWnd, EXTGWL_HMENU);
	if (hmenu != NULL) {
		DestroyMenu(hmenu);
		SetWindowLong(hWnd, EXTGWL_HMENU, 0);
	}
}

void extclass_enablemenu(HWND hWnd, BRESULT enable) {

	HMENU	hmenu;
	BRESULT	draw;

	hmenu = (HMENU)GetWindowLong(hWnd, EXTGWL_HMENU);
	draw = FALSE;
	if (enable) {
		if (hmenu) {
			SetMenu(hWnd, hmenu);
			hmenu = NULL;
			draw = TRUE;
		}
	}
	else {
		if (hmenu == NULL) {
			hmenu = GetMenu(hWnd);
			if (hmenu) {
				SetMenu(hWnd, NULL);
				draw = TRUE;
			}
		}
	}
	SetWindowLong(hWnd, EXTGWL_HMENU, (LONG)hmenu);
	if (draw) {
		DrawMenuBar(hWnd);
	}
}

HMENU extclass_gethmenu(HWND hWnd) {

	HMENU	ret;

	ret = (HMENU)GetWindowLong(hWnd, EXTGWL_HMENU);
	if (ret == NULL) {
		ret = GetMenu(hWnd);
	}
	return(ret);
}

