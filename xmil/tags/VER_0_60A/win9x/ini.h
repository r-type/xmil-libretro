/**
 *	@file	ini.h
 *	@brief	設定ファイル アクセスの宣言およびインターフェイスの定義をします
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
