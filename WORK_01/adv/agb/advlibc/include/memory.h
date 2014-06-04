#ifndef _ADVLIBC_MEMORY_
#define _ADVLIBC_MEMORY_

#include	"./advtypes.h"

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(NOBUILTIN)
#define	memset(d, s, c)		__builtin_memset(d, s, c)
#define	memcpy(d, s, c)		__builtin_memcpy(d, s, c)
#define	memcmp(d, s, c)		__builtin_memcmp(d, s, c)
#else
void LIBCCALL *memset(void *buf, int c, size_t count);
void LIBCCALL *memcpy(void *buf1, const void *buf2, size_t count);
int LIBCCALL memcmp(const void *buf1, const void *buf2, size_t count);
#endif

void LIBCCALL *memmove(void *buf1, const void *buf2, size_t count);

void LIBCCALL *memchr(const void *buf, int c, size_t count);
int LIBCCALL memicmp(const void *buf1, const void *buf2, unsigned int count);

void LIBCCALL bzero(void *ptr, unsigned int count);

void LIBCCALL *bzero8(void *ptr, unsigned int count);
void LIBCCALL *bzero16(void *ptr, unsigned int count);
void LIBCCALL *bzero32(void *ptr, unsigned int count);
void LIBCCALL *bset8(void *ptr, unsigned int count, unsigned int fill);
void LIBCCALL *bset16(void *ptr, unsigned int count, unsigned int fill);
void LIBCCALL *bset32(void *ptr, unsigned int count, unsigned int fill);
void LIBCCALL *bcopy8(void *dst, const void *src, unsigned int count);
void LIBCCALL *bcopy16(void *dst, const void *src, unsigned int count);
void LIBCCALL *bcopy32(void *dst, const void *src, unsigned int count);

#ifdef __cplusplus
}
#endif

#endif
