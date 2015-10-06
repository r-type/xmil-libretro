#include "compiler.h"
#include <stdarg.h>
#include "libnds.h"

#ifdef TRACE
void trace_fmt(const char *lpcszFormat, ...)
{
	va_list	ap;
	va_start(ap, lpcszFormat);
	char szWork[256];
	vsprintf(szWork, lpcszFormat, ap);
	va_end(ap);
	iprintf("%s\n", szWork);
}
#endif

