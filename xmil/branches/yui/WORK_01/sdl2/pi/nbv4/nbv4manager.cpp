/**
 * @file	nbv4manager.cpp
 * @brief	NBV4 管理クラスの動作の定義を行います
 */

#include "compiler.h"
#include "nbv4manager.h"
#include <algorithm>
#include "nbv4chip.h"
#include <wiringPi.h>
#include <wiringPiSPI.h>

#define GPIO0	0		/*!< GPIO */
#define A0		(1 << 4)
#define WR		(1 << 5)
#define ICL		(1 << 6)
#define ACTLOW	(NBV4_PSG | NBV4_FM1 | NBV4_FM2 | WR | ICL)		/*! アクティブ ロー */

/*! 唯一のインスタンスです */
CNbv4Manager CNbv4Manager::sm_instance;

/**
 * コンストラクタ
 */
CNbv4Manager::CNbv4Manager()
	: m_bInitialized(false)
	, m_bReseted(false)
{
}

/**
 * 初期化
 */
void CNbv4Manager::Initialize()
{
	::wiringPiSetup();
	::wiringPiSPISetup(0, 8000000);
	::pinMode(GPIO0, OUTPUT);
	::digitalWrite(GPIO0, 0);
	m_bInitialized = true;

	ResetChips();
}

/**
 * 解放
 */
void CNbv4Manager::Deinitialize()
{
	std::vector<CNbv4Chip*>::iterator it = m_chips.begin();
	while (it != m_chips.end())
	{
		CNbv4Chip* pChip = *it;
		it = m_chips.erase(it);

		pChip->Reset();
		delete pChip;
	}
}

/**
 * チップ確保
 * @param[in] nChipType チップ タイプ
 * @param[in] nClock チップ クロック
 * @return インスタンス
 */
IExternalChip* CNbv4Manager::GetInterface(IExternalChip::ChipType nChipType, UINT nClock)
{
	CNbv4Chip* pChip = NULL;

	if (nChipType == IExternalChip::kYM2151)
	{
		if (pChip == NULL)
		{
			pChip = GetInterfaceInner(NBV4_FM1);
		}
		if (pChip == NULL)
		{
			pChip = GetInterfaceInner(NBV4_FM2);
		}
	}
	else if (nChipType == IExternalChip::kAY8910)
	{
		if (pChip == NULL)
		{
			pChip = GetInterfaceInner(NBV4_PSG);
		}
	}

	if (pChip)
	{
		m_chips.push_back(pChip);
	}
	return pChip;
}

/**
 * チップ確保
 * @param[in] nType チップ タイプ
 * @return インスタンス
 */
CNbv4Chip* CNbv4Manager::GetInterfaceInner(UINT nType)
{
	for (std::vector<CNbv4Chip*>::iterator it = m_chips.begin(); it != m_chips.end(); ++it)
	{
		if ((*it)->GetType() == nType)
		{
			return NULL;
		}
	}
	return new CNbv4Chip(this, nType);
}

/**
 * チップ解放
 * @param[in] pChip チップ
 */
void CNbv4Manager::Release(IExternalChip* pChip)
{
	Detach(pChip);
	if (pChip)
	{
		pChip->Reset();
		delete pChip;
	}
}

/**
 * チップ解放
 * @param[in] pChip チップ
 */
void CNbv4Manager::Detach(IExternalChip* pChip)
{
	std::vector<CNbv4Chip*>::iterator it = std::find(m_chips.begin(), m_chips.end(), pChip);
	if (it != m_chips.end())
	{
		m_chips.erase(it);
	}
}

/**
 * 音源リセット
 */
void CNbv4Manager::Reset()
{
	for (std::vector<CNbv4Chip*>::iterator it = m_chips.begin(); it != m_chips.end(); ++it)
	{
		(*it)->Reset();
	}
}

/**
 * ミュート
 * @param[in] bMute ミュート
 */
void CNbv4Manager::Mute(bool bMute)
{
	for (std::vector<CNbv4Chip*>::iterator it = m_chips.begin(); it != m_chips.end(); ++it)
	{
		(*it)->Message(IExternalChip::kMute, static_cast<intptr_t>(bMute));
	}
}

/**
 * チップ リセット
 */
void CNbv4Manager::ResetChips()
{
	if ((m_bInitialized) && (!m_bReseted))
	{
		m_bReseted = true;

		Write(0xff & (~(A0 | ICL)));
		delayMicroseconds(100);
		Write(0xff & (~(A0)));
	}
}

/**
 * レジスタ ライト
 * @param[in] nType タイプ
 * @param[in] nAddr アドレス
 * @param[in] cData データ
 */
void CNbv4Manager::SetRegister(UINT nType, UINT nAddr, UINT8 cData)
{
	if (m_bInitialized)
	{
		WriteData((nAddr << 8), nType);
		WriteData((static_cast<UINT>(cData) << 8) | A0, nType);
		m_bReseted = false;
	}
}

/**
 * データ ライト
 * @param[in] nData データ
 * @param[in] nType タイプ
 */
void CNbv4Manager::WriteData(UINT nData, UINT nType)
{
	Write(nData | ACTLOW);
	Write(nData | (ACTLOW & (~(nType | WR))));
	Write(nData | ACTLOW);
}

/**
 * データ ライト
 * @param[in] nData データ
 */
void CNbv4Manager::Write(UINT nData)
{
	::digitalWrite(GPIO0, 0);

	/* ライト */
	unsigned char buf[2];
	buf[0] = static_cast<unsigned char>(nData >> 0);
	buf[1] = static_cast<unsigned char>(nData >> 8);
	::wiringPiSPIDataRW(0, &buf[0], 2);

	/* ラッチ */
	::digitalWrite(GPIO0, 1);
}
