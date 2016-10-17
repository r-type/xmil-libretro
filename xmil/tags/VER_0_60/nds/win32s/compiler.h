
#define	_WIN32_IE	0x0200

#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <stddef.h>
#include <stdarg.h>
#include "ndscore.h"

#define	BYTESEX_LITTLE
#define	OSLANG_SJIS
#define	OSLINEBREAK_CRLF
#define	NDS_SIMULATE

#ifndef __GNUC__
typedef signed int			SINT;
typedef signed char			SINT8;
typedef unsigned char		UINT8;
typedef	signed short		SINT16;
typedef	unsigned short		UINT16;
typedef	signed int			SINT32;
typedef	unsigned int		UINT32;
typedef signed __int64		SINT64;
typedef unsigned __int64	UINT64;
#define	INLINE				__inline
#define	QWORD_CONST(v)		((UINT64)(v))
#define	SQWORD_CONST(v)		((SINT64)(v))
#define	snprintf			_snprintf
#define	vsnprintf			_vsnprintf
#else
#include	<stdlib.h>
typedef signed char			SINT8;
typedef unsigned char		UINT8;
typedef	short				SINT16;
typedef	unsigned short		UINT16;
typedef	int					SINT32;
typedef signed __int64		SINT64;
#define	INLINE				inline
#endif
#define	FASTCALL			__fastcall

#define	LOADINTELDWORD(a)		(*((UINT32 *)(a)))
#define	LOADINTELWORD(a)		(*((UINT16 *)(a)))
#define	STOREINTELDWORD(a, b)	*(UINT32 *)(a) = (b)
#define	STOREINTELWORD(a, b)	*(UINT16 *)(a) = (b)

#define	STRCALL		__stdcall

#define	BRESULT				UINT8
#define	OEMCHAR				TCHAR
#define	OEMTEXT(string)		_T(string)
#define	OEMSPRINTF			wsprintf
#define	OEMSTRLEN			lstrlen

#include	"common.h"
#include	"milstr.h"
#include	"_memory.h"
#include	"trace.h"
#include	"rect.h"
#include	"extrom.h"


#define	GETTICK()			GetTickCount()
#define	__ASSERT(s)
#define	SPRINTF				wsprintf
#define	STRLEN				lstrlen


#define	SUPPORT_SJIS
#define	DISABLE_SOUND
#define DISABLE_MAKEFONT
#define DISABLE_MOUSE
#define	CONST_DISKIMAGE
#define	FIX_Z80A

#define SUPPORT_8BPP
#define SUPPORT_16BPP
#define SUPPORT_24BPP
#define SUPPORT_32BPP

#define LONG_CALL

