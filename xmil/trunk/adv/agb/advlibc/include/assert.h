#ifndef _ADVLIBC_ASSERT_
#define _ADVLIBC_ASSERT_

#include	"./advtypes.h"

#ifdef __cplusplus
extern "C" {
#endif

void LIBCCALL _assert(const void *flag, const void *file, unsigned int line);

#if !defined(DEBUG)
#define assert(exp)	((void)0)
#else
#define assert(exp)	\
				((exp)?(void)0:_assert(#exp, __FILE__, __LINE__))
#endif

#ifdef __cplusplus
}
#endif

#endif
