/**
 * @file	externalchipevent.h
 * @brief	外部演奏イベント基底クラスの宣言およびインターフェイスの定義をします
 */

#pragma once

#include "externalchip.h"
#include "externalchiptimestamp.h"

/**
 * @brief 外部演奏イベント基底クラス
 */
class CExternalChipEvent : public IExternalChip
{
public:
	CExternalChipEvent(IExternalChip* pChip);
	virtual ~CExternalChipEvent();
	virtual ChipType GetChipType();
	virtual void Reset();
	virtual void WriteRegister(UINT nAddr, UINT8 cData);
	virtual void WriteRegisterEvent(ExternalChipTimestamp timestamp, UINT nAddr, UINT8 cData);

private:
	IExternalChip* m_pChip;				//!< チップ
};
