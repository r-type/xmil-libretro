#include "compiler.h"

void ndscore_timer3init() { }
void ndscore_timer3int() { }
void ndscore_sleep(unsigned int uCount) { Sleep(uCount); }
void ndscore_rest(unsigned int uCount) { Sleep(uCount / 512); }
unsigned int ndscore_gettick() { return GetTickCount(); }

