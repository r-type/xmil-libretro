/**
 * @file	joymng.h
 * @brief	�W���C�p�b�h�̐錾����уC���^�[�t�F�C�X�̒�`�����܂�
 */

#pragma once

#ifdef __cplusplus

void joymng_initialize();
bool joymng_isEnabled();
void joymng_sync();

extern "C"
{
#endif

REG8 joymng_getstat(void);

#ifdef __cplusplus
}
#endif
