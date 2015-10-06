#include "compiler.h"
#include "libnds.h"


void consoleInitDefault(u16* map, u16* charBase, u8 bitDepth)
{
}

void iprintf(const char *lpcszFormat, ...)
{
	va_list	ap;

	va_start(ap, lpcszFormat);
	vprintf(lpcszFormat, ap);
	va_end(ap);
}

