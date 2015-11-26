
#include	"advlibc/include/advtask.h"
#include	"advlibc/include/fileio.h"
#include	"advlibc/include/stdio.h"
#include	"advlibc/include/stdlib.h"
#include	"advlibc/include/stdarg.h"
#include	"advlibc/include/stddef.h"
#include	"advlibc/include/stdarg.h"
#include	"advlibc/include/memory.h"
#include	"advlibc/include/string.h"
#include	"advlibc/include/malloc.h"

#define	BYTESEX_LITTLE
#define	OSLANG_ANK
#define	OSLINEBREAK_CRLF

#ifndef __GNUC__
typedef signed int			SINT;
typedef signed char			SINT8;
typedef unsigned char		UINT8;
typedef	signed short		SINT16;
typedef	unsigned short		UINT16;
typedef	signed int			SINT32;
typedef	unsigned int		UINT32;
#define	INLINE				__inline
#else
typedef signed int			SINT;
typedef unsigned int		UINT;
typedef signed char			SINT8;
typedef unsigned char		UINT8;
typedef	short				SINT16;
typedef	unsigned short		UINT16;
typedef	signed int			SINT32;
typedef	unsigned int		UINT32;
#define	INLINE				inline
#endif


#define BOOL		char
#define	FALSE		0
#define	TRUE		1


// for ARM optimize
#define	REG8		UINT
#define REG16		UINT
#define	LOW12(a)	(((UINT32)((a) << 20)) >> 20)
#define	LOW14(a)	(((UINT32)((a) << 18)) >> 18)
#define	LOW15(a)	(((UINT32)((a) << 17)) >> 17)
#define	LOW16(a)	((UINT16)(a))
#define	HIGH16(a)	(((UINT32)(a)) >> 16)

#define	BRESULT				UINT
#define	OEMCHAR				char
#define	OEMTEXT(string)		(string)
#define	OEMSPRINTF			sprintf
#define	OEMSTRLEN			strlen

#include	"common.h"
#include	"advsys.h"
#include	"romimg.h"
#include	"milstr.h"
#include	"_memory.h"
#include	"trace.h"
#include	"rect.h"


#ifndef	max
#define	max(a,b)	(((a) > (b)) ? (a) : (b))
#endif
#ifndef	min
#define	min(a,b)	(((a) < (b)) ? (a) : (b))
#endif

#ifndef	ZeroMemory
#define	ZeroMemory(d,n)		memset((d), 0, (n))
#endif
#ifndef	CopyMemory
#define	CopyMemory(d,s,n)	memcpy((d),(s),(n))
#endif
#ifndef	FillMemory
#define	FillMemory(a, b, c)	memset((a),(c),(b))
#endif

#ifndef	roundup
#define	roundup(x, y)	((((x)+((y)-1))/(y))*(y))
#endif


#define	GETTICK()			advsys_gettick()
#define	__ASSERT(s)
#define	SPRINTF				sprintf
#define	STRLEN				strlen


#define	SUPPORT_ANK
#define	DISABLE_SOUND
#define	CONST_DISKIMAGE
#define	FIX_Z80A

#define	MEMOPTIMIZE			100

