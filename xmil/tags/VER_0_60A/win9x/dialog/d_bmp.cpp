/**
 * @file	d_bmp.cpp
 * @brief	画像ファイル アクセスの動作の定義を行います
 */

#include "compiler.h"
#include "resource.h"
#include "d_bmp.h"
#include "xmil.h"
#include "dosio.h"
#include "sysmng.h"
#include "misc/DlgProc.h"
#include "strres.h"
#include "pccore.h"
#include "iocore.h"
#include "scrnsave.h"

static const TCHAR bmpui_file[] = TEXT("XMIL%04d.BMP");

static const UINT s_nFilterIDs[] =
{
	IDS_BMPFILTER1,
	IDS_BMPFILTER4,
	IDS_BMPFILTER8,
	IDS_BMPFILTER24
};

void dialog_writebmp(HWND hWnd)
{
	SCRNSAVE ss = scrnsave_get();
	if (ss == NULL)
	{
		return;
	}

	TCHAR szPath[MAX_PATH];
	for (UINT i = 0; i < 10000; i++)
	{
		TCHAR szFilename[MAX_PATH];
		wsprintf(szFilename, bmpui_file, i);

		file_cpyname(szPath, fddfolder, _countof(szPath));
		file_cutname(szPath);
		file_catname(szPath, szFilename, _countof(szPath));
		if (file_attr(szPath) == -1)
		{
			break;
		}
	}

	std::tstring rTitle(LoadTString(IDS_BMPTITLE));
	std::tstring rFilter(LoadTString(s_nFilterIDs[ss->type]));
	std::tstring rExt(LoadTString(IDS_BMPEXT));

	CFileDlg dlg(FALSE, rExt.c_str(), szPath, OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY, rFilter.c_str(), hWnd);
	dlg.m_ofn.nFilterIndex = 1;
	if (dlg.DoModal() == IDOK)
	{
		LPCTSTR lpPath = dlg.GetPathName();
		LPCTSTR lpExt = file_getext(lpPath);
		if ((ss->type <= SCRNSAVE_8BIT) && (!file_cmpname(lpExt, TEXT("gif"))))
		{
			scrnsave_writegif(ss, lpPath, SCRNSAVE_AUTO);
		}
		else if (!file_cmpname(lpExt, str_bmp))
		{
			scrnsave_writebmp(ss, lpPath, SCRNSAVE_AUTO);
		}

		file_cpyname(bmpfilefolder, lpPath, _countof(bmpfilefolder));
		sysmng_update(SYS_UPDATEOSCFG);
	}
	scrnsave_trash(ss);
}
