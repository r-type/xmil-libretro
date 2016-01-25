/**
 * @file	d_screen.cpp
 * @brief	�X�N���[���ݒ�_�C�A���O �N���X�̓���̒�`���s���܂�
 */

#include "compiler.h"
#include "d_screen.h"
#include "resource.h"
#include <commctrl.h>
#include <vector>
#include "misc/PropProc.h"
#include "sysmng.h"
#include "pccore.h"
#include "palettes.h"

#if !defined(__GNUC__)
#pragma comment(lib, "comctl32.lib")
#endif	// !defined(__GNUC__)

/**
 * @brief Video �y�[�W
 */
class ScrOptVideoPage : public CPropPageProc
{
public:
	ScrOptVideoPage();

protected:
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual LRESULT WindowProc(UINT nMsg, WPARAM wParam, LPARAM lParam);
};

/**
 * �R���X�g���N�^
 */
ScrOptVideoPage::ScrOptVideoPage()
	: CPropPageProc(IDD_SCROPT)
{
}

/**
 * ���̃��\�b�h�� WM_INITDIALOG �̃��b�Z�[�W�ɉ������ČĂяo����܂�
 * @retval TRUE �ŏ��̃R���g���[���ɓ��̓t�H�[�J�X��ݒ�
 * @retval FALSE ���ɐݒ��
 */
BOOL ScrOptVideoPage::OnInitDialog()
{
	CheckDlgButton(IDC_SKIPLINE, (xmilcfg.skipline) ? BST_CHECKED : BST_UNCHECKED);
	SendDlgItemMessage(IDC_SKIPLIGHT, TBM_SETRANGE, TRUE, MAKELONG(0, 256));
	SendDlgItemMessage(IDC_SKIPLIGHT, TBM_SETPOS, TRUE, xmilcfg.skiplight);
	SetDlgItemInt(IDC_LIGHTSTR, xmilcfg.skiplight);

	return TRUE;
}
/**
 * ���[�U�[�� OK �̃{�^�� (IDOK ID ���̃{�^��) ���N���b�N����ƌĂяo����܂�
 */
void ScrOptVideoPage::OnOK()
{
	UINT nUpdateFlags = 0;

	bool bRenewal = false;
	const UINT8 bSkipline = (IsDlgButtonChecked(IDC_SKIPLINE) != BST_UNCHECKED) ? 1 : 0;
	if (xmilcfg.skipline != bSkipline)
	{
		xmilcfg.skipline = bSkipline;
		bRenewal = true;
	}
	UINT nSkipLight = SendDlgItemMessage(IDC_SKIPLIGHT, TBM_GETPOS);
	if (nSkipLight > 256)
	{
		nSkipLight = 256;
	}
	if (xmilcfg.skiplight != nSkipLight)
	{
		xmilcfg.skiplight = nSkipLight;
		bRenewal = true;
	}
	if (bRenewal)
	{
		pal_reset();
		nUpdateFlags |= SYS_UPDATECFG;
	}
	::sysmng_update(nUpdateFlags);

	CPropPageProc::OnOK();
}

/**
 * CWndProc �I�u�W�F�N�g�� Windows �v���V�[�W�� (WindowProc) ���p�ӂ���Ă��܂�
 * @param[in] nMsg ��������� Windows ���b�Z�[�W���w�肵�܂�
 * @param[in] wParam ���b�Z�[�W�̏����Ŏg���t������񋟂��܂��B���̃p�����[�^�̒l�̓��b�Z�[�W�Ɉˑ����܂�
 * @param[in] lParam ���b�Z�[�W�̏����Ŏg���t������񋟂��܂��B���̃p�����[�^�̒l�̓��b�Z�[�W�Ɉˑ����܂�
 * @return ���b�Z�[�W�Ɉˑ�����l��Ԃ��܂�
 */
LRESULT ScrOptVideoPage::WindowProc(UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	switch (nMsg)
	{
		case WM_HSCROLL:
			if (reinterpret_cast<HWND>(lParam) == GetDlgItem(IDC_SKIPLIGHT))
			{
				SetDlgItemInt(IDC_LIGHTSTR, SendDlgItemMessage(IDC_SKIPLIGHT, TBM_GETPOS));
				return TRUE;
			}
			break;
	}
	return CPropPageProc::WindowProc(nMsg, wParam, lParam);
}

/**
 * �X�N���[���ݒ�
 * @param[in] hWnd �e�E�B���h�E
 */
void dialog_scropt(HWND hWnd)
{
	std::vector<HPROPSHEETPAGE> hpsp;

	ScrOptVideoPage video;
	hpsp.push_back(::CreatePropertySheetPage(&video.m_psp));

	std::tstring rTitle(LoadTString(IDS_SCREENOPTION));

	PROPSHEETHEADER psh;
	ZeroMemory(&psh, sizeof(psh));
	psh.dwSize = sizeof(psh);
	psh.dwFlags = PSH_NOAPPLYNOW | PSH_USEHICON;
	psh.hwndParent = hWnd;
	psh.hInstance = CWndProc::GetResourceHandle();
	psh.hIcon = LoadIcon(psh.hInstance, MAKEINTRESOURCE(IDI_ICON1));
	psh.nPages = hpsp.size();
	psh.phpage = &hpsp.at(0);
	psh.pszCaption = rTitle.c_str();
	::PropertySheet(&psh);
	::InvalidateRect(hWnd, NULL, TRUE);
}
