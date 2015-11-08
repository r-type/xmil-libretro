/**
 * @file	juliet.cpp
 * @brief	ROMEO �A�N�Z�X �N���X�̓���̒�`���s���܂�
 */

#include "compiler.h"
#include "juliet.h"
#include <stddef.h>
#include "romeo.h"

//! DLL ��
#define	PCIDEBUG_DLL			TEXT("pcidebug.dll")

//! PCI �f�o�C�X �A�h���X���쐬
#define PCIBUSDEVFUNC(b, d, f)	(((b) << 8) | ((d) << 3) | (f))

//! �x���_/�f�o�C�X���쐬
#define	DEVVEND(v, d)			((ULONG)((v) | ((d) << 16)))

/**
 * �R���X�g���N�^
 */
CJuliet::CJuliet()
	: m_hModule(NULL)
	, m_fnRead32(NULL)
	, m_fnOut8(NULL)
	, m_fnOut32(NULL)
	, m_fnIn8(NULL)
	, m_fnIn32(NULL)
	, m_ulAddress(0)
	, m_ucIrq(0)
	, m_bHasYM2151(false)
	, m_nSnoopCount(0)
	, m_nQueIndex(0)
	, m_nQueCount(0)
	, m_pChip288(NULL)
	, m_pChip2151(NULL)
{
}

/**
 * �f�X�g���N�^
 */
CJuliet::~CJuliet()
{
}

/**
 * ������
 * @retval true ����
 * @retval false ���s
 */
bool CJuliet::Initialize()
{
	if (m_hModule)
	{
		return false;
	}

	m_hModule = ::LoadLibrary(PCIDEBUG_DLL);
	if (m_hModule == NULL)
	{
		return false;
	}

	//! ���[�h�֐����X�g
	static const ProcItem s_dllProc[] =
	{
		{"_pciConfigReadLong",	offsetof(CJuliet, m_fnRead32)},
		{"_MemWriteChar",		offsetof(CJuliet, m_fnOut8)},
		{"_MemWriteLong",		offsetof(CJuliet, m_fnOut32)},
		{"_MemReadChar",		offsetof(CJuliet, m_fnIn8)},
		{"_MemReadLong",		offsetof(CJuliet, m_fnIn32)},
	};

	for (size_t i = 0; i < _countof(s_dllProc); i++)
	{
		FARPROC proc = ::GetProcAddress(m_hModule, s_dllProc[i].lpSymbol);
		if (proc == NULL)
		{
			Deinitialize();
			return false;
		}
		*(reinterpret_cast<FARPROC*>(reinterpret_cast<INT_PTR>(this) + s_dllProc[i].nOffset)) = proc;
	}

	const ULONG ulPciAddress = SearchRomeo();
	if (ulPciAddress == static_cast<ULONG>(-1))
	{
		Deinitialize();
		return false;
	}
	m_ulAddress = (*m_fnRead32)(ulPciAddress, ROMEO_BASEADDRESS1);
	m_ucIrq = static_cast<UCHAR>((*m_fnRead32)(ulPciAddress, ROMEO_PCIINTERRUPT));
	if (!m_ulAddress)
	{
		Deinitialize();
		return false;
	}

	Reset();
	Start();

	return true;
}

/**
 * ���
 */
void CJuliet::Deinitialize()
{
	Stop();
	m_nQueIndex = 0;
	m_nQueCount = 0;

	if (m_pChip288)
	{
		delete m_pChip288;
	}
	if (m_pChip2151)
	{
		delete m_pChip2151;
	}

	if (m_hModule)
	{
		::FreeLibrary(m_hModule);
		m_hModule = NULL;
		m_fnRead32 = NULL;
		m_fnOut8 = NULL;
		m_fnOut32 = NULL;
		m_fnIn8 = NULL;
		m_fnIn32 = NULL;
		m_ulAddress = 0;
		m_ucIrq = 0;
	}
}

/**
 * ROMEO ��T��
 * @note pciFindPciDevice�g���ƁAOS�N����ꔭ�ڂɌ������Ȃ����Ƃ������̂ŁA���O�Ō�������i��삳������j
 * @return �o�X �A�h���X
 */
