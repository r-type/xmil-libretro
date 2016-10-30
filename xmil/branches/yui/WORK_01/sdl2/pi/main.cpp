/*
 *	@file	main.cpp
 */

#include "compiler.h"
#include "nbv4/nbv4manager.h"

extern "C"
{
#include "../xmil.h"
}

/**
 * メイン
 */
int main(int argc, char *argv[])
{
	CNbv4Manager::GetInstance()->Initialize();
	const int r = xmil_main(argc, argv);
	CNbv4Manager::GetInstance()->Deinitialize();
	return r;
}
