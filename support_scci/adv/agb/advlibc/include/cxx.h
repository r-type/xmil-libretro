#ifndef _ADVLIBC_CXX_
#define _ADVLIBC_CXX_

#include	"./advtypes.h"
#include	"./memmng.h"

#ifdef __cplusplus

inline void *operator new(unsigned int size) {

	return(::memmng_alloc(size));
}

inline void operator delete(void *ptr) {

	::memmng_free(ptr);
}

inline void *operator new[](unsigned int size) {

	return(::memmng_alloc(size));
}

inline void operator delete[](void *ptr) {

	::memmng_free(ptr);
}

extern "C" {
void __cxa_pure_virtual(void);
void _pure_virtual(void);
void __pure_virtual(void);
}

#endif

#endif
