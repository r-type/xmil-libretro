#ifndef _ADVLIBC_IO_
#define _ADVLIBC_IO_

#include	"./advtypes.h"

enum {
	_O_RDONLY		= 0x01,
	_O_WRONLY		= 0x02,
	_O_RDWR			= 0x04,
	_O_CREAT		= 0x08,
	_O_TRUNC		= 0x10,
	_O_APPEND		= 0x20,
	_O_BINARY		= 0x40,
	_O_TEXT			= 0x80
};

#ifdef __cplusplus
extern "C" {
#endif

int LIBCCALL _open(const char *filename, int mode, ...);
int LIBCCALL _close(int fileno);
int LIBCCALL _read(int fileno, void *buf, unsigned int count);
int LIBCCALL _write(int fileno, const void *buf, unsigned int count);
long LIBCCALL _lseek(int fileno, long offset, int origin);
long LIBCCALL _tell(int fileno);

#ifdef __cplusplus
}
#endif

#endif
