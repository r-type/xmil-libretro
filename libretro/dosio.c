#include "compiler.h"
#include <sys/stat.h>
#include <time.h>
#include <string.h>
#include <assert.h>
#include "libretro.h"
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
struct retro_vfs_interface *vfs_interface;

FILEH wrap_file(FILE *f) {
	if (f == NULL)
		return NULL;
	FILEH ret = malloc(sizeof(*ret));
	if (ret == NULL) {
		return NULL;
	}
	memset(ret, 0, sizeof(*ret));
	ret->type = FILEH_STDIO;
	ret->f = f;
	return ret;
}

FILEH wrap_libretro_file(struct retro_vfs_file_handle *lr) {
	if (lr == NULL)
		return NULL;
	FILEH ret = malloc(sizeof(*ret));
	if (ret == NULL) {
		return NULL;
	}
	memset(ret, 0, sizeof(*ret));
	ret->type = FILEH_LIBRETRO;
	ret->lr = lr;
	return ret;
}

FILEH fopen_wrap(const char *path, const char *mode, unsigned retro_mode) {
	if (vfs_interface) {
		return wrap_libretro_file(vfs_interface->open(path, retro_mode, RETRO_VFS_FILE_ACCESS_HINT_NONE));
	}
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
	return(fopen_wrap(path, "rb+",
		       RETRO_VFS_FILE_ACCESS_UPDATE_EXISTING | RETRO_VFS_FILE_ACCESS_READ_WRITE));
}

FILEH file_open_rb(const char *path) {
        return(fopen_wrap(path, "rb",
		       RETRO_VFS_FILE_ACCESS_READ));
}

FILEH file_create(const char *path) {
	return(fopen_wrap(path, "wb+", RETRO_VFS_FILE_ACCESS_READ_WRITE));
}

int method_stdio_to_lr(int m) {
	switch (m) {
	case SEEK_SET:
		return RETRO_VFS_SEEK_POSITION_START;
	case SEEK_CUR:
		return RETRO_VFS_SEEK_POSITION_CURRENT;
	case SEEK_END:
		return RETRO_VFS_SEEK_POSITION_END;
	default:
		assert(0);
		return RETRO_VFS_SEEK_POSITION_START;
	}

}

long file_seek(FILEH handle, long pointer, int method) {
	switch (handle->type)
	{
	case FILEH_MEM: {
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
	case FILEH_LIBRETRO:
		vfs_interface->seek(handle->lr, pointer, method_stdio_to_lr(method));
		return(vfs_interface->tell(handle->lr));
	case FILEH_STDIO:
		fseek(handle->f, pointer, method);
		return(ftell(handle->f));
	default:
		assert(0);
		return -1;
	}
}

UINT file_read(FILEH handle, void *data, UINT length) {
	switch (handle->type)
	{
	case FILEH_MEM: {
		long real_len = length;
		if (real_len > handle->memsize - handle->memptr)
			real_len = handle->memsize - handle->memptr;
		if (real_len < 0)
			return -1;
		memcpy(data, handle->mem + handle->memptr, real_len);
		handle->memptr += real_len;
		return real_len;
	}
	case FILEH_LIBRETRO:
		return vfs_interface->read(handle->lr, data, length);
	case FILEH_STDIO:
		return((UINT)fread(data, 1, length, handle->f));
	default:
		assert(0);
		return -1;
	}		
}

UINT file_write(FILEH handle, const void *data, UINT length) {
	switch (handle->type)
	{
	case FILEH_MEM: {
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
	case FILEH_LIBRETRO:
		return vfs_interface->write(handle->lr, data, length);
	case FILEH_STDIO:
		return((UINT)fwrite(data, 1, length, handle->f));
	default:
		assert(0);
		return -1;
	}
}

short file_close(FILEH handle) {
	switch (handle->type)
	{
	case FILEH_MEM:
		return 0;
	case FILEH_STDIO:
		fclose(handle->f);
		return(0);
	case FILEH_LIBRETRO:
		vfs_interface->close(handle->lr);
		return(0);
	}
	return(0);
}

UINT file_getsize(FILEH handle) {
	switch (handle->type)
	{
	case FILEH_MEM:
		return handle->memsize;
	case FILEH_LIBRETRO:
		return vfs_interface->size(handle->lr);
	case FILEH_STDIO: {
			struct stat sb;

			if (fstat(fileno(handle->f), &sb) == 0)
			{
				return (UINT)sb.st_size;
			}
			return(0);
		}
	}
}

static BRESULT cnv_sttime(time_t *t, DOSDATE *dosdate, DOSTIME *dostime) {

struct tm      *ftime;

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
