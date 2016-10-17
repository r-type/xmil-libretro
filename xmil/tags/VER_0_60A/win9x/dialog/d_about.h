/**
 * @file	d_about.h
 * @brief	バージョン情報ダイアログ クラスの宣言およびインターフェイスの定義をします
 */

#pragma once

#include "misc/DlgProc.h"

/**
 * @brief バージョン情報ダイアログ
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
 * ダイアログ ボックスを開く
 * @param[in] hwndParent 親ウィンドウ
 */
inline void CAboutDlg::About(HWND hwndParent)
{
	CAboutDlg dlg(hwndParent);
	dlg.DoModal();
}
