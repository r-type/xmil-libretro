#include "compiler.h"
#include "libnds.h"


	UINT16 g_wRegKeyXY = 0;

touchPosition touchReadXY()
{
	touchPosition pos;
	ZeroMemory(&pos, sizeof(pos));
	return pos;
}

