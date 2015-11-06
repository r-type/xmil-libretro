#include	"compiler.h"
#include	"strres.h"
#include	"xmil.h"
#include	"resource.h"
#include	"dosio.h"
#include	"sysmng.h"
#include	"dialog.h"
#include	"dialogs.h"
#include	"diskdrv.h"
#include	"newdisk.h"
#include	"fddfile.h"


static const OEMCHAR fddui_title[] = OEMTEXT("Select floppy image");
static const OEMCHAR fddui_filter[] = OEMTEXT("2D image files (*.2D)\0*.2d\0D88 image files (*.D88;*.88D)\0*.d88;*.88d\0All supported Files\0*.2d;*.d88;*.88d;*.d8u\0All files (*.*)\0*.*\0\0");
static const FILESEL fddui = {fddui_title, str_d88, fddui_filter, 3};

static const OEMCHAR newdisk_title[] = OEMTEXT("Create disk image");
static const OEMCHAR newdisk_filter[] = OEMTEXT("D88 image files (*.D88;*.88D)\0*.d88;*.88d\0");
static const FILESEL newdiskui = {newdisk_title, str_d88, newdisk_filter, 1};


// ----

void dialog_changefdd(HWND hWnd, REG8 drv) {

const OEMCHAR	*p;
	OEMCHAR		path[MAX_PATH];
	int			readonly;

	p = fddfile_diskname(drv);
	if ((p == NULL) || (p[0] == '\0')) {
		p = fddfolder;
	}
	file_cpyname(path, p, _countof(path));
	if (dlgs_selectfile(hWnd, &fddui, path, NELEMENTS(path), &readonly)) {
		file_cpyname(fddfolder, path, NELEMENTS(fddfolder));
		sysmng_update(SYS_UPDATEOSCFG);
		diskdrv_setfdd(drv, path, readonly);
	}
}


// ----

static const OEMCHAR str_newdisk[] = OEMTEXT("newdisk");

static	UINT8	makefdtype = DISKTYPE_2D << 4;
static	OEMCHAR	disklabel[16+1];

static LRESULT CALLBACK NewdiskDlgProc(HWND hWnd, UINT msg,
													WPARAM wp, LPARAM lp) {

	UINT16	res;

	switch (msg) {
		case WM_INITDIALOG:
			if (makefdtype == (DISKTYPE_2D << 4)) {
				res = IDC_MAKE2D;
			}
			else {
				res = IDC_MAKE2HD;
			}
			SetDlgItemCheck(hWnd, res, 1);
			SetFocus(GetDlgItem(hWnd, IDC_DISKLABEL));
			return(FALSE);

		case WM_COMMAND:
			switch (LOWORD(wp)) {
				case IDOK:
					GetWindowText(GetDlgItem(hWnd, IDC_DISKLABEL),
											disklabel, NELEMENTS(disklabel));
					if (milstr_kanji1st(disklabel, NELEMENTS(disklabel) - 1)) {
						disklabel[NELEMENTS(disklabel) - 1] = '\0';
					}
					if (GetDlgItemCheck(hWnd, IDC_MAKE2D)) {
						makefdtype = (DISKTYPE_2D << 4);
					}
					else {
						makefdtype = (DISKTYPE_2HD << 4);
					}
					EndDialog(hWnd, IDOK);
					break;

				case IDCANCEL:
					EndDialog(hWnd, IDCANCEL);
					break;

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

void dialog_newdisk(HWND hWnd) {

	OEMCHAR		path[MAX_PATH];
	HINSTANCE	hinst;
const OEMCHAR	*ext;

	file_cpyname(path, fddfolder, NELEMENTS(path));
	file_cutname(path);
	file_catname(path, str_newdisk, NELEMENTS(path));

	if (!dlgs_selectwritefile(hWnd, &newdiskui, path, NELEMENTS(path))) {
		return;
	}
	hinst = reinterpret_cast<HINSTANCE>(GetWindowLongPtr(hWnd, GWLP_HINSTANCE));
	ext = file_getext(path);
	if ((!file_cmpname(ext, str_d88)) || (!file_cmpname(ext, str_88d))) {
		if (DialogBox(hinst, MAKEINTRESOURCE(IDD_NEWDISK),
									hWnd, (DLGPROC)NewdiskDlgProc) == IDOK) {
			newdisk_fdd(path, makefdtype, disklabel);
		}
	}
}

