#include	"compiler.h"
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

	OEMCHAR		work[32];
	WORD		res;

	dlgs_setlistuint32(hWnd, IDC_SAMPLERATE, ratehz, NELEMENTS(ratehz));

	SPRINTF(work, str_u, xmilcfg.samplingrate);
	SetDlgItemText(hWnd, IDC_SAMPLERATE, work);
	SPRINTF(work, str_u, xmilcfg.delayms);
	SetDlgItemText(hWnd, IDC_SNDBUFFER, work);
	SPRINTF(work, str_u, xmilcfg.MOTORVOL);
	SetDlgItemText(hWnd, IDC_SEEKVOL, work);
	if (xmilcfg.TEXTMODE) {
		res = IDC_TXTENHANCED;
	}
	else {
		res = IDC_TXTREAL;
	}
	SetDlgItemCheck(hWnd, res, TRUE);
	switch(xmilcfg.TEXT400L) {
		case 0:
			res = IDC_TXTEXTEND;
			break;

		case 1:
			res = IDC_TXT400;
			break;

		default:
			res = IDC_TXTCSTM;
			break;
	}
	SetDlgItemCheck(hWnd, res, TRUE);
	EnableWindow(GetDlgItem(hWnd, IDC_TXTVALUE), (res == IDC_TXTCSTM));

	SPRINTF(work, str_u, xmilcfg.LINETEXT);
	SetDlgItemText(hWnd, IDC_TXTVALUE, work);
	SPRINTF(work, str_u, xmilcfg.BLKLIGHT);
	SetDlgItemText(hWnd, IDC_BLKLIGHT, work);
	SPRINTF(work, str_u, xmilcfg.LINEDEPTH);
	SetDlgItemText(hWnd, IDC_LINEDEPTH, work);
}

static void cfgupdate(HWND hWnd) {

	UINT	updateflag;
	OEMCHAR	work[32];
	UINT16	wval;
	UINT8	bval;
	BRESULT	renewalflg;

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

	bval = (UINT8)GetDlgItemCheck(hWnd, IDC_TXTENHANCED);
	if (xmilcfg.TEXTMODE != bval) {
		xmilcfg.TEXTMODE = bval;
		updateflag |= SYS_UPDATECFG;
	}

	renewalflg = FALSE;
	if (GetDlgItemCheck(hWnd, IDC_TXTEXTEND)) {
		bval = 0;
	}
	else if (GetDlgItemCheck(hWnd, IDC_TXT400)) {
		bval = 1;
	}
	else {
		bval = 2;
	}
	if (xmilcfg.TEXT400L != bval) {
		xmilcfg.TEXT400L = bval;
		renewalflg = TRUE;
	}
	GetDlgItemText(hWnd, IDC_TXTVALUE, work, NELEMENTS(work));
	bval = (UINT8)LIMITS(milstr_solveINT(work), 0, 255);
	if (xmilcfg.LINETEXT != bval) {
		xmilcfg.LINETEXT = bval;
		renewalflg = TRUE;
	}
	GetDlgItemText(hWnd, IDC_BLKLIGHT, work, NELEMENTS(work));
	bval = (UINT8)LIMITS(milstr_solveINT(work), 0, 255);
	if (xmilcfg.BLKLIGHT != bval) {
		xmilcfg.BLKLIGHT = bval;
		renewalflg = TRUE;
	}
	GetDlgItemText(hWnd, IDC_LINEDEPTH, work, NELEMENTS(work));
	bval = (UINT8)LIMITS(milstr_solveINT(work), 0, 255);
	if (xmilcfg.LINEDEPTH != bval) {
		xmilcfg.LINEDEPTH = bval;
		renewalflg = TRUE;
	}
	if (renewalflg) {
		pal_reset();
		makescrn.palandply = 1;
		updateflag |= SYS_UPDATECFG;
	}
	sysmng_update(updateflag);
}

LRESULT CALLBACK CfgDialogProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {

	switch (msg) {
		case WM_INITDIALOG:
			cfgcreate(hWnd);
			return(FALSE);

		case WM_COMMAND:
			switch (LOWORD(wp)) {
				case IDOK:
					cfgupdate(hWnd);
					EndDialog(hWnd, IDOK);
					break;

				case IDCANCEL:
					EndDialog(hWnd, IDCANCEL);
					break;

				case IDC_TXTEXTEND:
				case IDC_TXT400:
				case IDC_TXTCSTM:
					EnableWindow(GetDlgItem(hWnd, IDC_TXTVALUE),
										GetDlgItemCheck(hWnd, IDC_TXTCSTM));
					return(FALSE);

				default:
					return(FALSE);
			}
			break;

		case WM_CLOSE:
			PostMessage(hWnd, WM_COMMAND, IDCANCEL, 0);
			break;

		default:
			return(FALSE);
	}
	return(TRUE);
}

