/**
 * @file	nbv4chip.cpp
 * @brief	NBV4 アクセス クラスの動作の定義を行います
 */

#include "compiler.h"
#include "nbv4chip.h"
#include "nbv4manager.h"

/**
 * コンストラクタ
 * @param[in] pManager 親インスタンス
 * @param[in] nType チップ インスタンス
 */
CNbv4Chip::CNbv4Chip(CNbv4Manager* pManager, UINT nType)
	: m_pManager(pManager)
	, m_nType(nType)
{
}

/**
 * デストラクタ
 */
CNbv4Chip::~CNbv4Chip()
{
	m_pManager->Detach(this);
}

/**
 * Get chip type
 * @return The type of the chip
 */
IExternalChip::ChipType CNbv4Chip::GetChipType()
{
	switch (m_nType)
	{
		case NBV4_PSG:
			return IExternalChip::kAY8910;

		case NBV4_FM1:
		case NBV4_FM2:
			return IExternalChip::kYM2151;

		default:
			break;
	}
	return IExternalChip::kNone;
}

/**
 * リセット
 */
void CNbv4Chip::Reset()
{
	m_pManager->ResetChips();
}

/**
 * レジスタ書き込み
 * @param[in] nAddr アドレス
 * @param[in] cData データ
 */
void CNbv4Chip::WriteRegister(UINT nAddr, UINT8 cData)
{
	m_pManager->SetRegister(m_nType, nAddr, cData);
}

/**
 * メッセージ
 * @param[in] nMessage メッセージ
 * @param[in] nParameter パラメータ
 * @return リザルト
 */
intptr_t CNbv4Chip::Message(UINT nMessage, intptr_t nParameter)
{
	return 0;
}
