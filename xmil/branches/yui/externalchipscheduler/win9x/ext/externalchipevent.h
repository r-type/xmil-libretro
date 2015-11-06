/**
 * @file	externalchipevent.h
 * @brief	�O�����t�C�x���g���N���X�̐錾����уC���^�[�t�F�C�X�̒�`�����܂�
 */

#pragma once

#include "externalchip.h"
#include "externalchiptimestamp.h"

/**
 * @brief �O�����t�C�x���g���N���X
 */
class CExternalChipEvent : public IExternalChip
{
public:
	CExternalChipEvent(IExternalChip* pChip);
	virtual ~CExternalChipEvent();
	virtual ChipType GetChipType();
	virtual void Reset();
	virtual void WriteRegister(UINT nAddr, UINT8 cData);
	virtual void WriteRegisterEvent(ExternalChipTimestamp timestamp, UINT nAddr, UINT8 cData);

private:
	IExternalChip* m_pChip;				//!< �`�b�v
};
