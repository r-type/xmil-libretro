
#define	FILEH			HANDLE
#define	FILEH_INVALID	(INVALID_HANDLE_VALUE)

enum {
	FSEEK_SET	= 0,
	FSEEK_CUR	= 1,
	FSEEK_END	= 2
};

typedef struct {
	UINT16	year;		// cx
	UINT8	month;		// dh
	UINT8	day;		// dl
} DOSDATE;

typedef struct {
	UINT8	hour;		// ch
	UINT8	minute;		// cl
	UINT8	second;		// dh
} DOSTIME;


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
BRESULT __stdcall file_close(FILEH handle);
UINT __stdcall file_getsize(FILEH handle);
BRESULT __stdcall file_getdatetime(FILEH handle, DOSDATE *dosdate, DOSTIME *dostime);
BRESULT __stdcall file_delete(const OEMCHAR *path);
SINT16 __stdcall file_attr(const OEMCHAR *path);
BRESULT __stdcall file_dircreate(const OEMCHAR *path);

											// カレントファイル操作
void __stdcall file_setcd(const OEMCHAR *exename);
OEMCHAR * __stdcall file_getcd(const OEMCHAR *path);
FILEH __stdcall file_open_c(const OEMCHAR *path);
FILEH __stdcall file_open_rb_c(const OEMCHAR *path);
FILEH __stdcall file_create_c(const OEMCHAR *path);
BRESULT __stdcall file_delete_c(const OEMCHAR *path);
SINT16 __stdcall file_attr_c(const OEMCHAR *path);

#define	file_cpyname(a, b, c)	milstr_ncpy(a, b, c)
#define	file_catname(a, b, c)	milstr_ncat(a, b, c)
#define	file_cmpname(a, b)		milstr_cmp(a, b)
OEMCHAR * __stdcall file_getname(const OEMCHAR *path);
void __stdcall file_cutname(OEMCHAR *path);
OEMCHAR * __stdcall file_getext(const OEMCHAR *path);
void __stdcall file_cutext(OEMCHAR *path);
void __stdcall file_cutseparator(OEMCHAR *path);
void __stdcall file_setseparator(OEMCHAR *path, int maxlen);

#ifdef __cplusplus
}
#endif

#define	file_createex(p, t)		file_create(p)
#define	file_createex_c(p, t)	file_create_c(p)

