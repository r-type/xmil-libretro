/**
 * @file	juliet.h
 * @brief	ROMEO �A�N�Z�X �N���X�̐錾����уC���^�[�t�F�C�X�̒�`�����܂�
 */

#pragma once

#include "..\externalchip.h"
#include "misc\guard.h"
#include "misc\threadbase.h"

/**
 * @brief ROMEO �A�N�Z�X �N���X
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
	//! @brief ���[�h�֐�
	struct ProcItem
	{
		LPCSTR lpSymbol;		//!< �֐���
		size_t nOffset;			//!< �I�t�Z�b�g
	};

	// ��`
	typedef ULONG (WINAPI * FnRead32)(ULONG ulPciAddress, ULONG ulRegAddress);	//!< �R���t�B�O���[�V�����ǂݎ��֐���`
	typedef VOID (WINAPI * FnOut8)(ULONG ulAddress, UCHAR ucParam);				//!< outp �֐���`
	typedef VOID (WINAPI * FnOut32)(ULONG ulAddress, ULONG ulParam);			//!< outpd �֐���`
	typedef UCHAR (WINAPI * FnIn8)(ULONG ulAddress);							//!< inp �֐���`
	typedef ULONG (WINAPI * FnIn32)(ULONG ulAddress);							//!< inpd �֐���`

	HMODULE m_hModule;			//!< ���W���[��
	FnRead32 m_fnRead32;		//!< �R���t�B�O���[�V�����ǂݎ��֐�
	FnOut8 m_fnOut8;			//!< outp �֐�
	FnOut32 m_fnOut32;			//!< outpd �֐�
	FnIn8 m_fnIn8;				//!< inp �֐�
	FnIn32 m_fnIn32;			//!< inpd �֐�
	ULONG m_ulAddress;			//!< ROMEO �x�[�X �A�h���X
	UCHAR m_ucIrq;				//!< ROMEO IRQ
	bool m_bHasYM2151;			//!< 2151 �t���O
	UINT m_nSnoopCount;			//!< �X�v�[�� �J�E���^
	CGuard m_pciGuard;			/*!< The guard of PCI */
	CGuard m_queGuard;			/*!< The guard of que */
	size_t m_nQueIndex;			/*!< The position in que */
	size_t m_nQueCount;			/*!< The count in que */
	UINT32 m_que[0x400];		/*!< que */

	ULONG SearchRomeo() const;

	/**
	 * �`�b�v ID
	 */
	enum ChipId
	{
		kYMF288		= 0,		/*!< YMF288 */
		kYM2151					/*!< YM2151 */
	};
	void ResetChip(ChipId nChipId);
	void Write(ChipId nChipId, UINT nAddr, UINT8 cData);

	/**
	 * @brief �`�b�v �N���X
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
			CJuliet* m_pJuliet;			//!< �e�C���X�^���X
			ChipId m_nChipId;			//!< �`�b�v ID
	};
	IExternalChip* m_pChip288;		//!< YMF288 �C���X�^���X
	IExternalChip* m_pChip2151;		//!< YM2151 �C���X�^���X

	void Detach(IExternalChip* pChip);
	friend class Chip;
};

/**
 * ROMEO �͗L��?
 * @retval true �L��
 * @retval false ����
 */
inline bool CJuliet::IsEnabled() const
{
	return (m_hModule != NULL);
}
