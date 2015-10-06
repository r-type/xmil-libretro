#ifndef _ADVLIBC_CONIO_
#define _ADVLIBC_CONIO_

#include	"advtypes.h"

#ifdef __cplusplus
extern "C" {
#endif

unsigned char LIBCCALL inpb(unsigned short port);
unsigned short LIBCCALL inpw(unsigned short port);
unsigned int LIBCCALL inpd(unsigned short port);
void LIBCCALL outpb(unsigned int port, unsigned char value);
void LIBCCALL outpw(unsigned int port, unsigned short value);
void LIBCCALL outpd(unsigned int port, unsigned int value);

#ifdef __cplusplus
}
#endif

#endif
