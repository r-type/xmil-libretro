/**
 * @file	d_sound.cpp
 * @brief	サウンド ファイル アクセスの動作の定義を行います
 */

#include "compiler.h"

#if defined(SUPPORT_X1F)
#include "d_sound.h"
#include "resource.h"
#include "xmil.h"
#include "dosio.h"
#include "sysmng.h"
#include "misc/DlgProc.h"
#include "x1f.h"

static const TCHAR x1fui_file[] = TEXT("xmil%04d.x1f");

/**
 * X1F ファイル選択
 * @param[in] hWnd 親ウィンドウ
 */
void dialog_x1f(HWND hWnd)
{
	x1f_close();

	TCHAR szPath[MAX_PATH];
	for (UINT i = 0; i < 10000; i++)
	{
		TCHAR szFilename[MAX_PATH];
		wsprintf(szFilename, x1fui_file, i);

		file_cpyname(szPath, fddfolder, _countof(szPath));
		file_cutname(szPath);
		file_catname(szPath, szFilename, _countof(szPath));
		if (file_attr(szPath) == -1)
		{
			break;
		}
	}

	std::tstring rTitle(LoadTString(IDS_X1FTITLE));
	std::tstring rFilter(LoadTString(IDS_X1FFILTER));
	std::tstring rExt(LoadTString(IDS_X1FEXT));

	CFileDlg dlg(FALSE, rExt.c_str(), szPath, OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY, rFilter.c_str(), hWnd);
	dlg.m_ofn.nFilterIndex = 1;
	if (dlg.DoModal() == IDOK)
	{
		LPCTSTR lpPath = dlg.GetPathName();
		if (x1f_open(lpPath) == SUCCESS)
		{
			file_cpyname(bmpfilefolder, lpPath, _countof(bmpfilefolder));
			sysmng_update(SYS_UPDATEOSCFG);
		}
	}
}
#endif
