#include "compiler.h"
#include "libnds.h"


static int checkMemory(LPCVOID lpcvDst, LPCVOID lpcvSrc, int nSize)
{
	const UINT_PTR uDst = reinterpret_cast<UINT_PTR>(lpcvDst);
	const UINT_PTR uSrc = reinterpret_cast<UINT_PTR>(lpcvSrc);
	int nRet = 0;
	if (uDst <= uSrc)
	{
		nRet = (uDst + nSize) - uSrc;
		nRet = max(nRet, 0);
	}
	return nRet;
}

void dmaCopy(LPCVOID lpcvSrc, LPVOID lpvDst, int nSize)
{
	int nLength = checkMemory(g_vram, lpvDst, sizeof(g_vram));
	if (nLength)
	{
		nSize = min(nLength, nSize);
		CopyMemory(lpvDst, lpcvSrc, nLength);
		CNdsRegUpdate::getInstance().set(RT_VRAM);
		return;
	}

	nLength = checkMemory(g_pal, lpvDst, sizeof(g_pal));
	if (nLength)
	{
		nSize = min(nLength, nSize);
		CopyMemory(lpvDst, lpcvSrc, nLength);
		CNdsRegUpdate::getInstance().set(RT_PALETTE);
		return;
	}

}

