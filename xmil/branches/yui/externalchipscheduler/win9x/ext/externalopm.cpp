/**
 * @file	externalopm.cpp
 * @brief	�O�� OPM ���t�N���X�̓���̒�`���s���܂�
 */

#include "compiler.h"
#include "externalopm.h"

/**
 * �R���X�g���N�^
 * @param[in] pChip �`�b�v
 */
CExternalOpm::CExternalOpm(IExternalChip* pChip)
	: CExternalChipEvent(pChip)
{
	memset(m_cAlgorithm, 0, sizeof(m_cAlgorithm));
	memset(m_cTtl, 0x7f, sizeof(m_cTtl));
}

/**
 * �f�X�g���N�^
 */
CExternalOpm::~CExternalOpm()
{
}

/**
 * �������Z�b�g
 */
void CExternalOpm::Reset()
{
	memset(m_cAlgorithm, 0, sizeof(m_cAlgorithm));
	memset(m_cTtl, 0x7f, sizeof(m_cTtl));
	CExternalChipEvent::Reset();
}

/**
 * ���W�X�^��������
 * @param[in] timestamp �^�C���X�^���v
 * @param[in] nAddr �A�h���X
 * @param[in] cData �f�[�^
 */
void CExternalOpm::WriteRegisterEvent(ExternalChipTimestamp timestamp, UINT nAddr, UINT8 cData)
{
	if ((nAddr & 0xe0) == 0x60)					// ttl
	{
		m_cTtl[nAddr & 0x1f] = cData;
	}
	else if ((nAddr & 0xf8) == 0x20)			// algorithm
	{
		m_cAlgorithm[nAddr & 7] = cData;
	}
	CExternalChipEvent::WriteRegisterEvent(timestamp, nAddr, cData);
}

/**
 * ���b�Z�[�W
 * @param[in] nMessage ���b�Z�[�W
 * @param[in] nParameter �p�����[�^
 * @return ����
 */
INTPTR CExternalOpm::Message(UINT nMessage, INTPTR nParameter)
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
void CExternalOpm::Mute(bool bMute)
{
	const int nVolume = (bMute) ? -127 : 0;
	for (UINT ch = 0; ch < 8; ch++)
	{
		SetVolume(ch, nVolume);
	}
}

/**
 * ���H�����[���ݒ�
 * @param[in] nChannel �`�����l��
 * @param[in] nVolume ���H�����[���l
 */
void CExternalOpm::SetVolume(UINT nChannel, int nVolume)
{
	//! �A���S���Y�� �X���b�g �}�X�N
	static const UINT8 s_opmask[] = {0x08, 0x08, 0x08, 0x08, 0x0c, 0x0e, 0x0e, 0x0f};
	UINT8 cMask = s_opmask[m_cAlgorithm[nChannel] & 7];

	int nOffset = nChannel;
	do
	{
		if (cMask & 1)
		{
			int nTtl = (m_cTtl[nOffset] & 0x7f) - nVolume;
			if (nTtl < 0)
			{
				nTtl = 0;
			}
			else if (nTtl > 0x7f)
			{
				nTtl = 0x7f;
			}
			CExternalChipEvent::WriteRegisterEvent(0, 0x60 + nOffset, static_cast<UINT8>(nTtl));
		}
		nOffset += 8;
		cMask >>= 1;
	} while (cMask != 0);
}
