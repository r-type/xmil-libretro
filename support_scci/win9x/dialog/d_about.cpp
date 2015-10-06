#include	"compiler.h"
#include	"resource.h"
#include	"pccore.h"
#include	"dialog.h"


static const OEMCHAR str_xmiltitle[] = OEMTEXT("X millennium  ");
// static const OEMCHAR str_xmiltitle[] = OEMTEXT("‚Ë‚±‚ÆƒBƒ•‚Ì”÷–­‚ÈŠÖŒW  ");

static void about_init(HWND hWnd) {

	OEMCHAR	work[128];

	milstr_ncpy(work, str_xmiltitle, NELEMENTS(work));
	milstr_ncat(work, xmilversion, NELEMENTS(work));
#if defined(XMILVER_WIN9X)
	milstr_ncat(work, XMILVER_WIN9X, NELEMENTS(work));
#endif
	SetDlgItemText(hWnd, IDC_XMILVER, work);
}

LRESULT CALLBACK AboutDialogProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {

	switch (msg) {
		case WM_INITDIALOG:
			about_init(hWnd);
			return(FALSE);

		case WM_COMMAND:
			switch(LOWORD(wp)) {
				case IDOK:
					EndDialog(hWnd, IDOK);
					break;

				default:
					return(FALSE);
			}
			break;

		case WM_CLOSE:
			PostMessage(hWnd, WM_COMMAND, IDOK, 0);
			break;

		default:
			return(FALSE);
	}
	return(TRUE);
}

