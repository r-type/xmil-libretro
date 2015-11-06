/**
 * @file	externalchiptimestamp.h
 * @brief	�O�����t�^�C���X�^���v �N���X�̐錾����уC���^�[�t�F�C�X�̒�`�����܂�
 */

#pragma once

//! �^�C���X�^���v
typedef unsigned long ExternalChipTimestamp;

/**
 * @brief �O�����t�^�C���X�^���v �N���X
 */
class CExternalChipTimestamp
{
public:
	static CExternalChipTimestamp* GetInstance();

	CExternalChipTimestamp();
	void Reset();
	void Sync();
	ExternalChipTimestamp Get();

private:
	static CExternalChipTimestamp sm_instance;

	DWORD m_dwLastTick;
	DWORD m_dwLastClock;
};

/**
 * �C���X�^���X�𓾂�
 * @return �C���X�^���X
 */
inline CExternalChipTimestamp* CExternalChipTimestamp::GetInstance()
{
	return &sm_instance;
}
