#include	"compiler.h"
#include	"dosio.h"


static TCHAR s_szCurPath[MAX_PATH];
static LPTSTR s_lpszFile = s_szCurPath;

// ----

void dosio_init() { }
void dosio_term() { }

											// ファイル操作
FILEH __stdcall file_open(LPCTSTR lpcszPath)
{
	FILEH fhRet = CreateFile(lpcszPath, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (fhRet == INVALID_HANDLE_VALUE)
	{
		fhRet = CreateFile(lpcszPath, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	}
	if (fhRet == INVALID_HANDLE_VALUE)
	{
		return FILEH_INVALID;
	}
	return fhRet;
}

FILEH __stdcall file_open_rb(LPCTSTR lpcszPath)
{
	FILEH fhRet = CreateFile(lpcszPath, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (fhRet == INVALID_HANDLE_VALUE)
	{
		return FILEH_INVALID;
	}
	return fhRet;
}

FILEH __stdcall file_create(LPCTSTR lpcszPath)
{
	FILEH fhRet = CreateFile(lpcszPath, GENERIC_READ | GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (fhRet == INVALID_HANDLE_VALUE)
	{
		return FILEH_INVALID;
	}
	return fhRet;
}

long __stdcall file_seek(FILEH handle, long lPointer, int nMethod)
{
	return SetFilePointer(handle, lPointer, 0, nMethod);
}

UINT __stdcall file_read(FILEH handle, LPVOID lpvData, UINT uLength)
{
	DWORD dwReadSize;
	if (!ReadFile(handle, lpvData, uLength, &dwReadSize, NULL))
	{
		return 0;
	}
	return dwReadSize;
}

UINT __stdcall file_write(FILEH handle, LPCVOID lpcvData, UINT uLength)
{
	if (uLength)
	{
		DWORD dwWriteSize;
		if (WriteFile(handle, lpcvData, uLength, &dwWriteSize, NULL))
		{
			return dwWriteSize;
		}
	}
	else
	{
		SetEndOfFile(handle);
	}
	return 0;
}

BRESULT __stdcall file_close(FILEH handle)
{
	CloseHandle(handle);
	return 0;
}

UINT __stdcall file_getsize(FILEH handle)
{
	return GetFileSize(handle, NULL);
}

static BRESULT cnvdatetime(LPFILETIME lpFt, PDOSDATE pDosDate, PDOSTIME pDosTime)
{
	FILETIME localtime;
	if (FileTimeToLocalFileTime(lpFt, &localtime) == 0)
	{
		return FAILURE;
	}

	SYSTEMTIME systime;
	if (FileTimeToSystemTime(&localtime, &systime) == 0)
	{
		return FAILURE;
	}

	if (pDosDate)
	{
		pDosDate->year = (UINT16)systime.wYear;
		pDosDate->month = (UINT8)systime.wMonth;
		pDosDate->day = (UINT8)systime.wDay;
	}
	if (pDosTime)
	{
		pDosTime->hour = (UINT8)systime.wHour;
		pDosTime->minute = (UINT8)systime.wMinute;
		pDosTime->second = (UINT8)systime.wSecond;
	}
	return SUCCESS;
}

BRESULT __stdcall file_getdatetime(FILEH handle, PDOSDATE pDosDate, PDOSTIME pDosTime)
{
	FILETIME lastwrite;
	if ((GetFileTime(handle, NULL, NULL, &lastwrite) == 0) ||
		(cnvdatetime(&lastwrite, pDosDate, pDosTime) != SUCCESS))
	{
		return FAILURE;
	}
	return SUCCESS;
}

BRESULT __stdcall file_delete(LPCTSTR lpcszPath)
{
	return (DeleteFile(lpcszPath) ? SUCCESS : FAILURE);
}

SINT16 __stdcall file_attr(LPCTSTR lpcszPath)
{
	return static_cast<SINT16>(GetFileAttributes(lpcszPath));
}

BRESULT __stdcall file_dircreate(LPCTSTR lpcszPath)
{
	return (CreateDirectory(lpcszPath, NULL) ? SUCCESS : FAILURE);
}


											// カレントファイル操作
void __stdcall file_setcd(LPCTSTR lpcszExePath)
{
	file_cpyname(s_szCurPath, lpcszExePath, NELEMENTS(s_szCurPath));
	s_lpszFile = file_getname(s_szCurPath);
	*s_lpszFile = '\0';
}

OEMCHAR * __stdcall file_getcd(LPCTSTR lpcszPath)
{
	*s_lpszFile = '\0';
	file_catname(s_szCurPath, lpcszPath, NELEMENTS(s_szCurPath));
	return s_szCurPath;
}

FILEH __stdcall file_open_c(LPCTSTR lpcszPath)
{
	*s_lpszFile = '\0';
	file_catname(s_szCurPath, lpcszPath, NELEMENTS(s_szCurPath));
	return file_open(s_szCurPath);
}

FILEH __stdcall file_open_rb_c(LPCTSTR lpcszPath)
{
	*s_lpszFile = '\0';
	file_catname(s_szCurPath, lpcszPath, NELEMENTS(s_szCurPath));
	return file_open_rb(s_szCurPath);
}

FILEH __stdcall file_create_c(LPCTSTR lpcszPath)
{
	*s_lpszFile = '\0';
	file_catname(s_szCurPath, lpcszPath, NELEMENTS(s_szCurPath));
	return file_create(s_szCurPath);
}

BRESULT __stdcall file_delete_c(LPCTSTR lpcszPath)
{
	*s_lpszFile = '\0';
	file_catname(s_szCurPath, lpcszPath, NELEMENTS(s_szCurPath));
	return file_delete(s_szCurPath);
}

SINT16 __stdcall file_attr_c(LPCTSTR lpcszPath)
{
	*s_lpszFile = '\0';
	file_catname(s_szCurPath, lpcszPath, NELEMENTS(s_szCurPath));
	return file_attr(s_szCurPath);
}


LPTSTR __stdcall file_getname(LPCTSTR lpcszPath)
{
	LPCTSTR lpcszRet = lpcszPath;
	int nSize;
	while((nSize = milstr_charsize(lpcszPath)) != 0)
	{
		if (nSize == 1)
		{
			if ((*lpcszPath == '\\') || (*lpcszPath == '/') || (*lpcszPath == ':'))
			{
				lpcszRet = lpcszPath + 1;
			}
		}
		lpcszPath += nSize;
	}
	return const_cast<LPTSTR>(lpcszRet);
}

void __stdcall file_cutname(LPTSTR lpszPath)
{
	LPTSTR p = file_getname(lpszPath);
	p[0] = '\0';
}

LPTSTR __stdcall file_getext(LPCTSTR lpcszPath)
{
	LPCTSTR p = file_getname(lpcszPath);
	LPCTSTR q = 0;
	while(*p != '\0')
	{
		if (*p == '.')
		{
			q = p + 1;
		}
		p++;
	}
	if (!q)
	{
		q = p;
	}
	return const_cast<LPTSTR>(q);
}

void __stdcall file_cutext(LPTSTR lpszPath)
{
	LPTSTR p = file_getname(lpszPath);
	LPTSTR q = 0;
	while(*p != '\0')
	{
		if (*p == '.')
		{
			q = p;
		}
		p++;
	}
	if (q)
	{
		*q = '\0';
	}
}

void __stdcall file_cutseparator(LPTSTR lpszPath)
{
	int nPos = lstrlen(lpszPath) - 1;
	if ((nPos > 0) &&								// 2文字以上でー
		(lpszPath[nPos] == '\\') &&					// ケツが \ でー
		(!milstr_kanji2nd(lpszPath, nPos)) &&		// 漢字の2バイト目ぢゃなくてー
		((nPos != 1) || (lpszPath[0] != '\\')) &&	// '\\' ではなくてー
		((nPos != 2) || (lpszPath[1] != ':')))		// '?:\' ではなかったら
	{
		lpszPath[nPos] = '\0';
	}
}

void __stdcall file_setseparator(LPTSTR lpszPath, int nMaxLen)
{
	const int nPos = lstrlen(lpszPath) - 1;
	if ((nPos < 0) ||
		((nPos == 1) && (lpszPath[1] == ':')) ||
		((lpszPath[nPos] == '\\') && (!milstr_kanji2nd(lpszPath, nPos))) ||
		((nPos + 2) >= nMaxLen))
	{
		return;
	}
	lpszPath[nPos + 1] = '\\';
	lpszPath[nPos + 2] = '\0';
}

