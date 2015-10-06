#ifndef _ADVLIBC_TASK_
#define _ADVLIBC_TASK_

#include	"./advtypes.h"

typedef struct {
	void			(*irq)(unsigned int ie);
	unsigned int	__tickcounter;
} ADVIRQ;


#ifdef __cplusplus
extern "C" {
#endif

extern	ADVIRQ	__advirq;

unsigned int LIBCCALL advsys_gettick(void);
void LIBCCALL advsys_sleep(unsigned int tick);
void LIBCCALL advsys_waitvdisp(void);
void LIBCCALL advsys_waitvsync(void);
void LIBCCALL advsys_risevsync(void);
void LIBCCALL advsys_rest(unsigned int clock64);

#ifdef __cplusplus
}
#endif

#endif
