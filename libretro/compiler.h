/**
 * @file	compiler.h
 * @brief	include file for standard system include files,
 *			or project specific include files that are used frequently,
 *			but are changed infrequently
 */

#pragma once

#ifdef __OBJC__
#import <UIKit/UIKit.h>
#endif

#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#define SDL_arraysize(array)   (sizeof(array)/sizeof(array[0]))

#ifndef MSB_FIRST
#define	BYTESEX_LITTLE
#else
#define	BYTESEX_BIG
#endif

#define	OSLANG_UTF8
#define	OSLINEBREAK_CRLF
#define RESOURCE_US

typedef	signed int			SINT;
typedef	unsigned int		UINT;
typedef	signed char			SINT8;
typedef	unsigned char		UINT8;
typedef	signed short		SINT16;
typedef	unsigned short		UINT16;
typedef	signed int			SINT32;
typedef	unsigned int		UINT32;
#define INTPTR intptr_t 

#define	BRESULT				UINT
#define	OEMCHAR				char
#define	OEMTEXT(string)		string
#define	OEMSPRINTF			sprintf
#define	OEMSTRLEN			strlen

#if !defined(SIZE_QVGA)
#define	SIZE_VGA
#define	RGB16		UINT16
#else
#define RGB16 UINT32
#endif

typedef	signed char		CHAR;
typedef	unsigned char	BYTE;

#if !defined(OBJC_BOOL_DEFINED)
typedef signed char BOOL;
#endif

#ifndef	TRUE
#define	TRUE	1
#endif

#ifndef	FALSE
#define	FALSE	0
#endif

#ifndef	MAX_PATH
#define	MAX_PATH	256
#endif

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
#define	CopyMemory(d,s,n)	memcpy((d), (s), (n))
#endif
#ifndef	FillMemory
#define	FillMemory(a, b, c)	memset((a), (c), (b))
#endif

#include "common.h"
#include "milstr.h"
#include "_memory.h"
#include "rect.h"
#include "lstarray.h"
#include "trace.h"


long GetTicks();
#define	GETTICK()			GetTicks()
#define	__ASSERT(s)
#define	SPRINTF				sprintf
#define	STRLEN				strlen

#define	SUPPORT_UTF8

#define	SUPPORT_16BPP
#define	MEMOPTIMIZE		2

//#define SOUND_CRITICAL
#define	SOUNDRESERVE	100

#define	SUPPORT_OPM

#define	SCREEN_BPP		16
