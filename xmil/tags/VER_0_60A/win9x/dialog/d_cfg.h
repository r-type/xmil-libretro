/**
 * @file	d_cfg.h
 * @brief	�R���t�B�O �_�C�A���O �N���X�̐錾����уC���^�[�t�F�C�X�̒�`�����܂�
 */

#pragma once

#include "misc/DlgProc.h"

/**
 * @brief �o�[�W�������_�C�A���O
 */
class CConfigDlg : public CDlgProc
{
public:
	static void Config(HWND hwndParent);
	CConfigDlg(HWND hwndParent);

protected:
	virtual BOOL OnInitDialog();
	virtual void OnOK();
};

/**
 * �_�C�A���O �{�b�N�X���J��
 * @param[in] hwndParent �e�E�B���h�E
 */
inline void CConfigDlg::Config(HWND hwndParent)
{
	CConfigDlg dlg(hwndParent);
	dlg.DoModal();
}
