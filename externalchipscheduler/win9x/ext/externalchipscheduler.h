/**
 * @file	externalchipscheduler.h
 * @brief	外部演奏スケジューラ クラスの宣言およびインターフェイスの定義をします
 */

#pragma once

#include <deque>
#include <map>
#include "externalchiptimestamp.h"
#include "misc/guard.h"
#include "misc/tickcounter.h"
#include "misc/threadbase.h"

class IExternalChip;

//! タイミング分解度
#define EXTERNALCHIPSCHEDULER_MULTIPLE		8U

/**
 * @brief 外部演奏スケジューラ クラス
 */
class CExternalChipScheduler : protected CThreadBase
{
public:
	static CExternalChipScheduler* GetInstance();
	CExternalChipScheduler();
	virtual ~CExternalChipScheduler();
	ULONG AddRef();
	void Release();
	void Clear(IExternalChip* pChip);
	void Reset(IExternalChip* pChip);
	void WriteRegister(IExternalChip* pChip, ExternalChipTimestamp timestamp, UINT nAddr, UINT8 cData);
	ExternalChipTimestamp Now();

protected:
	virtual bool Task();

private:
	static CExternalChipScheduler sm_instance;		//!< 唯一のインスタンスです

	/**
	 * @brief メッセージ キュー
	 */
	struct Message
	{
		IExternalChip* pChip;
		ExternalChipTimestamp timestamp;
		DWORD dwData;
	};
	ULONG m_nRef;											//!< リファレンス カウンタ
	CGuard m_guard;											//!< ガード
	std::deque<Message> m_queue;							//!< メッセージ キュー
	std::map<IExternalChip*, ULONG> m_ref;					//!< リファレンス
	TickCounter<EXTERNALCHIPSCHEDULER_MULTIPLE> m_tick;		//!< TICK
	void ClearInner(IExternalChip* pChip);
};

/**
 * インスタンスを得る
 * @return インスタンス
 */
inline CExternalChipScheduler* CExternalChipScheduler::GetInstance()
{
	return &sm_instance;
}
