#include "compiler.h"
#include <sys/stat.h>
#include <time.h>
#include <string.h>
#if defined(WIN32) && defined(OSLANG_UTF8)
#include "codecnv/codecnv.h"
#endif
#include "dosio.h"
#if defined(WIN32)
#include <direct.h>
#else
#include <dirent.h>
#endif

static	char	curpath[MAX_PATH] = "./";
static	char	*curfilep = curpath + 2;

FILEH wrap_file(FILE *f) {
  if (f == NULL)
    return f;
  FILEH ret = malloc(sizeof(*ret));
  if (ret == NULL) {
    return NULL;
  }
  memset(ret, 0, sizeof(*ret));
  ret->type = FILEH_FILE;
  ret->f = f;
  return ret;
}

FILEH fopen_wrap(const char *path, const char *mode) {
#if defined(WIN32) && defined(OSLANG_UTF8)
  char	sjis[MAX_PATH];
  codecnv_utf8tosjis(sjis, SDL_arraysize(sjis), path, (UINT)-1);
  return wrap_file(fopen(sjis, mode));
#else
  return wrap_file(fopen(path, mode));
#endif
}

/* ファイル操作 */
FILEH file_open(const char *path) {
	return(fopen_wrap(path, "rb+"));
}

FILEH file_open_rb(const char *path) {
        return(fopen_wrap(path, "rb"));
}

FILEH file_create(const char *path) {
	return(fopen_wrap(path, "wb+"));
}

long file_seek(FILEH handle, long pointer, int method) {
  if (handle->type == FILEH_MEM) {
    long oldptr = handle->memptr, newptr = oldptr;
    switch (method) {
    case SEEK_SET:
      newptr = pointer;
      break;
    case SEEK_CUR:
      newptr = oldptr + pointer;
      break;
    case SEEK_END:
      newptr = handle->memsize - pointer;
      break;
    }
    if (newptr >= handle->memsize)
      pointer = handle->memsize - 1;
    if (newptr < 0)
      newptr = 0;
    handle->memptr = newptr;
    return newptr;
  }

	fseek(handle->f, pointer, method);
	return(ftell(handle->f));
}

UINT file_read(FILEH handle, void *data, UINT length) {
  if (handle->type == FILEH_MEM) {
    long real_len = length;
    if (real_len > handle->memsize - handle->memptr)
      real_len = handle->memsize - handle->memptr;
    if (real_len < 0)
      return -1;
    memcpy(data, handle->mem + handle->memptr, real_len);
    handle->memptr += real_len;
    return real_len;
  }

	return((UINT)fread(data, 1, length, handle->f));
}

UINT file_write(FILEH handle, const void *data, UINT length) {
  if (handle->type == FILEH_MEM) {
    if (!handle->writeable)
      return -1;
    if (handle->memalloc <= handle->memptr + length) {
      long newsize = 2 * (handle->memptr + length);
      void *newmem = realloc(handle->mem, newsize);
      if (newmem == NULL) {
	return -1;
      }
      handle->mem = newmem;
      handle->memalloc = newsize;
    }
    if (handle->memptr > handle->memsize)
      memset(handle->mem + handle->memsize, 0, handle->memptr - handle->memsize);
    memcpy(handle->mem + handle->memptr, data, length);
    handle->memptr += length;
    if (handle->memptr > handle->memsize)
      handle->memsize = handle->memptr;
    return length;
  }

	return((UINT)fwrite(data, 1, length, handle->f));
}

short file_close(FILEH handle) {
  if (handle->type != FILEH_MEM)
	fclose(handle->f);
	return(0);
}

UINT file_getsize(FILEH handle) {
  if (handle->type == FILEH_MEM)
    return handle->memsize;

	struct stat sb;

	if (fstat(fileno(handle->f), &sb) == 0)
	{
		return (UINT)sb.st_size;
	}
	return(0);
}

short file_attr(const char *path) {

struct stat	sb;
	short	attr;

#if defined(WIN32) && defined(OSLANG_UTF8)
	char	sjis[MAX_PATH];
	codecnv_utf8tosjis(sjis, SDL_arraysize(sjis), path, (UINT)-1);
	if (stat(sjis, &sb) == 0)
#else
	if (stat(path, &sb) == 0)
#endif
	{
#if defined(WIN32)
		if (sb.st_mode & _S_IFDIR) {
			attr = FILEATTR_DIRECTORY;
		}
		else {
			attr = 0;
		}
		if (!(sb.st_mode & S_IWRITE)) {
			attr |= FILEATTR_READONLY;
		}
#else
		if (S_ISDIR(sb.st_mode)) {
			return(FILEATTR_DIRECTORY);
		}
		attr = 0;
		if (!(sb.st_mode & S_IWUSR)) {
			attr |= FILEATTR_READONLY;
		}
#endif
		return(attr);
	}
	return(-1);
}

