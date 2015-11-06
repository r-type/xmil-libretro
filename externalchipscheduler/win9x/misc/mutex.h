/**
 * @file	mutex.h
 * @brief	ミューテクス クラスの宣言およびインターフェイスの定義をします
 */

#pragma once

#include "guard.h"

/**
 * @brief ミューテクス クラス
 */
class CMutex
{
public:
	CMutex(CGuard& guard);
	~CMutex();

private:
	CGuard& m_guard;		//!< クリティカル セクション インスタンス
};

/**
 * コンストラクタ
 * @param[in] guard クリティカル セクション インスタンス
 */
inline CMutex::CMutex(CGuard& guard)
	: m_guard(guard)
{
	guard.Enter();
}

/**
 * デストラクタ
 */
inline CMutex::~CMutex()
{
	m_guard.Leave();
}
