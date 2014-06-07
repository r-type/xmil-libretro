#ifndef _ADVLIBC_STDIO_
#define _ADVLIBC_STDIO_

#include	"./advtypes.h"

#define	EOF		(-1)

enum {
	_IOREAD			= 0x01,
	_IOWRT			= 0x02,
	_IORW			= 0x04,
	_IOERR			= 0x08,
	_IOEOF			= 0x10
};

typedef struct {
	char	*_ptr;
	int		_count;
	char	*_base;
	int		_flag;
	int		_file;
} FILE;

#define stdin  (__iob + 0)
#define stdout (__iob + 1)
#define stderr (__iob + 2)
#define stdaux (__iob + 3)
#define stdprn (__iob + 4)


#ifdef __cplusplus
extern "C" {
#endif

extern FILE __iob[];

int LIBCCALL _filbuf(FILE *stream);
int LIBCCALL _flsbuf(int c, FILE *stream);

FILE LIBCCALL *fopen(const char *filename, const char *mode);
int LIBCCALL fclose(FILE *stream);
int LIBCCALL fseek(FILE *stream, long offset, int origin);
long LIBCCALL ftell(FILE *stream);
size_t LIBCCALL fread(void *buf, size_t size, size_t count, FILE *stream);
size_t LIBCCALL fwrite(const void *buf, size_t size, size_t count, FILE *stream);
int LIBCCALL fgetc(FILE *stream);
char LIBCCALL *fgets(char *string, int, FILE *stream);
int LIBCCALL fputc(int c, FILE *stream);
int LIBCCALL fputs(const char *string, FILE *stream);

int LIBCCALL getch(void);
int LIBCCALL getche(void);
char LIBCCALL *gets(char *string);
int LIBCCALL putch(int c);
int LIBCCALL puts(const char *string);

int LIBCCALL printf(const char *format, ...);
int LIBCCALL cprintf(const char *format, ...);
int LIBCCALL sprintf(char *buffer, const char *format, ...);
int LIBCCALL fprintf(FILE *stream, const char *format, ...);
int LIBCCALL vprintf(const char *format, va_list argptr);
int LIBCCALL vcprintf(const char *format, va_list argptr);
int LIBCCALL vsprintf(char *buffer, const char *format, va_list argptr);
int LIBCCALL vfprintf(FILE *stream, const char *format, va_list argptr);

// Macros
#define	fileno(s)		((s)->_file)
#define	feof(s)			((s)->_flag & _IOEOF)
#define	ferror(s)		((s)->_flag & _IOERR)
#define getc(s)			(((s)->_count-- > 0)?(*((s)->_ptr)++)&0xff:_filbuf(s))
#define	putc(c, s)		_flsbuf((c), (s))
#define getchar()		getc(stdin)
#define putchar(c)		putc((c), stdout)

#ifdef __cplusplus
}
#endif

#endif
