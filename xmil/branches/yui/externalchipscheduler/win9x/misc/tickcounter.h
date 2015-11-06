/**
 * @file	tickcounter.h
 * @brief	TICK �J�E���^�̐錾����уC���^�[�t�F�C�X�̒�`�����܂�
 */

#pragma once

#ifdef __cplusplus

/**
 * @brief TICK �J�E���^�[ �N���X
 */
template<unsigned int nMultiply>
class TickCounter
{
public:
	/**
	 * �R���X�g���N�^
	 */
	TickCounter()
	{
		m_nFreq.QuadPart = 0;
		::QueryPerformanceFrequency(&m_nFreq);
		if (m_nFreq.QuadPart)
		{
			m_dwLastTick = 0;
			::QueryPerformanceCounter(&m_nLast);
		}
		else
		{
			m_dwLastTick = ::GetTickCount();
		}
	}

	/**
	 * TICK �𓾂�
	 * @return TICK
	 */
	DWORD Get()
	{
		if (m_nFreq.QuadPart)
		{
			LARGE_INTEGER nNow;
			::QueryPerformanceCounter(&nNow);
			const ULONGLONG nPast = nNow.QuadPart - m_nLast.QuadPart;

			const DWORD dwTick = static_cast<DWORD>((nPast * (nMultiply * 1000U)) / m_nFreq.QuadPart);
			const DWORD dwRet = (m_dwLastTick * (nMultiply * 1000U)) + dwTick;
			if (dwTick >= (nMultiply * 1000U))
			{
				const DWORD dwSeconds = dwTick / (nMultiply * 1000U);
				m_nLast.QuadPart += m_nFreq.QuadPart * dwSeconds;
				m_dwLastTick += dwSeconds;
			}
			return dwRet;
		}
		else
		{
			return (::GetTickCount() - m_dwLastTick) * nMultiply;
		}
	}

private:
	LARGE_INTEGER m_nFreq;		//!< ���g��
	LARGE_INTEGER m_nLast;		//!< �Ō�̃J�E���^
	DWORD m_dwLastTick;			//!< �Ō�� TICK
};

extern "C"
{
#endif	// __cplusplus

DWORD GetTickCounter();

#ifdef __cplusplus
}
#endif	// __cplusplus