static BRESULT cnv_sttime(time_t *t, DOSDATE *dosdate, DOSTIME *dostime) {

struct tm	*ftime;

	ftime = localtime(t);
	if (ftime == NULL) {
		return(FAILURE);
	}
	if (dosdate) {
		dosdate->year = ftime->tm_year + 1900;
		dosdate->month = ftime->tm_mon + 1;
		dosdate->day = ftime->tm_mday;
	}
	if (dostime) {
		dostime->hour = ftime->tm_hour;
		dostime->minute = ftime->tm_min;
		dostime->second = ftime->tm_sec;
	}
	return(SUCCESS);
}

short file_getdatetime(FILEH handle, DOSDATE *dosdate, DOSTIME *dostime) {
  if (handle->type == FILEH_MEM)
    return (-1);

struct stat sb;

	if (fstat(fileno(handle->f), &sb) == 0) {
		if (cnv_sttime(&sb.st_mtime, dosdate, dostime) == SUCCESS) {
			return(0);
		}
	}
	return(-1);
}

short file_delete(const char *path) {

	return(remove(path));
}

/* カレントファイル操作 */
void file_setcd(const char *exepath) {

	file_cpyname(curpath, exepath, SDL_arraysize(curpath));
	curfilep = file_getname(curpath);
	*curfilep = '\0';
}

char *file_getcd(const char *path) {

	file_cpyname(curfilep, path, SDL_arraysize(curpath) - (UINT)(curfilep - curpath));
	return(curpath);
}

FILEH file_open_c(const char *path) {

	file_cpyname(curfilep, path, SDL_arraysize(curpath) - (UINT)(curfilep - curpath));
	return(file_open(curpath));
}

FILEH file_open_rb_c(const char *path) {

	file_cpyname(curfilep, path, SDL_arraysize(curpath) - (UINT)(curfilep - curpath));
	return(file_open_rb(curpath));
}

FILEH file_create_c(const char *path) {

	file_cpyname(curfilep, path, SDL_arraysize(curpath) - (UINT)(curfilep - curpath));
	return(file_create(curpath));
}

void file_catname(char *path, const char *name, int maxlen) {

	int		csize;

	while(maxlen > 0) {
		if (*path == '\0') {
			break;
		}
		path++;
		maxlen--;
	}
	file_cpyname(path, name, maxlen);
	while((csize = milstr_charsize(path)) != 0) {
		if ((csize == 1) && (*path == '\\')) {
			*path = '/';
		}
		path += csize;
	}
}

char *file_getname(const char *path) {

const char	*ret;
	int		csize;

	ret = path;
	while((csize = milstr_charsize(path)) != 0) {
		if ((csize == 1) && (*path == '/')) {
			ret = path + 1;
		}
		path += csize;
	}
	return((char *)ret);
}

void file_cutname(char *path) {

	char	*p;

	p = file_getname(path);
	*p = '\0';
}

char *file_getext(const char *path) {

const char	*p;
const char	*q;

	p = file_getname(path);
	q = NULL;
	while(*p != '\0') {
		if (*p == '.') {
			q = p + 1;
		}
		p++;
	}
	if (q == NULL) {
		q = p;
	}
	return((char *)q);
}

void file_cutext(char *path) {

	char	*p;
	char	*q;

	p = file_getname(path);
	q = NULL;
	while(*p != '\0') {
		if (*p == '.') {
			q = p;
		}
		p++;
	}
	if (q != NULL) {
		*q = '\0';
	}
}

void file_cutseparator(char *path) {

	int		pos;

	pos = (int)strlen(path) - 1;
	if ((pos > 0) &&							// 2文字以上でー
		(path[pos] == '/') &&					// ケツが \ でー
		((pos != 1) || (path[0] != '.'))) {		// './' ではなかったら
		path[pos] = '\0';
	}
}

void file_setseparator(char *path, int maxlen) {

	int		pos;

	pos = (int)strlen(path);
	if ((pos) && (path[pos-1] != '/') && ((pos + 2) < maxlen)) {
		path[pos++] = '/';
		path[pos] = '\0';
	}
}

FILEH make_readmem_file(void *buf, long size) {
  FILEH ret = malloc(sizeof(*ret));
  if (ret == NULL) {
    return NULL;
  }
  ret->type = FILEH_MEM;
  ret->memsize = size;
  ret->memalloc = -1;
  ret->mem = buf;
  ret->memptr = 0;
  ret->writeable = 0;

  return ret;
}

FILEH make_writemem_file() {
  FILEH ret = malloc(sizeof(*ret));
  if (ret == NULL) {
    return NULL;
  }
  ret->type = FILEH_MEM;
  ret->writeable = 1;
  ret->memalloc = 256;
  ret->memsize = 0;
  ret->mem = malloc(ret->memalloc);
  if (ret->mem == NULL)
    return NULL;
  ret->memptr = 0;

  return ret;
}
