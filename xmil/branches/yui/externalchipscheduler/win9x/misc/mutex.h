/**
 * @file	mutex.h
 * @brief	�~���[�e�N�X �N���X�̐錾����уC���^�[�t�F�C�X�̒�`�����܂�
 */

#pragma once

#include "guard.h"

/**
 * @brief �~���[�e�N�X �N���X
 */
class CMutex
{
public:
	CMutex(CGuard& guard);
	~CMutex();

private:
	CGuard& m_guard;		//!< �N���e�B�J�� �Z�N�V���� �C���X�^���X
};

/**
 * �R���X�g���N�^
 * @param[in] guard �N���e�B�J�� �Z�N�V���� �C���X�^���X
 */
inline CMutex::CMutex(CGuard& guard)
	: m_guard(guard)
{
	guard.Enter();
}

/**
 * �f�X�g���N�^
 */
inline CMutex::~CMutex()
{
	m_guard.Leave();
}
