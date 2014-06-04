#ifndef _ADVLIBC_ADVTYPES_
#define _ADVLIBC_ADVTYPES_

#ifndef LIBCCALL
#define	LIBCCALL
#endif

#ifndef __GNUC__
typedef int				bool;
#endif
#ifndef false
#define	false			0
#endif
#ifndef true
#define	true			(!false)
#endif

typedef void			*HANDLE;
#define	INVALID_HANDLE	((HANDLE)0)

#ifndef NULL
#define NULL	0
#endif

#ifndef _SIZE_T_DEFINED
#define	_SIZE_T_DEFINED
typedef	unsigned int	size_t;
#endif

#ifndef _time_t_defined
#define _time_t_defined
typedef	long			time_t;
#endif

#ifndef _CLOCK_T_DEFINED
typedef	long			clock_t;
#define _CLOCK_T_DEFINED
#endif

typedef	char			*va_list;

#endif
