#ifndef _ADVLIBC_MEMMNG_
#define _ADVLIBC_MEMMNG_

#include	"./advtypes.h"

#ifdef __cplusplus
extern "C" {
#endif

void LIBCCALL *memmng_alloc(unsigned int size);
void LIBCCALL memmng_free(void *addr);

#ifdef __cplusplus
}
#endif

#endif
