#ifndef _ADVLIBC_LIMITS_
#define _ADVLIBC_LIMITS_

#define	CHAR_BIT	8
#define	SCHAR_MIN	(-127)
#define	SCHAR_MAX	(127)
#define	UCHAR_MAX	(0xff)
#ifndef	CHAR_UNSIGNED
#define	CHAR_MIN	SCHAR_MIN
#define	CHAR_MAX	SCHAR_MAX
#else
#define	CHAR_MIN	0
#define	CHAR_MAX	UCHAR_MAX
#endif

#define	SHRT_MIN	(-32767)
#define	SHRT_MAX	(32767)
#define	USHRT_MAX	(0xffff)

#define	INT_MIN		(-2147483647)
#define	INT_MAX		(2147483647)
#define	UINT_MAX	(0xffffffff)

#define	LONG_MIN	(-2147483647)
#define	LONG_MAX	(2147483647)
#define	ULONG_MAX	(0xffffffff)

#endif
