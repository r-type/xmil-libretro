#include	"compiler.h"
#include	"dosio.h"
#include	"pealstub.h"

static	UINT8	curpath[MAX_PATH];
static	char	*curfilep= curpath + 1;

void*   Callback_vfs[11];


#define ISKANJI(c)	(((((c) ^ 0x20) - 0xa1) & 0xff) < 0x3c)

void dosio_init(void) { }
void dosio_term(void) { }

											// ファイル操作
FILEH file_open(const char *path) {
	UInt32 arg;
	arg = ByteSwap32(path);

	return((FILEH)(gCall68KFuncP)(gEmulStateP, (unsigned long)Callback_vfs[0], &arg, 4));
}

FILEH file_open_rb(const char *path) {

	return(file_open(path));
}

FILEH file_create(const char *path) {
	UInt32 arg;
	arg = ByteSwap32(path);

	return((FILEH)(gCall68KFuncP)(gEmulStateP, (unsigned long)Callback_vfs[1], &arg, 4));
}

long file_seek(FILEH handle, long pointer, int method) {
	UInt32 arg[3];
	arg[0] = ByteSwap32(handle);
	arg[1] = ByteSwap32(pointer);
	arg[2] = ByteSwap32(method);

	return((long)(gCall68KFuncP)(gEmulStateP, (unsigned long)Callback_vfs[2], &arg, 12));
}

UINT file_read(FILEH handle, void *data, UINT length) {
	UInt32 arg[3];
	arg[0] = ByteSwap32(handle);
	arg[1] = ByteSwap32(data);
	arg[2] = ByteSwap32(length);

	return((UINT)(gCall68KFuncP)(gEmulStateP, (unsigned long)Callback_vfs[3], &arg, 12));
}

UINT file_write(FILEH handle, const void *data, UINT length) {
	UInt32 arg[3];
	arg[0] = ByteSwap32(handle);
	arg[1] = ByteSwap32(data);
	arg[2] = ByteSwap32(length);

	return((UINT)(gCall68KFuncP)(gEmulStateP, (unsigned long)Callback_vfs[4], &arg, 12));
}

short file_close(FILEH handle) {
	ArgOne params;
	params.ptr = ByteSwap32(handle);

	(gCall68KFuncP)(gEmulStateP, (unsigned long)Callback_vfs[5], &params, 4);
	return(0);
}

UINT file_getsize(FILEH handle) {
	ArgOne arg;
	arg.ptr = ByteSwap32(handle);

	return((UINT)(gCall68KFuncP)(gEmulStateP, (unsigned long)Callback_vfs[6], &arg, 4));
}

static void cnvdatetime(UINT32 nowseconds, DOSDATE *dosdate, DOSTIME *dostime) {

	DateTimeType 	now_time;

	ARM_TimSecondsToDateTime(nowseconds, &now_time);

	if (dosdate) {
		dosdate->year = now_time.year;
		dosdate->month = now_time.month;
		dosdate->day = now_time.day;
	}
	if (dostime) {
		dostime->hour = now_time.hour;
		dostime->minute = now_time.minute;
		dostime->second = now_time.second;
	}
}

short file_getdatetime(FILEH handle, DOSDATE *dosdate, DOSTIME *dostime) {

	UINT32  seconds;
	ArgOne params;
	params.ptr = ByteSwap32(handle);
	
	seconds = (UINT32)(gCall68KFuncP)(gEmulStateP, (unsigned long)Callback_vfs[7], &params, 4);
	if (!seconds) {
		return(-1);
	}
	cnvdatetime(seconds, dosdate, dostime);
	return(0);
}

short file_delete(const char *path) {
	ArgOne params;
	params.ptr = ByteSwap32(path);

	return((short)(gCall68KFuncP)(gEmulStateP, (unsigned long)Callback_vfs[8], &params, 4));
}

short file_attr(const char *path) {

	FILEH   handle;
	ArgOne params;
	UInt32  ret;
	
	handle = file_open(path);
	if (handle == FILEH_INVALID) {
		return(-1);
	}
	params.ptr = ByteSwap32(handle);
	ret = (UINT32)(gCall68KFuncP)(gEmulStateP, (unsigned long)Callback_vfs[9], &params, 4);
	file_close(handle);
	return(ret);
}

short file_dircreate(const char *path) {
	ArgOne params;
	params.ptr = ByteSwap32(path);

	return((short)(gCall68KFuncP)(gEmulStateP, (unsigned long)Callback_vfs[10], &params, 4));
}

											// カレントファイル操作
void file_setcd(const char *exepath) {

	file_cpyname(curpath, exepath, sizeof(curpath));
	curfilep = file_getname(curpath);
	*curfilep = '\0';
}

char *file_getcd(const char *path) {

	*curfilep = '\0';
	file_catname(curpath, path, sizeof(curpath));
	return(curpath);
}

FILEH file_open_c(const char *path) {

	*curfilep = '\0';
	file_catname(curpath, path, sizeof(curpath));
	return(file_open(curpath));
}

FILEH file_open_rb_c(const char *path) {

	*curfilep = '\0';
	file_catname(curpath, path, sizeof(curpath));
	return(file_open_rb(curpath));
}

FILEH file_create_c(const char *path) {

	*curfilep = '\0';
	file_catname(curpath, path, sizeof(curpath));
	return(file_create(curpath));
}

short file_delete_c(const char *path) {

	*curfilep = '\0';
	file_catname(curpath, path, sizeof(curpath));
	return(file_delete(curpath));
}

short file_attr_c(const char *path) {

	*curfilep = '\0';
	file_catname(curpath, path, sizeof(curpath));
	return(file_attr(curpath));
}

