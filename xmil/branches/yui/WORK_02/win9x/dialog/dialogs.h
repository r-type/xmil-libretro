/**
 * @file	dialogs.h
 * @brief	�_�C�A���O �w���p�[�̐錾����уC���^�[�t�F�C�X�̒�`�����܂�
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
 * @brief �R���{ �{�b�N�X �N���X
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
