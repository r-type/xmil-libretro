/**
 * @file	externalchiptimestamp.h
 * @brief	外部演奏タイムスタンプ クラスの宣言およびインターフェイスの定義をします
 */

#pragma once

//! タイムスタンプ
typedef unsigned long ExternalChipTimestamp;

/**
 * @brief 外部演奏タイムスタンプ クラス
 */
class CExternalChipTimestamp
{
public:
	static CExternalChipTimestamp* GetInstance();

	CExternalChipTimestamp();
	void Reset();
	void Sync();
	ExternalChipTimestamp Get();

private:
	static CExternalChipTimestamp sm_instance;

	DWORD m_dwLastTick;
	DWORD m_dwLastClock;
};

/**
 * インスタンスを得る
 * @return インスタンス
 */
inline CExternalChipTimestamp* CExternalChipTimestamp::GetInstance()
{
	return &sm_instance;
}
