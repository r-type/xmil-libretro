#ifndef _ADVLIBC_FILEIO_
#define _ADVLIBC_FILEIO_

#include	"./advtypes.h"

#ifdef __cplusplus
extern "C" {
#endif

void LIBCCALL *FileMapping(const char *filename, unsigned int *size);
#define	FileUnmapping(ptr)

HANDLE LIBCCALL FileOpen(const char *filename);
unsigned int LIBCCALL FileRead(HANDLE handle, void *ptr, unsigned int size);
long LIBCCALL FileSeek(HANDLE handle, long position, unsigned int origin);
unsigned int LIBCCALL FileGetSize(HANDLE handle);
void LIBCCALL FileClose(HANDLE handle);

#ifdef __cplusplus
}
#endif

#endif
