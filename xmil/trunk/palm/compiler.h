#include	<PalmOS.h>
#include	<PalmCompatibility.h>
#include	<string.h>
#include	<stdio.h>
#include	<stdlib.h>
#include	<stdarg.h>
#include	<stddef.h>
#if defined(CPUCORE_IA32)
#include	<setjmp.h>
#endif

#define	PALMOS
#define	BYTESEX_LITTLE
#define	OSLANG_SJIS
#define	OSLINEBREAK_LF


typedef signed int		SINT;
typedef unsigned int	UINT;
typedef signed char		SINT8;
typedef unsigned char	UINT8;
typedef signed short	SINT16;
typedef unsigned short  UINT16;
typedef signed int		SINT32;
typedef unsigned int	UINT32;
typedef signed long long	SINT64;
typedef unsigned long long  UINT64;
#define	TRUE	true
#define FALSE	false

// for ARM optimize
#if !defined(SIZE_VGATEST)
#define	RGB16			UINT32
#endif

#define	REG8		UINT
#define REG16		UINT
#define	LOW12(a)	((((UINT)(a)) << 20) >> 20)
#define	LOW14(a)	((((UINT)(a)) << 18) >> 18)
#define	LOW15(a)	((((UINT)(a)) << 17) >> 17)
#define	LOW16(a)	((UINT16)(a))
#define	HIGH16(a)	(((UINT32)(a)) >> 16)


#define	MAX_PATH	256

#define	ZeroMemory(a, b)		ARM_MemSet((a), (b), 0)
#define	FillMemory(a, b, c)		ARM_MemSet((a), (b), (c))
#define	CopyMemory(a, b, c)		ARM_MemMove((a), (b), (c))

#define	max(a, b)				(((a)>(b))?(a):(b))
#define	min(a, b)				(((a)<(b))?(a):(b))


#define	BRESULT				UINT
#define	OEMCHAR				char
#define	OEMTEXT(string)		(string)
#define	OEMSPRINTF(a,b,c)	ARM_StrPrintF(a,b,(UINT32)c)
#define	OEMSTRLEN			ARM_StrLen
#define INLINE				inline

#include	"common.h"
#include	"milstr.h"
#include	"_memory.h"
#include	"rect.h"
#include	"trace.h"
#include	"palmossub.h"
#include	"fdlibm.h"

#define	GETTICK()			palmos_gettick()
#define	__ASSERT(s)
#define	SPRINTF				ARM_StrPrintF
#define	STRLEN				ARM_StrLen
#define malloc				ARM_MemPtrNew
#define free				ARM_MemPtrFree

#define	SUPPORT_SJIS

typedef unsigned char	BYTE;
typedef unsigned short  WORD;
typedef unsigned long   DWORD;

#define	MEMOPTIMIZE			2
#define	SIZE_QVGA
#define	SUPPORT_16BPP
//#define	SUPPORT_X1F
//#define	SUPPORT_WAVEREC
#define	SUPPORT_RESUME
//#define	SUPPORT_STATSAVE	10

#define	SOUNDRESERVE		80

//#define SUPPORT_TURBOZ
#define SUPPORT_OPM
