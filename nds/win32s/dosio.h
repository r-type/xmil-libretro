
#define	FILEH			HANDLE
#define	FILEH_INVALID	(INVALID_HANDLE_VALUE)

enum
{
	FSEEK_SET	= 0,
	FSEEK_CUR	= 1,
	FSEEK_END	= 2
};

struct tagDosDate
{
	UINT16 year;		// cx
	UINT8 month;		// dh
	UINT8 day;			// dl
};
typedef struct tagDosDate		DOSDATE;
typedef struct tagDosDate		*PDOSDATE;

struct tagDosTime
{
	UINT8 hour;			// ch
	UINT8 minute;		// cl
	UINT8 second;		// dh
};
typedef struct tagDosTime		DOSTIME;
typedef struct tagDosTime		*PDOSTIME;


#ifdef __cplusplus
extern "C"
{
#endif

											// DOSIO:関数の準備
void dosio_init();
void dosio_term();
											// ファイル操作
FILEH __stdcall file_open(LPCTSTR lpcszPath);
FILEH __stdcall file_open_rb(LPCTSTR lpcszPath);
FILEH __stdcall file_create(LPCTSTR lpcszPath);
long __stdcall file_seek(FILEH handle, long lPointer, int nMethod);
UINT __stdcall file_read(FILEH handle, LPVOID lpvData, UINT uLength);
UINT __stdcall file_write(FILEH handle, LPCVOID lpcvData, UINT uLength);
BRESULT __stdcall file_close(FILEH handle);
UINT __stdcall file_getsize(FILEH handle);
BRESULT __stdcall file_getdatetime(FILEH handle, PDOSDATE pDosDate, PDOSTIME pDosTime);
BRESULT __stdcall file_delete(LPCTSTR lpcszPath);
SINT16 __stdcall file_attr(LPCTSTR lpcszPath);
BRESULT __stdcall file_dircreate(LPCTSTR lpcszPath);

											// カレントファイル操作
void __stdcall file_setcd(LPCTSTR lpcszExeName);
LPTSTR __stdcall file_getcd(LPCTSTR lpcszPath);
FILEH __stdcall file_open_c(LPCTSTR lpcszPath);
FILEH __stdcall file_open_rb_c(LPCTSTR lpcszPath);
FILEH __stdcall file_create_c(LPCTSTR lpcszPath);
BRESULT __stdcall file_delete_c(LPCTSTR lpcszPath);
SINT16 __stdcall file_attr_c(LPCTSTR lpcszPath);

#define	file_cpyname(a, b, c)	milstr_ncpy(a, b, c)
#define	file_catname(a, b, c)	milstr_ncat(a, b, c)
#define	file_cmpname(a, b)		milstr_cmp(a, b)
LPTSTR __stdcall file_getname(LPCTSTR lpcszPath);
void __stdcall file_cutname(LPTSTR lpszPath);
LPTSTR __stdcall file_getext(LPCTSTR lpcszPath);
void __stdcall file_cutext(LPTSTR lpszPath);
void __stdcall file_cutseparator(LPTSTR lpszPath);
void __stdcall file_setseparator(LPTSTR lpszPath, int nMaxLen);

#ifdef __cplusplus
}
#endif

#define	file_createex(p, t)		file_create(p)
#define	file_createex_c(p, t)	file_create_c(p)

