#ifndef _ADVLIBC_CTYPE_
#define _ADVLIBC_CTYPE_

#include	"./advtypes.h"

#ifdef __cplusplus
extern "C" {
#endif

extern const unsigned char _ctype[257];

enum {
	_UPPER		= 0x01,
	_LOWER		= 0x02,
	_DIGIT		= 0x04,
	_SPACE		= 0x08,
	_PUNCT		= 0x10,
	_CONTROL	= 0x20,
	_BLANK		= 0x40,
	_HEX		= 0x80
};

#define	_tolower(c)	((c)-'A'+'a')
#define	_toupper(c)	((c)-'a'+'A')

#define	isalpha(c)	((_ctype+1)[(c)] & (_UPPER|_LOWER))
#define	isupper(c)	((_ctype+1)[(c)] & _UPPER)
#define	islower(c)	((_ctype+1)[(c)] & _LOWER)
#define	isdigit(c)	((_ctype+1)[(c)] & _DIGIT)
#define	isxdigit(c)	((_ctype+1)[(c)] & _HEX)
#define	isspace(c)	((_ctype+1)[(c)] & _SPACE)
#define	ispunct(c)	((_ctype+1)[(c)] & _PUNCT)
#define	isalnum(c)	((_ctype+1)[(c)] & (_UPPER|_LOWER|_DIGIT))
#define	isprint(c)	((_ctype+1)[(c)] & (_BLANK|_PUNCT|_UPPER|_LOWER|_DIGIT))
#define	isgraph(c)	((_ctype+1)[(c)] & (_PUNCT|_UPPER|_LOWER|_DIGIT))
#define	iscntrl(c)	((_ctype+1)[(c)] & _CONTROL)
#define	toupper(c)	((islower(c))?_toupper(c):(c))
#define	tolower(c)	((isupper(c))?_tolower(c):(c))
#define	isascii(c)	((unsigned)(c) < 0x80)
#define	toascii(c)	((c) & 0x7f)

#ifdef __cplusplus
}
#endif

#endif