/*
typedef struct {
	BOOL			eoff;
	FSIterator		fsi;
	FSCatalogInfo	fsci;
	HFSUniStr255	name;
} _FLHDL, *FLHDL;

static void char2str(char *dst, int size, const UniChar *uni, int unicnt) {

	CFStringRef	cfsr;

	cfsr = CFStringCreateWithCharacters(NULL, uni, unicnt);
	CFStringGetCString(cfsr, dst, size, CFStringGetSystemEncoding());
	CFRelease(cfsr);
}

void *file_list1st(const char *dir, FLINFO *fli) {

	FLISTH		ret;
	Str255		fname;
	FSSpec		fss;
	FSRef		fsr;
	FSIterator	fsi;

	mkstr255(fname, dir);
	if ((FSMakeFSSpec(0, 0, fname, &fss) != noErr) ||
		(FSpMakeFSRef(&fss, &fsr) != noErr) ||
		(FSOpenIterator(&fsr, kFSIterateFlat, &fsi) != noErr)) {
		goto ff1_err1;
	}
	ret = _MALLOC(sizeof(_FLHDL), dir);
	if (ret == NULL) {
		goto ff1_err2;
	}
	((FLHDL)ret)->eoff = FALSE;
	((FLHDL)ret)->fsi = fsi;
	if (file_listnext(ret, fli) == SUCCESS) {
		return(ret);
	}

ff1_err2:
	FSCloseIterator(fsi);

ff1_err1:
	return(NULL);
}

BOOL file_listnext(FLISTH hdl, FLINFO *fli) {

	FLHDL		flhdl;
	ItemCount	count;
	OSStatus	r;
	UTCDateTime	*dt;

	flhdl = (FLHDL)hdl;
	if ((flhdl == NULL) || (flhdl->eoff)) {
		goto ffn_err;
	}
	r = FSGetCatalogInfoBulk(flhdl->fsi, 1, &count, NULL,
						kFSCatInfoNodeFlags | kFSCatInfoDataSizes |
						kFSCatInfoAllDates,
						&flhdl->fsci, NULL, NULL, &flhdl->name);
	if (r != noErr) {
		flhdl->eoff = TRUE;
		if (r != errFSNoMoreItems) {
			goto ffn_err;
		}
	}
	if (count != 1) {
		flhdl->eoff = TRUE;
		goto ffn_err;
	}
	if (fli) {
		fli->caps = FLICAPS_SIZE | FLICAPS_ATTR | FLICAPS_DATE | FLICAPS_TIME;
		if (flhdl->fsci.nodeFlags & kFSNodeIsDirectoryMask) {
			fli->attr = FILEATTR_DIRECTORY;
			fli->size = 0;
			dt = &flhdl->fsci.createDate;
		}
		else {
			fli->attr = FILEATTR_ARCHIVE;
			fli->size = (UINT32)flhdl->fsci.dataLogicalSize;
			dt = &flhdl->fsci.contentModDate;
		}
		if (flhdl->fsci.nodeFlags & kFSNodeLockedMask) {
			fli->attr |= FILEATTR_READONLY;
		}
		cnvdatetime(dt, &fli->date, &fli->time);
		char2str(fli->path, sizeof(fli->path),
								flhdl->name.unicode, flhdl->name.length);
	}
	return(SUCCESS);

ffn_err:
	return(FAILURE);
}

void file_listclose(FLISTH hdl) {

	if (hdl) {
		FSCloseIterator(((FLHDL)hdl)->fsi);
		_MFREE(hdl);
	}
}

BOOL getLongFileName(char *dst, const char *path) {

	FSSpec			fss;
	Str255			fname;
	FSRef			fref;
	HFSUniStr255	name;

	if (*path == '\0') {
		return(false);
	}
	mkstr255(fname, path);
	FSMakeFSSpec(0, 0, fname, &fss);
	FSpMakeFSRef(&fss, &fref);
	if (FSGetCatalogInfo(&fref, kFSCatInfoNone, NULL, &name, NULL, NULL)
																!= noErr) {
		return(false);
	}
	char2str(dst, 512, name.unicode, name.length);
	if (!dst) {
		return(false);
	}
	return(true);
}
*/

void file_catname(char *path, const char *sjis, int maxlen) {

	char	*p;

	p = path + OEMSTRLEN(path);
	milstr_ncat(path, sjis, maxlen);
	while(1) {
		if (ISKANJI(*p)) {
			if (*(p+1) == '\0') {
				break;
			}
			p++;
		}
		else if ((*p == '/') || (*p == '\\')) {
			*p = '/';
		}
		else if (*p == '\0') {
			break;
		}
		p++;
	}
}

char *file_getname(const char *path) {

const char 	*ret;

	ret = path;
	while(*path != '\0') {
		if (*path++ == '/') {
			ret = path;
		}
	}
	return((char *)ret);
}

void file_cutname(char *path) {

	char 	*p;

	p = file_getname(path);
	p[0] = '\0';
}

char *file_getext(const char *path) {

const char	*p;
const char	*q;

	p = file_getname(path);
	q = NULL;

	while(*p != '\0') {
		if (*p++ == '.') {
			q = p;
		}
	}
	if (!q) {
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
	if (q) {
		*q = '\0';
	}
}

void file_cutseparator(char *path) {

	int		pos;

	pos = OEMSTRLEN(path) - 1;
	if ((pos > 0) && (path[pos] == '/')) {
		path[pos] = '\0';
	}
}

void file_setseparator(char *path, int maxlen) {

	int		pos;

	pos = OEMSTRLEN(path) - 1;
	if ((pos < 0) || (path[pos] == '/') || ((pos + 2) >= maxlen)) {
		return;
	}
	path[++pos] = '/';
	path[++pos] = '\0';
}

