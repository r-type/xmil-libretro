#include	"compiler.h"
#include	"strres.h"
#include	"resource.h"
#include	"xmil.h"
#include	"sysmng.h"
#include	"dialog.h"
#include	"dialogs.h"
#include	"pccore.h"
#include	"palettes.h"


static void setrate(ControlHandle *btn, UINT rate) {

	SetControlValue(btn[0], (rate == 11025)?1:0);
	SetControlValue(btn[1], (rate == 22050)?1:0);
	SetControlValue(btn[2], (rate == 44100)?1:0);
}


void ConfigDialogProc(void) {

	DialogPtr		hDlg;
	ControlHandle	ratebtn[3];
	ControlHandle	skipbtn;
	char			work[32];
	Str255			workstr;
	UINT			rate;
	UINT			ms;
	UINT			skipline;
	UINT			val;
	int				done;
	SInt16			item;
	UINT			update;
	UINT			renewal;

	hDlg = GetNewDialog(IDD_CONFIGURE, NULL, (WindowPtr)-1);
	if (!hDlg) {
		return;
	}
	ratebtn[0] = (ControlHandle)GetDlgItem(hDlg, IDC_RATE11);
	ratebtn[1] = (ControlHandle)GetDlgItem(hDlg, IDC_RATE22);
	ratebtn[2] = (ControlHandle)GetDlgItem(hDlg, IDC_RATE44);
	skipbtn = (ControlHandle)GetDlgItem(hDlg, IDC_SKIPLINE);

	rate = xmilcfg.samplingrate;
	if ((rate != 0) && (rate != 11025) && (rate != 44100)) {
		rate = 22050;
	}
	setrate(ratebtn, rate);
	ms = xmilcfg.delayms;
	if (ms < 50) {
		ms = 50;
	}
	else if (ms > 1000) {
		ms = 1000;
	}
	SPRINTF(work, str_u, ms);
	mkstr255(workstr, work);
	SetDialogItemText(GetDlgItem(hDlg, IDC_SOUNDBUF), workstr);

	skipline = (xmilcfg.skipline)?1:0;
	SetControlValue(skipbtn, skipline);
	SPRINTF(work, str_u, xmilcfg.skiplight);
	mkstr255(workstr, work);
	SetDialogItemText(GetDlgItem(hDlg, IDC_SKIPLIGHT), workstr);

	SetDialogDefaultItem(hDlg, IDOK);
	SetDialogCancelItem(hDlg, IDCANCEL);

	done = 0;
	while(!done) {
		ModalDialog(NULL, &item);
		switch(item) {
			case IDOK:
				update = 0;
				if (xmilcfg.samplingrate != rate) {
					xmilcfg.samplingrate = rate;
					update |= SYS_UPDATECFG;
					corestat.soundrenewal = 1;
				}
				GetDialogItemText(GetDlgItem(hDlg, IDC_SOUNDBUF), workstr);
				mkcstr(work, sizeof(work), workstr);
				ms = milstr_solveINT(work);
				if (ms < 50) {
					ms = 50;
				}
				else if (ms > 1000) {
					ms = 1000;
				}
				if (xmilcfg.delayms != ms) {
					xmilcfg.delayms = ms;
					corestat.soundrenewal = 1;
					update |= SYS_UPDATECFG;
				}

				renewal = 0;
				if (xmilcfg.skipline != skipline) {
					xmilcfg.skipline = skipline;
					renewal = 1;
				}
				GetDialogItemText(GetDlgItem(hDlg, IDC_SKIPLIGHT), workstr);
				mkcstr(work, sizeof(work), workstr);
				val = milstr_solveINT(work);
				if (val > 256) {
					val = 256;
				}
				if (xmilcfg.skiplight != (UINT16)val) {
					xmilcfg.skiplight = (UINT16)val;
					renewal = 1;
				}
				if (renewal) {
					pal_reset();
					update |= SYS_UPDATECFG;
				}
				sysmng_update(update);
				done = IDOK;
				break;

			case IDCANCEL:
				done = IDCANCEL;
				break;

			case IDC_RATE11:
				rate = 11025;
				setrate(ratebtn, rate);
				break;

			case IDC_RATE22:
				rate = 22050;
				setrate(ratebtn, rate);
				break;

			case IDC_RATE44:
				rate = 44100;
				setrate(ratebtn, rate);
				break;

			case IDC_SKIPLINE:
				skipline ^= 1;
				SetControlValue(skipbtn, skipline);
				break;
		}
	}
	DisposeDialog(hDlg);
}