ULONG CJuliet::SearchRomeo() const
{
	for (UINT bus = 0; bus < 0x100; bus++)
	{
		for (UINT dev = 0; dev < 0x20; dev++)
		{
			for (UINT func = 0; func < 0x08; func++)
			{
				const ULONG ulPciAddress = PCIBUSDEVFUNC(bus, dev, func);
				const ULONG ulDeviceVendor = (*m_fnRead32)(ulPciAddress, 0x0000);
				if ((ulDeviceVendor == DEVVEND(ROMEO_VENDORID, ROMEO_DEVICEID)) || (ulDeviceVendor == DEVVEND(ROMEO_VENDORID, ROMEO_DEVICEID2)))
				{
					return ulPciAddress;
				}
			}
		}
	}
	return static_cast<ULONG>(-1);
}

/**
 * �������Z�b�g
 */
void CJuliet::Reset()
{
	m_queGuard.Enter();
	m_nQueIndex = 0;
	m_nQueCount = 0;
	m_queGuard.Leave();

	ResetChip(kYMF288);
	ResetChip(kYM2151);
}

/**
 * �`�b�v ���Z�b�g
 * @param[in] nChipId The id of chips
 */
void CJuliet::ResetChip(ChipId nChipId)
{
	if (!IsEnabled())
	{
		return;
	}

	m_pciGuard.Enter();
	if (nChipId == kYMF288)
	{
		// YMF288 ���Z�b�g
		(*m_fnOut32)(m_ulAddress + ROMEO_YMF288CTRL, 0x00);
		::Sleep(150);

		(*m_fnOut32)(m_ulAddress + ROMEO_YMF288CTRL, 0x80);
		::Sleep(150);
	}
	else
	{
		m_bHasYM2151 = false;

		// YM2151 ���Z�b�g
		(*m_fnOut32)(m_ulAddress + ROMEO_YM2151CTRL, 0x00);
		::Sleep(10);					// 44.1kHz x 192 clk = 4.35ms �ȏ�Ȃ��ƁADAC�̃��Z�b�g������Ȃ�
		UINT8 cFlag = (m_fnIn8)(m_ulAddress + ROMEO_YM2151DATA) + 1;
		(*m_fnOut32)(m_ulAddress + ROMEO_YM2151CTRL, 0x80);
		::Sleep(10);					// ���Z�b�g������A�ꉞ���肷��܂ł�����Ƒ҂�
		cFlag |= (*m_fnIn8)(m_ulAddress + ROMEO_YM2151DATA);
		if (!cFlag)						// cFlag != 0 ���� OPM �`�b�v���Ȃ�
		{
			m_bHasYM2151 = true;

			// Busy���o�p��Snoop�J�E���^���g��
			(*m_fnOut32)(m_ulAddress + ROMEO_SNOOPCTRL, 0x80000000U);
			m_nSnoopCount = 0xffffffff;
		}
	}
	m_pciGuard.Leave();
}

/**
 * �C���^�[�t�F�C�X�擾
 * @param[in] nChipType �^�C�v
 * @param[in] nClock �N���b�N
 * @return �C���X�^���X
 */
IExternalChip* CJuliet::GetInterface(IExternalChip::ChipType nChipType, UINT nClock)
{
	const bool bInitialized = Initialize();

	do
	{
		if (m_hModule == NULL)
		{
			break;
		}

		if ((nChipType == IExternalChip::kYMF288) && (m_pChip288 == NULL))
		{
			m_pChip288 = new Chip(this, kYMF288);
			return m_pChip288;
		}

		if ((nChipType == IExternalChip::kYM2151) && (m_bHasYM2151) && (m_pChip2151 == NULL))
		{
			m_pChip2151 = new Chip(this, kYM2151);
			return m_pChip2151;
		}
	} while (false /*CONSTCOND*/);

	if (bInitialized)
	{
//		Deinitialize();
	}

	return NULL;
}

/**
 * ���
 * @param[in] pChip �`�b�v
 */
void CJuliet::Detach(IExternalChip* pChip)
{
	if (m_pChip288 == pChip)
	{
		m_pChip288 = NULL;
	}
	else if (m_pChip2151 == pChip)
	{
		m_pChip2151 = NULL;
	}
}

/**
 * Write
 * @param[in] nChipId The id of chips
 * @param[in] nAddr The address of registers
 * @param[in] cData The data
 */
