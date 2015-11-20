/**
 * @file	d_about.cpp
 * @brief	�o�[�W�������_�C�A���O �N���X�̓���̒�`���s���܂�
 */

#include "compiler.h"
#include "resource.h"
#include "d_about.h"
#include "pccore.h"

/**
 * �R���X�g���N�^
 * @param[in] hwndParent �e�E�B���h�E
 */
CAboutDlg::CAboutDlg(HWND hwndParent)
	: CDlgProc(IDD_ABOUT, hwndParent)
{
}

/**
 * ���̃��\�b�h�� WM_INITDIALOG �̃��b�Z�[�W�ɉ������ČĂяo����܂�
 * @retval TRUE �ŏ��̃R���g���[���ɓ��̓t�H�[�J�X��ݒ�
 * @retval FALSE ���ɐݒ��
 */
BOOL CAboutDlg::OnInitDialog()
{
	std::tstring str = LoadTString(IDS_APP_NAME) + std::tstring(TEXT("  ")) + std::tstring(xmilversion);
#if defined(XMILVER_WIN9X)
	str += std::tstring(XMILVER_WIN9X);
#endif
	SetDlgItemText(IDC_XMILVER, str.c_str());

	return TRUE;
}
