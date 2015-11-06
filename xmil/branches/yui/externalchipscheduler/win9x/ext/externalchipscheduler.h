/**
 * @file	externalchipscheduler.h
 * @brief	�O�����t�X�P�W���[�� �N���X�̐錾����уC���^�[�t�F�C�X�̒�`�����܂�
 */

#pragma once

#include <deque>
#include <map>
#include "externalchiptimestamp.h"
#include "misc/guard.h"
#include "misc/tickcounter.h"
#include "misc/threadbase.h"

class IExternalChip;

//! �^�C�~���O����x
#define EXTERNALCHIPSCHEDULER_MULTIPLE		8U

/**
 * @brief �O�����t�X�P�W���[�� �N���X
 */
class CExternalChipScheduler : protected CThreadBase
{
public:
	static CExternalChipScheduler* GetInstance();
	CExternalChipScheduler();
	virtual ~CExternalChipScheduler();
	ULONG AddRef();
	void Release();
	void Clear(IExternalChip* pChip);
	void Reset(IExternalChip* pChip);
	void WriteRegister(IExternalChip* pChip, ExternalChipTimestamp timestamp, UINT nAddr, UINT8 cData);
	ExternalChipTimestamp Now();

protected:
	virtual bool Task();

private:
	static CExternalChipScheduler sm_instance;		//!< �B��̃C���X�^���X�ł�

	/**
	 * @brief ���b�Z�[�W �L���[
	 */
	struct Message
	{
		IExternalChip* pChip;
		ExternalChipTimestamp timestamp;
		DWORD dwData;
	};
	ULONG m_nRef;											//!< ���t�@�����X �J�E���^
	CGuard m_guard;											//!< �K�[�h
	std::deque<Message> m_queue;							//!< ���b�Z�[�W �L���[
	std::map<IExternalChip*, ULONG> m_ref;					//!< ���t�@�����X
	TickCounter<EXTERNALCHIPSCHEDULER_MULTIPLE> m_tick;		//!< TICK
	void ClearInner(IExternalChip* pChip);
};

/**
 * �C���X�^���X�𓾂�
 * @return �C���X�^���X
 */
inline CExternalChipScheduler* CExternalChipScheduler::GetInstance()
{
	return &sm_instance;
}
