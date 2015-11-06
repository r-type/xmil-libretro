/**
 * @file	tickcounter.cpp
 * @brief	TICK カウンタの動作の定義を行います
 */

#include "compiler.h"
#include "tickcounter.h"

// ---- C インタフェイス

//! 唯一のインスタンスです
static TickCounter<1> s_counter;

/**
 * カウンタを得る
 * @return TICK
 */
DWORD GetTickCounter()
{
	return s_counter.Get();
}
