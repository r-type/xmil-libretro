
#define		FILEH				HANDLE
#define		FILEH_INVALID		(INVALID_HANDLE_VALUE)

enum {
	FSEEK_SET	= 0,
	FSEEK_CUR	= 1,
	FSEEK_END	= 2
};


#ifdef __cplusplus
extern "C" {
#endif

											// DOSIO:関数の準備
void dosio_init(void);
void dosio_term(void);
											// ファイル操作
FILEH __stdcall file_open(const OEMCHAR *path);
FILEH __stdcall file_open_rb(const OEMCHAR *path);
FILEH __stdcall file_create(const OEMCHAR *path);
long __stdcall file_seek(FILEH handle, long pointer, int method);
UINT __stdcall file_read(FILEH handle, void *data, UINT length);
UINT __stdcall file_write(FILEH handle, const void *data, UINT length);
short __stdcall file_close(FILEH handle);
UINT __stdcall file_getsize(FILEH handle);
short __stdcall file_delete(const OEMCHAR *path);
short __stdcall file_attr(const OEMCHAR *path);
short __stdcall file_dircreate(const OEMCHAR *path);

											// カレントファイル操作
void __stdcall file_setcd(const OEMCHAR *exename);
OEMCHAR * __stdcall file_getcd(const OEMCHAR *path);
FILEH __stdcall file_open_c(const OEMCHAR *path);
FILEH __stdcall file_open_rb_c(const OEMCHAR *path);
FILEH __stdcall file_create_c(const OEMCHAR *path);
short __stdcall file_attr_c(const OEMCHAR *path);

#define	file_cpyname(a, b, c)	milstr_ncpy(a, b, c)
#define	file_catname(a, b, c)	milstr_ncat(a, b, c)
#define	file_cmpname(a, b)		milstr_cmp(a, b)
OEMCHAR * __stdcall file_getname(const OEMCHAR *path);
void __stdcall file_cutname(OEMCHAR *path);
OEMCHAR * __stdcall file_getext(const OEMCHAR *path);
void __stdcall file_cutext(OEMCHAR *path);
void __stdcall file_cutseparator(OEMCHAR *path);
void __stdcall file_setseparator(OEMCHAR *path, int maxlen);

// LPSTR getFileName(LPSTR filename);
// void cutFileName(LPSTR filename);
// LPSTR getExtName(LPSTR filename);
// void cutExtName(LPSTR filename);
// void cutyen(LPSTR str);
// void fname_mix(LPSTR str, LPSTR mix, int size);

#ifdef __cplusplus
}
#endif

