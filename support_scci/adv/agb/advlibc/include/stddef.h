#ifndef _ADVLIBC_STDDEF_
#define _ADVLIBC_STDDEF_

#include	"./advtypes.h"

#define offsetof(s, m)	(size_t)&(((s *)0)->m)

#endif
