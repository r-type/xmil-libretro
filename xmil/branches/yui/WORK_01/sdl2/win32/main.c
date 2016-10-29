/**
 * @file	main.c
 * @brief	メイン
 */

#include "compiler.h"
#include "..\xmil.h"
#include "..\fontmng.h"

#if (SDL_MAJOR_VERSION >= 2)
#pragma comment(lib, "SDL2.lib")
#pragma comment(lib, "SDL2main.lib")
#else	/* (SDL_MAJOR_VERSION >= 2) */
#pragma comment(lib, "SDL.lib")
#pragma comment(lib, "SDLmain.lib")
#endif	/* (SDL_MAJOR_VERSION >= 2) */

#if !defined(RESOURCE_US)
#pragma comment(lib, "SDL2_ttf.lib")
#endif

/**
 * メイン
 * @param[in] argc 引数
 * @param[in] argv 引数
 * @return リザルト コード
 */
int main(int argc, char *argv[])
{
	UINT nLength;
	TCHAR szFont[MAX_PATH];

	nLength = GetWindowsDirectory(szFont, SDL_arraysize(szFont));
	lstrcpy(szFont + nLength, TEXT("\\Fonts\\msgothic.ttc"));
	fontmng_setdeffontname(szFont);

	return xmil_main(argc, argv);
}
