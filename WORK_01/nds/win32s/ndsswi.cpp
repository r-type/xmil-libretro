#include "compiler.h"
#include "libnds.h"


void swiWaitForVBlank(void)
{
	ndssys_task();
	Sleep(100);
}

