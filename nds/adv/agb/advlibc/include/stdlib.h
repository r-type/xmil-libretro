#ifndef _ADVLIBC_STDLIB_
#define _ADVLIBC_STDLIB_

#include	"./advtypes.h"

#ifdef __cplusplus
extern "C" {
#endif

#define RAND_MAX 0x7fff

#define max(a, b)	(((a)>(b))?(a):(b))
#define min(a, b)	(((a)<(b))?(a):(b))

void LIBCCALL exit(int status);
int LIBCCALL atexit(void (*func)(void));
void LIBCCALL abort(void);

int LIBCCALL abs(int value);
long LIBCCALL labs(long value);
int LIBCCALL atoi(const char *string);
long LIBCCALL atol(const char *string);
char LIBCCALL *itoa(int value, char *string, int radix);
char LIBCCALL *ltoa(long value, char *string, int radix);
char LIBCCALL *ultoa(unsigned long value, char *string, int radix);
int LIBCCALL rand(void);
void LIBCCALL srand(unsigned int seed);

unsigned int LIBCCALL _rotl(unsigned int value, int shift);
unsigned int LIBCCALL _rotr(unsigned int value, int shift);
unsigned long LIBCCALL _lrotl(unsigned long value, int shift);
unsigned long LIBCCALL _lrotr(unsigned long value, int shift);

unsigned int LIBCCALL _abd(unsigned int value);
unsigned int LIBCCALL _aad(unsigned int value);

#ifdef __cplusplus
}
#endif

#endif
