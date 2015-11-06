/**
 * @file	externalchiptimestamp.cpp
 * @brief	�O�����t�^�C���X�^���v �N���X�̓���̒�`���s���܂�
 */

#include "compiler.h"
#include "externalchiptimestamp.h"
#include "externalchipscheduler.h"
#include "z80core.h"

//! �B��̃C���X�^���X�ł�
CExternalChipTimestamp CExternalChipTimestamp::sm_instance;

/**
 * �R���X�g���N�^
 * @param[in] pChip �`�b�v
 */
CExternalChipTimestamp::CExternalChipTimestamp()
	: m_dwLastTick(0)
	, m_dwLastClock(0)
{
}

/**
 * ���Z�b�g
 */
void CExternalChipTimestamp::Reset()
{
	m_dwLastTick = CExternalChipScheduler::GetInstance()->Now();
	m_dwLastClock = CPU_CLOCK + CPU_BASECLOCK - CPU_REMCLOCK;
}

/**
 * �t���[�����Ƃɓ���
 */
void CExternalChipTimestamp::Sync()
{
	const DWORD dwZ80Clock = 4000000;

	// ����
	const DWORD dwNow = CExternalChipScheduler::GetInstance()->Now();
	const DWORD dwPastTick = dwNow - m_dwLastTick;
	if (dwPastTick == 0)
	{
		return;
	}

	// �N���b�N
	const DWORD dwCounter = CPU_CLOCK + CPU_BASECLOCK - CPU_REMCLOCK;
	const DWORD dwStep = dwCounter - m_dwLastClock;

	// ���̎��Ԃ����߂�
	const DWORD dwEstimateTick = dwStep / (dwZ80Clock / (EXTERNALCHIPSCHEDULER_MULTIPLE * 1000U));

	// 100ms �ȓ��łȂ��ꍇ�̓��Z�b�g (��������)
	const LONG nDiff = static_cast<LONG>(dwEstimateTick - dwPastTick);
	if (abs(nDiff) >=  (EXTERNALCHIPSCHEDULER_MULTIPLE * 100U))
	{
		m_dwLastTick = dwNow;
		m_dwLastClock = dwCounter;
	}
	else if ((nDiff >= 0) && (dwPastTick))
	{
		m_dwLastTick += dwPastTick;
		m_dwLastClock += dwPastTick * (dwZ80Clock / (EXTERNALCHIPSCHEDULER_MULTIPLE * 1000U));
	}
}

/**
 * ����CPU���Ԃ𓾂�
 * @return TICK
 */
ExternalChipTimestamp CExternalChipTimestamp::Get()
{
	const DWORD dwZ80Clock = 4000000;

	// ����
	DWORD dwTick = m_dwLastTick;

	// �N���b�N
	const DWORD dwCounter = CPU_CLOCK + CPU_BASECLOCK - CPU_REMCLOCK;
	const DWORD dwStep = dwCounter - m_dwLastClock;
	if (dwStep != 0)
	{
		dwTick += dwStep / (dwZ80Clock / (EXTERNALCHIPSCHEDULER_MULTIPLE * 1000U));
	}
	return dwTick + (EXTERNALCHIPSCHEDULER_MULTIPLE * 50U);
}
