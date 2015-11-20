/**
 * @file	d_about.h
 * @brief	�o�[�W�������_�C�A���O �N���X�̐錾����уC���^�[�t�F�C�X�̒�`�����܂�
 */

#pragma once

#include "misc/DlgProc.h"

/**
 * @brief �o�[�W�������_�C�A���O
 */
class CAboutDlg : public CDlgProc
{
public:
	static void About(HWND hwndParent);
	CAboutDlg(HWND hwndParent);

protected:
	virtual BOOL OnInitDialog();
};

/**
 * �_�C�A���O �{�b�N�X���J��
 * @param[in] hwndParent �e�E�B���h�E
 */
inline void CAboutDlg::About(HWND hwndParent)
{
	CAboutDlg dlg(hwndParent);
	dlg.DoModal();
}