void CJuliet::Write(ChipId nChipId, UINT nAddr, UINT8 cData)
{
	m_queGuard.Enter();
	while (m_nQueCount >= _countof(m_que))
	{
		m_queGuard.Leave();
		Delay(1000);
		m_queGuard.Enter();
	}

	m_que[(m_nQueIndex + m_nQueCount) % _countof(m_que)] = (nChipId << 24) | ((nAddr & 0x1ff) << 8) | cData;
	m_nQueCount++;

	m_queGuard.Leave();
}

/**
 * Thread
 * @retval true Cont.
 */
bool CJuliet::Task()
{
	m_queGuard.Enter();
	if (m_nQueCount == 0)
	{
		m_queGuard.Leave();
		Delay(1000);
	}
	else
	{
		while (m_nQueCount)
		{
			const UINT nData = m_que[m_nQueIndex];
			m_nQueIndex = (m_nQueIndex + 1) % _countof(m_que);
			m_nQueCount--;
			m_queGuard.Leave();

			m_pciGuard.Enter();
			if (static_cast<ChipId>(nData >> 24) == kYMF288)
			{
				while (((*m_fnIn8)(m_ulAddress + ROMEO_YMF288ADDR1) & 0x80) != 0)
				{
					::Sleep(0);
				}
				(*m_fnOut8)(m_ulAddress + ((nData & 0x10000) ? ROMEO_YMF288ADDR2 : ROMEO_YMF288ADDR1), static_cast<UINT8>(nData >> 8));

				while (((*m_fnIn8)(m_ulAddress + ROMEO_YMF288ADDR1) & 0x80) != 0)
				{
					::Sleep(0);
				}
				(*m_fnOut8)(m_ulAddress + ((nData & 0x10000) ? ROMEO_YMF288DATA2 : ROMEO_YMF288DATA1), static_cast<UINT8>(nData));
			}
			else
			{
				while (m_nSnoopCount == (*m_fnIn32)(m_ulAddress + ROMEO_SNOOPCTRL))
				{
					::Sleep(0);
				}
				m_nSnoopCount = (*m_fnIn32)(m_ulAddress + ROMEO_SNOOPCTRL);

				// �J�E���^���������_�ł͂܂�Busy�̉\��������̂ŁAOPM��Busy��������
				while ((*m_fnIn8)(m_ulAddress + ROMEO_YM2151DATA) & 0x80)
				{
					::Sleep(0);
				}
				(*m_fnOut8)(m_ulAddress + ROMEO_YM2151ADDR, static_cast<UINT8>(nData >> 8));
				(*m_fnIn8)(m_ulAddress + ROMEO_YM2151DATA);
				(*m_fnOut8)(m_ulAddress + ROMEO_YM2151DATA, static_cast<UINT8>(nData));
			}
			m_pciGuard.Leave();

			m_queGuard.Enter();
		}
		m_queGuard.Leave();
	}
	return true;
}



// ---- �`�b�v

/**
 * �R���X�g���N�^
 * @param[in] pJuliet �e�C���X�^���X
 * @param[in] nChipId �`�b�v ID
 */
CJuliet::Chip::Chip(CJuliet* pJuliet, ChipId nChipId)
	: m_pJuliet(pJuliet)
	, m_nChipId(nChipId)
{
}

/**
 * �f�X�g���N�^
 */
CJuliet::Chip::~Chip()
{
	m_pJuliet->Detach(this);
}

/**
 * Get chip type
 * @return The type of the chip
 */
IExternalChip::ChipType CJuliet::Chip::GetChipType()
{
	return (m_nChipId == kYMF288) ? IExternalChip::kYMF288 : IExternalChip::kYM2151;
}

/**
 * ���Z�b�g
 */
void CJuliet::Chip::Reset()
{
	m_pJuliet->ResetChip(m_nChipId);
}

/**
 * ���W�X�^��������
 * @param[in] nAddr �A�h���X
 * @param[in] cData �f�[�^
 */
void CJuliet::Chip::WriteRegister(UINT nAddr, UINT8 cData)
{
	m_pJuliet->Write(m_nChipId, nAddr, cData);
}

/**
 * ���b�Z�[�W
 * @param[in] nMessage ���b�Z�[�W
 * @param[in] nParameter �p�����[�^
 * @return ���U���g
 */
INTPTR CJuliet::Chip::Message(UINT nMessage, INTPTR nParameter)
{
	return 0;
}
