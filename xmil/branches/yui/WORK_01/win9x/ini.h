/**
 *	@file	ini.h
 *	@brief	�ݒ�t�@�C�� �A�N�Z�X�̐錾����уC���^�[�t�F�C�X�̒�`�����܂�
 */

#include "profile.h"

#ifdef __cplusplus
extern "C"
{
#endif

void ini_read(LPCTSTR lpPath, LPCTSTR lpTitle, const PFTBL* lpTable, UINT nCount);
void ini_write(LPCTSTR lpPath, LPCTSTR lpTitle, const PFTBL* lpTable, UINT nCount);

void initgetfile(LPTSTR lpPath, UINT cchPath);
void initload(void);
void initsave(void);

#ifdef __cplusplus
}
#endif
