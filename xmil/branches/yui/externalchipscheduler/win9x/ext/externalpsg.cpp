/**
 * @file	externalpsg.cpp
 * @brief	外部 PSG 演奏クラスの動作の定義を行います
 */

#include "compiler.h"
#include "externalpsg.h"

/**
 * コンストラクタ
 * @param[in] pChip チップ
 */
CExternalPsg::CExternalPsg(IExternalChip* pChip)
	: CExternalChipEvent(pChip)
	, m_cPsgMix(0x3f)
{
}

/**
 * デストラクタ
 */
CExternalPsg::~CExternalPsg()
{
}

/**
 * 音源リセット
 */
void CExternalPsg::Reset()
{
	m_cPsgMix = 0x3f;
	CExternalChipEvent::Reset();
}

/**
 * レジスタ書き込み
 * @param[in] timestamp タイムスタンプ
 * @param[in] nAddr アドレス
 * @param[in] cData データ
 */
void CExternalPsg::WriteRegisterEvent(ExternalChipTimestamp timestamp, UINT nAddr, UINT8 cData)
{
	if (nAddr < 0x0e)
	{
		if (nAddr == 0x07)
		{
			// psg mix
			m_cPsgMix = cData;
		}
		CExternalChipEvent::WriteRegisterEvent(timestamp, nAddr, cData);
	}
}

/**
 * メッセージ
 * @param[in] nMessage メッセージ
 * @param[in] nParameter パラメータ
 * @return 結果
 */
INTPTR CExternalPsg::Message(UINT nMessage, INTPTR nParameter)
{
	switch (nMessage)
	{
		case kMute:
			Mute(nParameter != 0);
			break;
	}
	return 0;
}

/**
 * ミュート
 * @param[in] bMute ミュート
 */
void CExternalPsg::Mute(bool bMute)
{
	CExternalChipEvent::WriteRegisterEvent(0, 0x07, (bMute) ? 0x3f : m_cPsgMix);
}
