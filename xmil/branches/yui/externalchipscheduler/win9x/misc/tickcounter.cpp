/**
 * @file	tickcounter.cpp
 * @brief	TICK �J�E���^�̓���̒�`���s���܂�
 */

#include "compiler.h"
#include "tickcounter.h"

// ---- C �C���^�t�F�C�X

//! �B��̃C���X�^���X�ł�
static TickCounter<1> s_counter;

/**
 * �J�E���^�𓾂�
 * @return TICK
 */
DWORD GetTickCounter()
{
	return s_counter.Get();
}
