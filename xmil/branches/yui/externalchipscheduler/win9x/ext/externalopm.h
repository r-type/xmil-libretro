/**
 * @file	externalopm.h
 * @brief	�O�� OPM ���t�N���X�̐錾����уC���^�[�t�F�C�X�̒�`�����܂�
 */

#pragma once

#include "externalchipevent.h"

/**
 * @brief �O�� OPM ���t�N���X
 */
class CExternalOpm : public CExternalChipEvent
{
public:
	CExternalOpm(IExternalChip* pChip);
	virtual ~CExternalOpm();
	virtual void Reset();
	virtual void WriteRegisterEvent(ExternalChipTimestamp timestamp, UINT nAddr, UINT8 cData);
	virtual INTPTR Message(UINT nMessage, INTPTR nParameter);

protected:
	UINT8 m_cAlgorithm[8];				//!< �A���S���Y�� �e�[�u��
	UINT8 m_cTtl[8 * 4];				//!< TTL �e�[�u��

	void Mute(bool bMute);
	void SetVolume(UINT nChannel, int nVolume);
};
