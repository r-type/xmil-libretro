/**
 * @file	externalchipevent.cpp
 * @brief	�O�����t�C�x���g���N���X�̓���̒�`���s���܂�
 */

#include "compiler.h"
#include "externalchipevent.h"
#include "externalchip.h"
#include "externalchipscheduler.h"

/**
 * �R���X�g���N�^
 * @param[in] pChip �`�b�v
 */
CExternalChipEvent::CExternalChipEvent(IExternalChip* pChip)
	: m_pChip(pChip)
{
	CExternalChipScheduler::GetInstance()->AddRef();
}

/**
 * �f�X�g���N�^
 */
CExternalChipEvent::~CExternalChipEvent()
{
	CExternalChipScheduler::GetInstance()->Clear(m_pChip);
	CExternalChipScheduler::GetInstance()->Release();
	delete m_pChip;
}

/**
 * �`�b�v �^�C�v�𓾂�
 * @return �`�b�v �^�C�v
 */
IExternalChip::ChipType CExternalChipEvent::GetChipType()
{
	return m_pChip->GetChipType();
}

/**
 * �������Z�b�g
 */
void CExternalChipEvent::Reset()
{
	CExternalChipScheduler::GetInstance()->Reset(m_pChip);
}

/**
 * ���W�X�^��������
 * @param[in] nAddr �A�h���X
 * @param[in] cData �f�[�^
 */
void CExternalChipEvent::WriteRegister(UINT nAddr, UINT8 cData)
{
	WriteRegisterEvent(0, nAddr, cData);
}

/**
 * ���W�X�^��������
 * @param[in] timestamp �^�C���X�^���v
 * @param[in] nAddr �A�h���X
 * @param[in] cData �f�[�^
 */
void CExternalChipEvent::WriteRegisterEvent(ExternalChipTimestamp timestamp, UINT nAddr, UINT8 cData)
{
	CExternalChipScheduler::GetInstance()->WriteRegister(m_pChip, timestamp, nAddr, cData);
}
