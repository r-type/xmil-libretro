/**
 * @file	externalchipevent.cpp
 * @brief	外部演奏イベント基底クラスの動作の定義を行います
 */

#include "compiler.h"
#include "externalchipevent.h"
#include "externalchip.h"
#include "externalchipscheduler.h"

/**
 * コンストラクタ
 * @param[in] pChip チップ
 */
CExternalChipEvent::CExternalChipEvent(IExternalChip* pChip)
	: m_pChip(pChip)
{
	CExternalChipScheduler::GetInstance()->AddRef();
}

/**
 * デストラクタ
 */
CExternalChipEvent::~CExternalChipEvent()
{
	CExternalChipScheduler::GetInstance()->Clear(m_pChip);
	CExternalChipScheduler::GetInstance()->Release();
	delete m_pChip;
}

/**
 * チップ タイプを得る
 * @return チップ タイプ
 */
IExternalChip::ChipType CExternalChipEvent::GetChipType()
{
	return m_pChip->GetChipType();
}

/**
 * 音源リセット
 */
void CExternalChipEvent::Reset()
{
	CExternalChipScheduler::GetInstance()->Reset(m_pChip);
}

/**
 * レジスタ書き込み
 * @param[in] nAddr アドレス
 * @param[in] cData データ
 */
void CExternalChipEvent::WriteRegister(UINT nAddr, UINT8 cData)
{
	WriteRegisterEvent(0, nAddr, cData);
}

/**
 * レジスタ書き込み
 * @param[in] timestamp タイムスタンプ
 * @param[in] nAddr アドレス
 * @param[in] cData データ
 */
void CExternalChipEvent::WriteRegisterEvent(ExternalChipTimestamp timestamp, UINT nAddr, UINT8 cData)
{
	CExternalChipScheduler::GetInstance()->WriteRegister(m_pChip, timestamp, nAddr, cData);
}
