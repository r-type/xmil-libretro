/*
 *	@file	main.m
 */

#include "compiler.h"
#include "../xmil/sdl2/dosio.h"
#include "../xmil/sdl2/xmil.h"

int SDL_main(int argc, char *argv[])
{
	file_setcd("/sdcard/application/xmil/");
	return xmil_main(argc, argv);
}
