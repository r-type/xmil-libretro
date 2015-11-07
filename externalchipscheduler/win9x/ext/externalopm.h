/**
 * @file	externalopm.h
 * @brief	外部 OPM 演奏クラスの宣言およびインターフェイスの定義をします
 */

#pragma once

#include "externalchipevent.h"

/**
 * @brief 外部 OPM 演奏クラス
 */
class CExternalOpm : public CExternalChipEvent
{
public:
	CExternalOpm(IExternalChip* pChip);
	virtual ~CExternalOpm();
	virtual void Reset();
	virtual void WriteRegisterEvent(ExternalChipTimestamp timestamp, UINT nAddr, UINT8 cData);
	virtual INTPTR Message(UINT nMessage, INTPTR nParameter);

protected:
	UINT8 m_cAlgorithm[8];				//!< アルゴリズム テーブル
	UINT8 m_cTtl[8 * 4];				//!< TTL テーブル

	void Mute(bool bMute);
	void SetVolume(UINT nChannel, int nVolume);
};
