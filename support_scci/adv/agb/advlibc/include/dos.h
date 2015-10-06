#ifndef _ADVLIBC_DOS_
#define _ADVLIBC_DOS_

#include	"advtypes.h"

#ifdef __cplusplus
extern "C" {
#endif

void LIBCCALL _enable(void);
void LIBCCALL _disable(void);
void LIBCCALL _halt(void);

#ifdef __cplusplus
}
#endif

#endif

