/**
 * @file	joymng.h
 * @brief	ジョイパッドの宣言およびインターフェイスの定義をします
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
