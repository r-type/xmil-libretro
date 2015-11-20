/**
 * @file	d_about.cpp
 * @brief	バージョン情報ダイアログ クラスの動作の定義を行います
 */

#include "compiler.h"
#include "resource.h"
#include "d_about.h"
#include "pccore.h"

/**
 * コンストラクタ
 * @param[in] hwndParent 親ウィンドウ
 */
CAboutDlg::CAboutDlg(HWND hwndParent)
	: CDlgProc(IDD_ABOUT, hwndParent)
{
}

/**
 * このメソッドは WM_INITDIALOG のメッセージに応答して呼び出されます
 * @retval TRUE 最初のコントロールに入力フォーカスを設定
 * @retval FALSE 既に設定済
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
