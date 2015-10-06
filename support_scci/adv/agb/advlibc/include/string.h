#ifndef _ADVLIBC_STRING_
#define _ADVLIBC_STRING_

#include	"./advtypes.h"

#ifdef __cplusplus
extern "C" {
#endif

char LIBCCALL *strcpy(char *string1, const char *string2);
char LIBCCALL *strcat(char *string1, const char *string2);
int LIBCCALL strcmp(const char *string1, const char *string2);
size_t LIBCCALL strlen(const char *string);

#if !defined(NOBUILTIN)
#define	strncpy(d, s, c)	__builtin_strncpy(d, s, c)
#define	strncat(d, s, c)	__builtin_strncat(d, s, c)
#define	strncmp(d, s, c)	__builtin_strncmp(d, s, c)
#else
char LIBCCALL *strncpy(char *string1, const char *string2, size_t count);
char LIBCCALL *strncat(char *string1, const char *string2, size_t count);
int LIBCCALL strncmp(const char *string1, const char *string2, size_t count);
#endif

char LIBCCALL *strchr(const char *string, int c);
char LIBCCALL *strrchr(const char *string, int c);

int LIBCCALL stricmp(const char *string1, const char *string2);
int LIBCCALL strnicmp(const char *string1, const char *string2, size_t count);

char LIBCCALL *strdup(const char *string);
char LIBCCALL *strupr(char *string);
char LIBCCALL *strlwr(char *string);
char LIBCCALL *strrev(char *string);
char LIBCCALL *strset(char *string, int c);
char LIBCCALL *strnset(char *string, int c, size_t count);
char LIBCCALL *strstr(const char *string1, const char *string2);
size_t LIBCCALL strspn(const char *string1, const char *string2);
size_t LIBCCALL strcspn(const char *string1, const char *string2);
char LIBCCALL *strpbrk(const char *string1, const char *string2);
char LIBCCALL *strtok(char *string1, const char *string2);

// Macros...
#define	strcmpi(s1, s2)			stricmp((s1), (s2))
#define strcoll(s1, s2)			strcmp((s1), (s2))
#define stricoll(s1, s2)		stricmp((s1), (s2))
#define strncoll(s1, s2, c)		strncmp((s1), (s2), (c))
#define strnicoll(s1, s2, c)	strnicmp((s1), (s2), (c))

#ifdef __cplusplus
}
#endif

#endif
