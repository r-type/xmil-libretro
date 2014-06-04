#ifndef _ADVLIBC_STRARG_
#define _ADVLIBC_STRARG_

#include	"./advtypes.h"

#define va_start(ap, last)	(ap) = (va_list)&(last) + sizeof(last)
#define va_arg(ap, t)		((t *)(ap += sizeof(t)))[-1]
#define va_end(ap)			(ap) = NULL

#endif
