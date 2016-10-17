/**
 * @file	dialogs.h
 * @brief	ダイアログ ヘルパーの宣言およびインターフェイスの定義をします
 */

#pragma once

#include "misc/DlgProc.h"

struct tagCBParam
{
	LPCTSTR	lpcszString;
	int		nItemData;
};
typedef struct tagCBParam		CBPARAM;
typedef struct tagCBParam		*PCBPARAM;
typedef const struct tagCBParam	*PCCBPARAM;

/**
 * @brief コンボ ボックス クラス
 */
class CNp2ComboBox : public CComboBoxProc
{
public:
	void Add(const UINT32* lpValues, UINT nCount);
	void Add(PCCBPARAM pcItem, UINT nCount);
	void Add(LPCTSTR lpString, DWORD_PTR nItemData);
	void SetCurItemData(UINT32 nValue);
	UINT32 GetCurItemData(UINT32 nDefault) const;
};
