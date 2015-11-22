/**
 * @file	d_disk.cpp
 * @brief	�f�B�X�N �A�N�Z�X�̓���̒�`���s���܂�
 */

#include "compiler.h"
#include "d_disk.h"
#include "resource.h"
#include "xmil.h"
#include "dosio.h"
#include "sysmng.h"
#include "misc/DlgProc.h"
#include "strres.h"
#include "diskdrv.h"
#include "fddfile.h"
#include "newdisk.h"

static const TCHAR str_newdisk[] = TEXT("newdisk");

/**
 * @brief �V�����f�B�X�N �_�C�A���O
 */
class CNewDiskDlg : public CDlgProc
{
public:
	CNewDiskDlg(HWND hwndParent, LPCTSTR lpPath);

protected:
	virtual BOOL OnInitDialog();
	virtual void OnOK();

private:
	LPCTSTR m_lpPath;			//!< �p�X
};

/**
 * �R���X�g���N�^
 * @param[in] hwndParent �e�E�B���h�E
 * @param[in] lpPath �p�X
 */
CNewDiskDlg::CNewDiskDlg(HWND hwndParent, LPCTSTR lpPath)
	: CDlgProc(IDD_NEWDISK, hwndParent)
	, m_lpPath(lpPath)
{
}

/**
 * ���̃��\�b�h�� WM_INITDIALOG �̃��b�Z�[�W�ɉ������ČĂяo����܂�
 * @retval TRUE �ŏ��̃R���g���[���ɓ��̓t�H�[�J�X��ݒ�
 * @retval FALSE ���ɐݒ��
 */
BOOL CNewDiskDlg::OnInitDialog()
{
	CheckDlgButton(IDC_MAKE2D, BST_CHECKED);
	GetDlgItem(IDC_DISKLABEL).SetFocus();
	return FALSE;
}

/**
 * ���[�U�[�� OK �̃{�^�� (IDOK ID ���̃{�^��) ���N���b�N����ƌĂяo����܂�
 */
void CNewDiskDlg::OnOK()
{
	TCHAR szLabel[32];
	GetDlgItemText(IDC_DISKLABEL, szLabel, _countof(szLabel));

	const UINT nMakeFdType = (IsDlgButtonChecked(IDC_MAKE2D) != BST_UNCHECKED) ? (DISKTYPE_2D << 4) : (DISKTYPE_2HD << 4);

	newdisk_fdd(m_lpPath, nMakeFdType, szLabel);

	CDlgProc::OnOK();
}



// ----

/**
 * �t���b�s�[ �f�B�X�N�}��
 * @param[in] hWnd �e�E�B���h�E
 * @param[in] drv �h���C�u
 */
void dialog_changefdd(HWND hWnd, REG8 drv)
{
	LPCTSTR lpDefaultPath = fddfile_diskname(drv);
	if ((lpDefaultPath == NULL) || (lpDefaultPath[0] == TEXT('\0')))
	{
		lpDefaultPath = fddfolder;
	}

	std::tstring rTitle(LoadTString(IDS_FDDTITLE));
	std::tstring rFilter(LoadTString(IDS_FDDFILTER));
	std::tstring rExt(LoadTString(IDS_FDDEXT));

	CFileDlg dlg(TRUE, rExt.c_str(), lpDefaultPath, OFN_FILEMUSTEXIST, rFilter.c_str(), hWnd);
	dlg.m_ofn.nFilterIndex = 3;
	if (dlg.DoModal() == IDOK)
	{
		LPCTSTR lpPath = dlg.GetPathName();
		file_cpyname(fddfolder, lpPath, _countof(fddfolder));
		sysmng_update(SYS_UPDATEOSCFG);
		diskdrv_setfdd(drv, lpPath, dlg.GetReadOnlyPref());
	}
}



/**
 * �t���b�s�[ �f�B�X�N�̍쐬
 * @param[in] hWnd �e�E�B���h�E
 */
void dialog_newdisk(HWND hWnd)
{
	TCHAR szPath[MAX_PATH];
	file_cpyname(szPath, fddfolder, _countof(szPath));
	file_cutname(szPath);
	file_catname(szPath, str_newdisk, _countof(szPath));

	std::tstring rTitle(LoadTString(IDS_NEWDISKTITLE));
	std::tstring rFilter(LoadTString(IDS_NEWDISKFILTER));
	std::tstring rExt(LoadTString(IDS_NEWDISKEXT));

	CFileDlg dlg(FALSE, rExt.c_str(), szPath, OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY, rFilter.c_str(), hWnd);
	dlg.m_ofn.nFilterIndex = 1;
	if (dlg.DoModal() == IDOK)
	{
		LPCTSTR lpPath = dlg.GetPathName();
		LPCTSTR lpExt = file_getext(lpPath);
		if ((file_cmpname(lpExt, str_d88) == 0) || (file_cmpname(lpExt, str_88d) == 0))
		{
			CNewDiskDlg dlg2(hWnd, lpPath);
			dlg2.DoModal();
		}
	}
}
