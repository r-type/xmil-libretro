/**
 * @file	externalpsg.cpp
 * @brief	�O�� PSG ���t�N���X�̓���̒�`���s���܂�
 */

#include "compiler.h"
#include "externalpsg.h"

/**
 * �R���X�g���N�^
 * @param[in] pChip �`�b�v
 */
CExternalPsg::CExternalPsg(IExternalChip* pChip)
	: CExternalChipEvent(pChip)
	, m_cPsgMix(0x3f)
{
}

/**
 * �f�X�g���N�^
 */
CExternalPsg::~CExternalPsg()
{
}

/**
 * �������Z�b�g
 */
void CExternalPsg::Reset()
{
	m_cPsgMix = 0x3f;
	CExternalChipEvent::Reset();
}

/**
 * ���W�X�^��������
 * @param[in] timestamp �^�C���X�^���v
 * @param[in] nAddr �A�h���X
 * @param[in] cData �f�[�^
 */
void CExternalPsg::WriteRegisterEvent(ExternalChipTimestamp timestamp, UINT nAddr, UINT8 cData)
{
	if (nAddr < 0x0e)
	{
		if (nAddr == 0x07)
		{
			// psg mix
			m_cPsgMix = cData;
		}
		CExternalChipEvent::WriteRegisterEvent(timestamp, nAddr, cData);
	}
}

/**
 * ���b�Z�[�W
 * @param[in] nMessage ���b�Z�[�W
 * @param[in] nParameter �p�����[�^
 * @return ����
 */
INTPTR CExternalPsg::Message(UINT nMessage, INTPTR nParameter)
{
	switch (nMessage)
	{
		case kMute:
			Mute(nParameter != 0);
			break;
	}
	return 0;
}

/**
 * �~���[�g
 * @param[in] bMute �~���[�g
 */
void CExternalPsg::Mute(bool bMute)
{
	CExternalChipEvent::WriteRegisterEvent(0, 0x07, (bMute) ? 0x3f : m_cPsgMix);
}
