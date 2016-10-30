/**
 * @file	nbv4chip.h
 * @brief	NBV4 アクセス クラスの宣言およびインターフェイスの定義をします
 */

#pragma once

#include "externalchip.h"

class CNbv4Manager;

/* チップ */
#define NBV4_PSG		(1 << 1)	/*!< PSG */
#define NBV4_FM1		(1 << 2)	/*!< OPM1 */
#define NBV4_FM2		(1 << 3)	/*!< OPM2 */

/**
 * @brief チップ クラス
 */
class CNbv4Chip : public IExternalChip
{
public:
	CNbv4Chip(CNbv4Manager* pManager, UINT nType);
	virtual ~CNbv4Chip();
	virtual ChipType GetChipType();
	virtual void Reset();
	virtual void WriteRegister(UINT nAddr, UINT8 cData);
	virtual intptr_t Message(UINT nMessage, intptr_t nParameter = 0);
	UINT GetType() const;

private:
	CNbv4Manager* m_pManager;	/*!< 親インスタンス */
	UINT m_nType;				/*!< タイプ */
};

/**
 * タイプを得る
 * @return タイプ
 */
inline UINT CNbv4Chip::GetType() const
{
	return m_nType;
}
