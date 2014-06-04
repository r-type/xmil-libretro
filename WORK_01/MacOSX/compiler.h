#include	<string.h>
#include	<stdio.h>
#include	<stdlib.h>
#include	<stdarg.h>
#include	<stddef.h>
#if defined(CPUCORE_IA32)
#include	<setjmp.h>
#endif

#define	MACOS
#define	BYTESEX_BIG
#define	OSLANG_SJIS
#define	OSLINEBREAK_CR


typedef signed int		SINT;
typedef unsigned int	UINT;
typedef signed char		SINT8;
typedef unsigned char	UINT8;
typedef signed short	SINT16;
typedef unsigned short	UINT16;
typedef signed int		SINT32;
typedef unsigned int	UINT32;
typedef SInt64			SINT64;
typedef UInt64			UINT64;
typedef Boolean			BOOL;


typedef unsigned char	BYTE;
typedef unsigned short	WORD;
typedef unsigned int	DWORD;


#define	MAX_PATH	256

#define	ZeroMemory(a, b)		memset((a),  0 , (b))
#define	FillMemory(a, b, c)		memset((a), (c), (b))
#define	CopyMemory(a, b, c)		memcpy((a), (b), (c))

#define	max(a, b)				(((a)>(b))?(a):(b))
#define	min(a, b)				(((a)<(b))?(a):(b))


#define	BRESULT				Boolean
#define	OEMCHAR				char
#define	OEMTEXT(string)		(string)
#define	OEMSPRINTF			sprintf
#define	OEMSTRLEN			strlen

#include	"common.h"
#include	"macossub.h"
#include	"milstr.h"
#include	"_memory.h"
#include	"trace.h"
#include	"rect.h"


#define	GETTICK()			macos_gettick()
#define	__ASSERT(s)
#define	SPRINTF				sprintf
#define	STRLEN				strlen

#define	SUPPORT_SJIS

#define	MEMOPTIMIZE			1
#define	SUPPORT_32BPP
#define	SUPPORT_OPM
#define	SUPPORT_RESUME
#define	SUPPORT_STATSAVE	10

#define	SOUNDRESERVE		80

