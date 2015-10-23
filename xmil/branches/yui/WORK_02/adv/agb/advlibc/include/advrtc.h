#ifndef _ADVLIBC_ADVRTC_
#define _ADVLIBC_ADVRTC_

#include	"./advtypes.h"

#ifdef __cplusplus
extern "C" {
#endif

int LIBCCALL advrtc_isexist(void);
void LIBCCALL advrtc_cmd(unsigned int cmd);
unsigned int LIBCCALL advrtc_recv(void);

#ifdef __cplusplus
}
#endif

#endif
