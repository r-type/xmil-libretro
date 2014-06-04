#ifndef _ADVLIBC_MALLOC_
#define _ADVLIBC_MALLOC_

#include	"./advtypes.h"
#include	"./memmng.h"

#ifdef __cplusplus
extern "C" {
#endif

void LIBCCALL *_calloc_dbg(size_t num, size_t size, const void *file, unsigned int line);
void LIBCCALL *_malloc_dbg(size_t size, const void *file, unsigned int line);
void LIBCCALL _free_dbg(void *ptr);


#if !defined(DEBUG)

void LIBCCALL *calloc(size_t num, size_t size);
#define	malloc(s)		memmng_alloc((s))
#define	free(p)			memmng_free((p))

#else

#define	calloc(n, s)	_calloc_dbg((n), (s), __FILE__, __LINE__)
#define	malloc(s)		_malloc_dbg((s), __FILE__, __LINE__)
#define	free(p)			_free_dbg((p))

#endif

#ifdef __cplusplus
}
#endif

#endif
