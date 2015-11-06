/**
 * @file	externalchipscheduler.cpp
 * @brief	外部演奏スケジューラ クラスの動作の定義を行います
 */

#include "compiler.h"
#include "externalchipscheduler.h"
#include "externalchip.h"
#include "misc/mutex.h"

//! 唯一のインスタンスです
CExternalChipScheduler CExternalChipScheduler::sm_instance;

/**
 * コンストラクタ
 */
CExternalChipScheduler::CExternalChipScheduler()
	: m_nRef(0)
{
}

/**
 * デストラクタ
 */
CExternalChipScheduler::~CExternalChipScheduler()
{
}

/**
 * 開始
 * @return リファレンス カウンタ
 */
ULONG CExternalChipScheduler::AddRef()
{
	if (m_nRef == 0)
	{
		Start();
	}
	m_nRef++;
	return m_nRef;
}

/**
 * 終了
 */
void CExternalChipScheduler::Release()
{
	if (m_nRef > 0)
	{
		m_nRef--;
		if (m_nRef == 0)
		{
			Stop();
			m_queue.clear();
		}
	}
}

/**
 * クリア
 * @param[in] pChip チップ
 */
void CExternalChipScheduler::ClearInner(IExternalChip* pChip)
{
	std::deque<Message>::iterator it = m_queue.begin();
	while (it != m_queue.end())
	{
		if (it->pChip == pChip)
		{
			it = m_queue.erase(it);
		}
		else
		{
			++it;
		}
	}
	m_ref.erase(pChip);
}

/**
 * クリア
 * @param[in] pChip チップ
 */
void CExternalChipScheduler::Clear(IExternalChip* pChip)
{
	CMutex m(m_guard);
	ClearInner(pChip);
}

/**
 * リセット
 * @param[in] pChip チップ
 */
void CExternalChipScheduler::Reset(IExternalChip* pChip)
{
	CMutex m(m_guard);
	ClearInner(pChip);
	pChip->Reset();
}

/**
 * ライト
 * @param[in] pChip チップ
 * @param[in] timestamp タイムスタンプ
 * @param[in] nAddr アドレス
 * @param[in] cData データ
 */
void CExternalChipScheduler::WriteRegister(IExternalChip* pChip, ExternalChipTimestamp timestamp, UINT nAddr, UINT8 cData)
{
	CMutex m(m_guard);

	std::map<IExternalChip*, ULONG>::iterator it = m_ref.find(pChip);
	if ((it == m_ref.end()) || (it->second == 0))
	{
		if (timestamp < m_tick.Get())
		{
			pChip->WriteRegister(nAddr, cData);
			return;
		}
	}
	if (it == m_ref.end())
	{
		m_ref.insert(std::pair<IExternalChip*, ULONG>(pChip, 1));
	}
	else
	{
		it->second++;
	}

	Message msg;
	msg.pChip = pChip;
	msg.timestamp = timestamp;
	msg.dwData = (nAddr << 8) | cData;
	m_queue.push_back(msg);
}

/**
 * TICK を得る
 * @return TICK
 */
ExternalChipTimestamp CExternalChipScheduler::Now()
{
	CMutex m(m_guard);
	return m_tick.Get();
}

/**
 * 演奏タスク
 * @retval true 常に true
 */
bool CExternalChipScheduler::Task()
{
	DWORD nSleep = 0;

	m_guard.Enter();
	const ExternalChipTimestamp nNow = m_tick.Get();

	while (true /*CONSTCOND*/)
	{
		if (m_queue.empty())
		{
			nSleep = 10;
			break;
		}
		const Message& msg = m_queue.front();
		if (msg.timestamp > nNow)
		{
			nSleep = (msg.timestamp - nNow - 1) / EXTERNALCHIPSCHEDULER_MULTIPLE;
			nSleep = min(nSleep, 10);
			break;
		}

		IExternalChip* pChip = msg.pChip;
		const DWORD dwData = msg.dwData;
		m_queue.pop_front();

		std::map<IExternalChip*, ULONG>::iterator it = m_ref.find(pChip);
		if (it != m_ref.end())
		{
			it->second--;
		}

		m_guard.Leave();

		pChip->WriteRegister(dwData >> 8, dwData & 0xff);

		m_guard.Enter();
	}

	m_guard.Leave();
	::Sleep(nSleep);
	return true;
}
