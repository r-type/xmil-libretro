/**
 * @file	d_cfg.h
 * @brief	コンフィグ ダイアログ クラスの宣言およびインターフェイスの定義をします
 */

#pragma once

#include "misc/DlgProc.h"

/**
 * @brief バージョン情報ダイアログ
 */
class CConfigDlg : public CDlgProc
{
public:
	static void Config(HWND hwndParent);
	CConfigDlg(HWND hwndParent);

protected:
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual LRESULT WindowProc(UINT nMsg, WPARAM wParam, LPARAM lParam);
};

/**
 * ダイアログ ボックスを開く
 * @param[in] hwndParent 親ウィンドウ
 */
inline void CConfigDlg::Config(HWND hwndParent)
{
	CConfigDlg dlg(hwndParent);
	dlg.DoModal();
}
