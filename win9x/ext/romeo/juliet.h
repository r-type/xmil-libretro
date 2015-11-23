/**
 * @file	juliet.h
 * @brief	ROMEO アクセス クラスの宣言およびインターフェイスの定義をします
 */

#pragma once

#include "..\externalchip.h"
#include "misc\guard.h"
#include "misc\threadbase.h"

/**
 * @brief ROMEO アクセス クラス
 */
class CJuliet : protected CThreadBase
{
public:
	CJuliet();
	~CJuliet();
	bool Initialize();
	void Deinitialize();
	void Reset();
	IExternalChip* GetInterface(IExternalChip::ChipType nChipType, UINT nClock);
	bool IsEnabled() const;

protected:
	virtual bool Task();

private:
	//! @brief ロード関数
	struct ProcItem
	{
		LPCSTR lpSymbol;		//!< 関数名
		size_t nOffset;			//!< オフセット
	};

	// 定義
	typedef ULONG (WINAPI * FnRead32)(ULONG ulPciAddress, ULONG ulRegAddress);	//!< コンフィグレーション読み取り関数定義
	typedef VOID (WINAPI * FnOut8)(ULONG ulAddress, UCHAR ucParam);				//!< outp 関数定義
	typedef VOID (WINAPI * FnOut32)(ULONG ulAddress, ULONG ulParam);			//!< outpd 関数定義
	typedef UCHAR (WINAPI * FnIn8)(ULONG ulAddress);							//!< inp 関数定義
	typedef ULONG (WINAPI * FnIn32)(ULONG ulAddress);							//!< inpd 関数定義

	HMODULE m_hModule;			//!< モジュール
	FnRead32 m_fnRead32;		//!< コンフィグレーション読み取り関数
	FnOut8 m_fnOut8;			//!< outp 関数
	FnOut32 m_fnOut32;			//!< outpd 関数
	FnIn8 m_fnIn8;				//!< inp 関数
	FnIn32 m_fnIn32;			//!< inpd 関数
	ULONG m_ulAddress;			//!< ROMEO ベース アドレス
	UCHAR m_ucIrq;				//!< ROMEO IRQ
	bool m_bHasYM2151;			//!< 2151 フラグ
	UINT m_nSnoopCount;			//!< スプール カウンタ
	CGuard m_pciGuard;			/*!< The guard of PCI */
	CGuard m_queGuard;			/*!< The guard of que */
	size_t m_nQueIndex;			/*!< The position in que */
	size_t m_nQueCount;			/*!< The count in que */
	UINT32 m_que[0x400];		/*!< que */

	ULONG SearchRomeo() const;

	/**
	 * チップ ID
	 */
	enum ChipId
	{
		kYMF288		= 0,		/*!< YMF288 */
		kYM2151					/*!< YM2151 */
	};
	void ResetChip(ChipId nChipId);
	void Write(ChipId nChipId, UINT nAddr, UINT8 cData);

	/**
	 * @brief チップ クラス
	 */
	class Chip : public IExternalChip
	{
		public:
			Chip(CJuliet* pJuliet, ChipId nChipId);
			virtual ~Chip();
			virtual ChipType GetChipType();
			virtual void Reset();
			virtual void WriteRegister(UINT nAddr, UINT8 cData);
			virtual INTPTR Message(UINT nMessage, INTPTR nParameter = 0);

		private:
			CJuliet* m_pJuliet;			//!< 親インスタンス
			ChipId m_nChipId;			//!< チップ ID
	};
	IExternalChip* m_pChip288;		//!< YMF288 インスタンス
	IExternalChip* m_pChip2151;		//!< YM2151 インスタンス

	void Detach(IExternalChip* pChip);
	friend class Chip;
};

/**
 * ROMEO は有効?
 * @retval true 有効
 * @retval false 無効
 */
inline bool CJuliet::IsEnabled() const
{
	return (m_hModule != NULL);
}
