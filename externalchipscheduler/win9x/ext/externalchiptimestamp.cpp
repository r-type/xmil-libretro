/**
 * @file	externalchiptimestamp.cpp
 * @brief	外部演奏タイムスタンプ クラスの動作の定義を行います
 */

#include "compiler.h"
#include "externalchiptimestamp.h"
#include "externalchipscheduler.h"
#include "z80core.h"

//! 唯一のインスタンスです
CExternalChipTimestamp CExternalChipTimestamp::sm_instance;

/**
 * コンストラクタ
 * @param[in] pChip チップ
 */
CExternalChipTimestamp::CExternalChipTimestamp()
	: m_dwLastTick(0)
	, m_dwLastClock(0)
{
}

/**
 * リセット
 */
void CExternalChipTimestamp::Reset()
{
	m_dwLastTick = CExternalChipScheduler::GetInstance()->Now();
	m_dwLastClock = CPU_CLOCK + CPU_BASECLOCK - CPU_REMCLOCK;
}

/**
 * フレームごとに同期
 */
void CExternalChipTimestamp::Sync()
{
	const DWORD dwZ80Clock = 4000000;

	// 時間
	const DWORD dwNow = CExternalChipScheduler::GetInstance()->Now();
	const DWORD dwPastTick = dwNow - m_dwLastTick;
	if (dwPastTick == 0)
	{
		return;
	}

	// クロック
	const DWORD dwCounter = CPU_CLOCK + CPU_BASECLOCK - CPU_REMCLOCK;
	const DWORD dwStep = dwCounter - m_dwLastClock;

	// 今の時間を求める
	const DWORD dwEstimateTick = dwStep / (dwZ80Clock / (EXTERNALCHIPSCHEDULER_MULTIPLE * 1000U));

	// 100ms 以内でない場合はリセット (処理落ち)
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
 * 今のCPU時間を得る
 * @return TICK
 */
ExternalChipTimestamp CExternalChipTimestamp::Get()
{
	const DWORD dwZ80Clock = 4000000;

	// 時間
	DWORD dwTick = m_dwLastTick;

	// クロック
	const DWORD dwCounter = CPU_CLOCK + CPU_BASECLOCK - CPU_REMCLOCK;
	const DWORD dwStep = dwCounter - m_dwLastClock;
	if (dwStep != 0)
	{
		dwTick += dwStep / (dwZ80Clock / (EXTERNALCHIPSCHEDULER_MULTIPLE * 1000U));
	}
	return dwTick + (EXTERNALCHIPSCHEDULER_MULTIPLE * 50U);
}
