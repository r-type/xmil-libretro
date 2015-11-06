/**
 * @file	externalpsg.h
 * @brief	外部 PSG 演奏クラスの宣言およびインターフェイスの定義をします
 */

#pragma once

#include "externalchipevent.h"

/**
 * @brief 外部 PSG 演奏クラス
 */
class CExternalPsg : public CExternalChipEvent
{
public:
	CExternalPsg(IExternalChip* pChip);
	virtual ~CExternalPsg();
	virtual void Reset();
	virtual void WriteRegisterEvent(ExternalChipTimestamp timestamp, UINT nAddr, UINT8 cData);
	virtual INTPTR Message(UINT nMessage, INTPTR nParameter);

protected:
	UINT8 m_cPsgMix;					//!< PSG ミキサー

	virtual void Mute(bool bMute);
};
