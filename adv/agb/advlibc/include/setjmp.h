#ifndef _ADVLIBC_SETJMP_
#define _ADVLIBC_SETJMP_

#include	"advtypes.h"

#ifndef _JMP_BUF_DEFINED
#define _JMP_BUF_DEFINED
typedef  int  jmp_buf[10];		// r4-11,sp,lr
#endif

#ifdef __cplusplus
extern "C" {
#endif

int LIBCCALL setjmp(jmp_buf env);
void LIBCCALL longjmp(jmp_buf env, int value);

#ifdef __cplusplus
}
#endif

#endif
