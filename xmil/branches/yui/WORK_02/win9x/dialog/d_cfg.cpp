#include	"compiler.h"
#include	<commctrl.h>
#include	"strres.h"
#include	"xmil.h"
#include	"resource.h"
#include	"dosio.h"
#include	"soundmng.h"
#include	"sysmng.h"
#include	"ini.h"
#include	"dialog.h"
#include	"dialogs.h"
#include	"pccore.h"
#include	"palettes.h"
#include	"makescrn.h"


#define	LIMITS(v, n, m)		(((v) > (m))?(m):(((v) < (n))?(n):(v)))

static const UINT32 ratehz[] = {48000, 44100, 33075, 32000,
								24000, 22050, 16000, 11025};

static void cfgcreate(HWND hWnd) {

	OEMCHAR	work[32];

	dlgs_setlistuint32(hWnd, IDC_SAMPLERATE, ratehz, NELEMENTS(ratehz));

	OEMSPRINTF(work, str_u, xmilcfg.samplingrate);
	SetDlgItemText(hWnd, IDC_SAMPLERATE, work);
	OEMSPRINTF(work, str_u, xmilcfg.delayms);
	SetDlgItemText(hWnd, IDC_SNDBUFFER, work);
	OEMSPRINTF(work, str_u, xmilcfg.MOTORVOL);
	SetDlgItemText(hWnd, IDC_SEEKVOL, work);

	SetDlgItemCheck(hWnd, IDC_SKIPLINE, xmilcfg.skipline);
	SendDlgItemMessage(hWnd, IDC_SKIPLIGHT, TBM_SETRANGE, TRUE,
														MAKELONG(0, 256));
	SendDlgItemMessage(hWnd, IDC_SKIPLIGHT, TBM_SETPOS, TRUE,
														xmilcfg.skiplight);
	OEMSPRINTF(work, str_d, xmilcfg.skiplight);
	SetDlgItemText(hWnd, IDC_LIGHTSTR, work);
}

static void lightstr(HWND hWnd) {

	UINT	val;
	OEMCHAR	work[32];

	val = (UINT)SendDlgItemMessage(hWnd, IDC_SKIPLIGHT, TBM_GETPOS, 0, 0);
	OEMSPRINTF(work, str_d, val);
	SetDlgItemText(hWnd, IDC_LIGHTSTR, work);
}

static void cfgupdate(HWND hWnd) {

	UINT	updateflag;
	OEMCHAR	work[32];
	UINT16	wval;
	UINT8	bval;
	BOOL	renewalflg;

	updateflag = 0;

	GetDlgItemText(hWnd, IDC_SAMPLERATE, work, NELEMENTS(work));
	wval = (UINT16)LIMITS(milstr_solveINT(work), 11025, 55500);
	if (xmilcfg.samplingrate != wval) {
		xmilcfg.samplingrate = wval;
		updateflag |= SYS_UPDATECFG;
		corestat.soundrenewal = TRUE;
	}

	GetDlgItemText(hWnd, IDC_SNDBUFFER, work, NELEMENTS(work));
	wval = (UINT16)LIMITS(milstr_solveINT(work), 100, 1000);
	if (xmilcfg.delayms != wval) {
		xmilcfg.delayms = wval;
		updateflag |= SYS_UPDATECFG;
		corestat.soundrenewal = TRUE;
	}

	GetDlgItemText(hWnd, IDC_SEEKVOL, work, NELEMENTS(work));
	bval = (UINT8)LIMITS(milstr_solveINT(work), 0, 100);
	if (xmilcfg.MOTORVOL != bval) {
		xmilcfg.MOTORVOL = bval;
		soundmng_pcmvolume(SOUND_PCMSEEK, bval);
		soundmng_pcmvolume(SOUND_PCMSEEK1, bval);
		updateflag |= SYS_UPDATEOSCFG;
	}

	renewalflg = FALSE;
	bval = GetDlgItemCheck(hWnd, IDC_SKIPLINE);
	if (xmilcfg.skipline != bval) {
		xmilcfg.skipline = bval;
		renewalflg = TRUE;
	}
	wval = (UINT16)SendDlgItemMessage(hWnd, IDC_SKIPLIGHT, TBM_GETPOS, 0, 0);
	if (wval > 256) {
		wval = 256;
	}
	if (xmilcfg.skiplight != wval) {
		xmilcfg.skiplight = wval;
		renewalflg = 1;
	}
	if (renewalflg) {
		pal_reset();
		updateflag |= SYS_UPDATECFG;
	}
	sysmng_update(updateflag);
}

LRESULT CALLBACK CfgDialogProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {

	switch (msg) {
		case WM_INITDIALOG:
			cfgcreate(hWnd);
			break;

		case WM_COMMAND:
			switch (LOWORD(wp)) {
				case IDOK:
					cfgupdate(hWnd);
					EndDialog(hWnd, IDOK);
					return(TRUE);

				case IDCANCEL:
					EndDialog(hWnd, IDCANCEL);
					return(TRUE);
			}
			break;

		case WM_HSCROLL:
			switch(GetDlgCtrlID((HWND)lp)) {
				case IDC_SKIPLIGHT:
					lightstr(hWnd);
					return(TRUE);
			}
			break;

		case WM_CLOSE:
			PostMessage(hWnd, WM_COMMAND, IDCANCEL, 0);
			return(TRUE);
	}
	return(FALSE);
}

