#include	"compiler.h"
#include	"resource.h"
#include	"dialog.h"
#include	"dialogs.h"
#include	"xmilver.h"
#include	"pccore.h"


#if TARGET_API_MAC_CARBON
static const char subversion[] = " (Carbon)";
#else
static const char subversion[] = " (Classic)";
#endif

void AboutDialogProc(void) {

	DialogPtr	hDlg;
	char		work[64];
	Str255		verstr;
	int			done;
	short		item;

	hDlg = GetNewDialog(IDD_ABOUT, NULL, (WindowPtr)-1);
	if (!hDlg) {
		return;
	}
	milstr_ncpy(work, xmilversion, sizeof(work));
#if defined(XMILVER_MACOS9)
	milstr_ncat(work, XMILVER_MACOS9, sizeof(work));
#endif
	milstr_ncat(work, subversion, sizeof(work));
	mkstr255(verstr, work);
	SetDialogItemText(GetDlgItem(hDlg, IDD_VERSION), verstr);
	SetDialogDefaultItem(hDlg, IDOK);

	done = 0;
	while(!done) {
		ModalDialog(NULL, &item);
		switch(item) {
			case IDOK:
				done = 1;
				break;
		}
	}
	DisposeDialog(hDlg);
}

