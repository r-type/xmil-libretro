#include	"compiler.h"
#include	"dosio.h"


static	OEMCHAR	curpath[MAX_PATH];
static	OEMCHAR	*curfilep = curpath;

// ----

void dosio_init(void) { }
void dosio_term(void) { }

											// ファイル操作
FILEH __stdcall file_open(const OEMCHAR *path) {

	FILEH	ret;

	if ((ret = CreateFile(path, GENERIC_READ | GENERIC_WRITE,
						0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL))
													== INVALID_HANDLE_VALUE) {
		if ((ret = CreateFile(path, GENERIC_READ,
						0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL))
													== INVALID_HANDLE_VALUE) {
			return(FILEH_INVALID);
		}
	}
	return(ret);
}

FILEH __stdcall file_open_rb(const OEMCHAR *path) {

	FILEH	ret;

	if ((ret = CreateFile(path, GENERIC_READ, FILE_SHARE_READ, 0,
								OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL))
													== INVALID_HANDLE_VALUE) {
		return(FILEH_INVALID);
	}
	return(ret);
}

FILEH __stdcall file_create(const OEMCHAR *path) {

	FILEH	ret;

	if ((ret = CreateFile(path, GENERIC_READ | GENERIC_WRITE,
						 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL))
											== INVALID_HANDLE_VALUE) {
		return(FILEH_INVALID);
	}
	return(ret);
}

long __stdcall file_seek(FILEH handle, long pointer, int method) {

	return(SetFilePointer(handle, pointer, 0, method));
}

UINT __stdcall file_read(FILEH handle, void *data, UINT length) {

	DWORD	readsize;

	if (!ReadFile(handle, data, length, &readsize, NULL)) {
		return(0);
	}
	return(readsize);
}

UINT __stdcall file_write(FILEH handle, const void *data, UINT length) {

	DWORD	writesize;

	if (length) {
		if (WriteFile(handle, data, length, &writesize, NULL)) {
			return(writesize);
		}
	}
	else {
		SetEndOfFile(handle);
	}
	return(0);
}

short __stdcall file_close(FILEH handle) {

	CloseHandle(handle);
	return(0);
}

UINT __stdcall file_getsize(FILEH handle) {

	return(GetFileSize(handle, NULL));
}

short __stdcall file_delete(const OEMCHAR *path) {

	return(DeleteFile(path)?0:-1);
}

short __stdcall file_attr(const OEMCHAR *path) {

	return((short)GetFileAttributes(path));
}

short __stdcall file_dircreate(const OEMCHAR *path) {

	return(CreateDirectory(path, NULL)?0:-1);
}


											// カレントファイル操作
void __stdcall file_setcd(const OEMCHAR *exepath) {

	file_cpyname(curpath, exepath, NELEMENTS(curpath));
	curfilep = file_getname(curpath);
	*curfilep = '\0';
}

OEMCHAR * __stdcall file_getcd(const OEMCHAR *path) {

	*curfilep = '\0';
	file_catname(curpath, path, NELEMENTS(curpath));
	return(curpath);
}

FILEH __stdcall file_open_c(const OEMCHAR *path) {

	*curfilep = '\0';
	file_catname(curpath, path, NELEMENTS(curpath));
	return(file_open(curpath));
}

FILEH __stdcall file_open_rb_c(const OEMCHAR *path) {

	*curfilep = '\0';
	file_catname(curpath, path, NELEMENTS(curpath));
	return(file_open_rb(curpath));
}

FILEH __stdcall file_create_c(const OEMCHAR *path) {

	*curfilep = '\0';
	file_catname(curpath, path, NELEMENTS(curpath));
	return(file_create(curpath));
}

short __stdcall file_delete_c(const OEMCHAR *path) {

	*curfilep = '\0';
	file_catname(curpath, path, NELEMENTS(curpath));
	return(file_delete(curpath));
}

short __stdcall file_attr_c(const OEMCHAR *path) {

	*curfilep = '\0';
	file_catname(curpath, path, NELEMENTS(curpath));
	return(file_attr(curpath));
}


OEMCHAR * __stdcall file_getname(const OEMCHAR *path) {

const OEMCHAR	*ret;
	int			csize;

	ret = path;
	while((csize = milstr_charsize(path)) != 0) {
		if (csize == 1) {
			if ((*path == '\\') || (*path == '/') || (*path == ':')) {
				ret = path + 1;
			}
		}
		path += csize;
	}
	return((OEMCHAR *)ret);
}

void __stdcall file_cutname(OEMCHAR *path) {

	OEMCHAR	*p;

	p = file_getname(path);
	p[0] = '\0';
}

OEMCHAR * __stdcall file_getext(const OEMCHAR *path) {

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
	return((OEMCHAR *)q);
}

void __stdcall file_cutext(OEMCHAR *path) {

	OEMCHAR	*p;
	OEMCHAR	*q;

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

void __stdcall file_cutseparator(OEMCHAR *path) {

	int		pos;

	pos = strlen(path) - 1;
	if ((pos > 0) &&							// 2文字以上でー
		(path[pos] == '\\') &&					// ケツが \ でー
		(!milstr_kanji2nd(path, pos)) &&		// 漢字の2バイト目ぢゃなくてー
		((pos != 1) || (path[0] != '\\')) &&	// '\\' ではなくてー
		((pos != 2) || (path[1] != ':'))) {		// '?:\' ではなかったら
		path[pos] = '\0';
	}
}

void __stdcall file_setseparator(OEMCHAR *path, int maxlen) {

	int		pos;

	pos = strlen(path) - 1;
	if ((pos < 0) ||
		((pos == 1) && (path[1] == ':')) ||
		((path[pos] == '\\') && (!milstr_kanji2nd(path, pos))) ||
		((pos + 2) >= maxlen)) {
		return;
	}
	path[++pos] = '\\';
	path[++pos] = '\0';
}

