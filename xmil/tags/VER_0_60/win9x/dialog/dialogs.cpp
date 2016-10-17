/**
 * @file	dialogs.cpp
 * @brief	ダイアログ ヘルパーの動作の定義を行います
 */

#include "compiler.h"
#include "dialogs.h"
#include "strres.h"

void CNp2ComboBox::Add(const UINT32* lpValues, UINT nCount)
{
	for (UINT i = 0; i < nCount; i++)
	{
		TCHAR str[16];
		wsprintf(str, str_u, lpValues[i]);
		InsertString(GetCount(), str);
	}
}

void CNp2ComboBox::Add(PCCBPARAM pcItem, UINT nCount)
{
	for (UINT i = 0; i <nCount; i++)
	{
		std::tstring rString(LoadTString(pcItem[i].lpcszString));
		Add(rString.c_str(), pcItem[i].nItemData);
	}
}

void CNp2ComboBox::Add(LPCTSTR lpString, DWORD_PTR nItemData)
{
	const int nIndex = AddString(lpString);
	if (nIndex >= 0)
	{
		SetItemData(nIndex, nItemData);
	}
}

void CNp2ComboBox::SetCurItemData(UINT32 nValue)
{
	const int nItems = GetCount();
	for (int i = 0; i < nItems; i++)
	{
		if (GetItemData(i) == nValue)
		{
			SetCurSel(i);
			break;
		}
	}
}

UINT32 CNp2ComboBox::GetCurItemData(UINT32 nDefault) const
{
	int nIndex = GetCurSel();
	if (nIndex >= 0)
	{
		return GetItemData(nIndex);
	}
	return nDefault;
}
