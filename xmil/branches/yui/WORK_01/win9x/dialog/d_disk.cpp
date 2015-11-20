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
#include "misc/DlgProc.h"

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

/**
 * @brief 新しいディスク ダイアログ
 */
class CNewDiskDlg : public CDlgProc
{
public:
	CNewDiskDlg(HWND hwndParent, LPCTSTR lpPath);

protected:
	virtual BOOL OnInitDialog();
	virtual void OnOK();

private:
	LPCTSTR m_lpPath;			//!< パス
};

/**
 * コンストラクタ
 * @param[in] hwndParent 親ウィンドウ
 * @param[in] lpPath パス
 */
CNewDiskDlg::CNewDiskDlg(HWND hwndParent, LPCTSTR lpPath)
	: CDlgProc(IDD_NEWDISK, hwndParent)
	, m_lpPath(lpPath)
{
}

/**
 * このメソッドは WM_INITDIALOG のメッセージに応答して呼び出されます
 * @retval TRUE 最初のコントロールに入力フォーカスを設定
 * @retval FALSE 既に設定済
 */
BOOL CNewDiskDlg::OnInitDialog()
{
	CheckDlgButton(IDC_MAKE2D, BST_CHECKED);
	GetDlgItem(IDC_DISKLABEL).SetFocus();
	return FALSE;
}

/**
 * ユーザーが OK のボタン (IDOK ID がのボタン) をクリックすると呼び出されます
 */
void CNewDiskDlg::OnOK()
{
	TCHAR szLabel[32];
	GetDlgItemText(IDC_DISKLABEL, szLabel, _countof(szLabel));

	const UINT nMakeFdType = (IsDlgButtonChecked(IDC_MAKE2D) != BST_UNCHECKED) ? (DISKTYPE_2D << 4) : (DISKTYPE_2HD << 4);

	newdisk_fdd(m_lpPath, nMakeFdType, szLabel);

	CDlgProc::OnOK();
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
	if ((!file_cmpname(ext, str_d88)) || (!file_cmpname(ext, str_88d)))
	{
		CNewDiskDlg dlg(hWnd, path);
		dlg.DoModal();
#if 0
		{
			newdisk_fdd(path, makefdtype, disklabel);
		}
#endif
	}
}

