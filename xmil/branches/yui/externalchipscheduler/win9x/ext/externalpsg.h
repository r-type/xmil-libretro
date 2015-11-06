/**
 * @file	externalpsg.h
 * @brief	�O�� PSG ���t�N���X�̐錾����уC���^�[�t�F�C�X�̒�`�����܂�
 */

#pragma once

#include "externalchipevent.h"

/**
 * @brief �O�� PSG ���t�N���X
 */
class CExternalPsg : public CExternalChipEvent
{
public:
	CExternalPsg(IExternalChip* pChip);
	virtual ~CExternalPsg();
	virtual void Reset();
	virtual void WriteRegisterEvent(ExternalChipTimestamp timestamp, UINT nAddr, UINT8 cData);
	virtual INTPTR Message(UINT nMessage, INTPTR nParameter);

protected:
	UINT8 m_cPsgMix;					//!< PSG �~�L�T�[

	virtual void Mute(bool bMute);
};
