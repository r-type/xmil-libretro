/**
 * @file	nbv4manager.h
 * @brief	NBV4 管理クラスの宣言およびインターフェイスの定義をします
 */

#pragma once

#include <vector>
#include "externalchip.h"

class CNbv4Chip;

/**
 * @brief NBV4 管理クラス
 */
class CNbv4Manager
{
public:
	static CNbv4Manager* GetInstance();

	CNbv4Manager();
	void Initialize();
	void Deinitialize();
	IExternalChip* GetInterface(IExternalChip::ChipType nChipType, UINT nClock);
	void Release(IExternalChip* pChip);
	void Reset();
	void Mute(bool bMute);

protected:
	void Detach(IExternalChip* pChip);
	void ResetChips();
	void SetRegister(UINT nType, UINT nAddr, UINT8 cData);

private:
	static CNbv4Manager sm_instance;	/*!< 唯一のインスタンスです */
	bool m_bInitialized;				/*!< 初期化フラグ */
	bool m_bReseted;					/*!< リセット フラグ */
	std::vector<CNbv4Chip*> m_chips;	/*!< チップ */
	CNbv4Chip* GetInterfaceInner(UINT nType);
	static void WriteData(UINT nData, UINT nType);
	static void Write(UINT nData);

	friend class CNbv4Chip;
};

/**
 * インスタンスを得る
 * @return インスタンス
 */
inline CNbv4Manager* CNbv4Manager::GetInstance()
{
	return &sm_instance;
}
