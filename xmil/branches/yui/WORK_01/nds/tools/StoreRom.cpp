#include <windows.h>
#include <tchar.h>
#include <stdio.h>


typedef unsigned char		UINT8;
typedef unsigned int		UINT32;

struct tagRomImg
{
	UINT8	bios1[0x1000];
	UINT8	bios2[0x8000];
	UINT8	ank[0x0800];
	UINT8	txt[0x1000];
	UINT8	knjx1[0x20000];
	UINT8	knjx1t[0x40000];

	UINT32 uDisk1Size;
	UINT32 uDisk2Size;
	UINT8 sDisk1[512*1024];
	UINT8 sDisk2[512*1024];
};

static const unsigned char s_cExtRom[] = {'R','O','M','A','R','E','A'};


static void lstrcatn(LPTSTR lpszDst, LPCTSTR lpcszSrc, int nCount)
{
	const int nPos = ::lstrlen(lpszDst);
	::lstrcpyn(lpszDst + nPos, lpcszSrc, nCount - nPos);
}

static unsigned char *readFile(LPCTSTR lpcszPath, UINT *puSize)
{
	unsigned char *pBuffer = 0;
	DWORD dwReadSize = 0;

	HANDLE hFile = INVALID_HANDLE_VALUE;
	if (lpcszPath)
	{
		hFile = CreateFile(lpcszPath, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	}
	if (hFile != INVALID_HANDLE_VALUE)
	{
		const DWORD dwSize = GetFileSize(hFile, NULL);
		if (dwSize)
		{
			pBuffer = new unsigned char [dwSize];
		}
		if (pBuffer)
		{
			if (!ReadFile(hFile, pBuffer, dwSize, &dwReadSize, NULL))
			{
				delete[] pBuffer;
				pBuffer = 0;
			}
		}
		CloseHandle(hFile);
	}
	if (puSize)
	{
		*puSize = dwReadSize;
	}
	return pBuffer;
}

static bool writeFile(LPCTSTR lpcszPath, const unsigned char *pcBuffer, UINT uSize)
{
	bool bResult = false;

	HANDLE hFile = INVALID_HANDLE_VALUE;
	if (lpcszPath)
	{
		hFile = CreateFile(lpcszPath, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	}
	if (hFile != INVALID_HANDLE_VALUE)
	{
		DWORD dwWrittenSize = 0;
		bResult = (WriteFile(hFile, pcBuffer, uSize, &dwWrittenSize, NULL)) && (dwWrittenSize == uSize);
		CloseHandle(hFile);
	}
	return bResult;
}

static UINT loadFile(LPCTSTR lpcszDir, LPCTSTR lpcszFile, LPVOID lpvBuffer, UINT uSize)
{
	TCHAR szPath[MAX_PATH];
	if (lpcszDir)
	{
		lstrcpyn(szPath, lpcszDir, MAX_PATH);
		lstrcatn(szPath, _T("\\"), MAX_PATH);
		lstrcatn(szPath, lpcszFile, MAX_PATH);
	}
	else
	{
		lstrcpyn(szPath, lpcszFile, MAX_PATH);
	}

	DWORD dwReadSize = 0;
	HANDLE hFile = CreateFile(szPath, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		ReadFile(hFile, lpvBuffer, uSize, &dwReadSize, NULL);
		CloseHandle(hFile);
	}
	else
	{
		_tprintf(_T("warning: not found %s\n"), lpcszDir);
	}
	return dwReadSize;
}

static int searchBinary(const unsigned char *pcBuffer, int nBufferSize, const unsigned char *pcKey, int nKeySize)
{
	if ((!pcBuffer) || (!pcKey) || (!nKeySize))
	{
		return -1;
	}

	const int nSearchLength = nBufferSize - nKeySize;
	for (int i=0; i<=nSearchLength; i++)
	{
		if (!memcmp(pcBuffer + i, pcKey, nKeySize))
		{
			return i;
		}
	}

	return -1;
}


// ----

#define	FONTX1_LR	0x10000
#define	FONTX1T_LR	0x20000

static const UINT8 s_jis2x1t[64] =
{
		0xff, 0xff, 0xff, 0xff,		/* 0x00 */
		0xff, 0xff, 0xff, 0xff,		/* 0x08 */
		0xff, 0xff, 0xff, 0xff,		/* 0x10 */
		0xff, 0xff, 0xff, 0xff,		/* 0x18 */
		0xff, 0x00, 0x02, 0x01,		/* 0x20 */
		0xff, 0xff, 0xff, 0xff,		/* 0x28 */
		0xff, 0x04, 0x06, 0x08,		/* 0x30 */
		0xff, 0x05, 0x07, 0x09,		/* 0x38 */

		0xff, 0x0a, 0x0c, 0x0e,		/* 0x40 */
		0xff, 0x0b, 0x0d, 0x0f,		/* 0x48 */
		0xff, 0x10, 0x12, 0x14,		/* 0x50 */
		0xff, 0x11, 0x13, 0x15,		/* 0x58 */
		0xff, 0x16, 0x18, 0x1a,		/* 0x60 */
		0xff, 0x17, 0x19, 0x1b,		/* 0x68 */
		0xff, 0x1c, 0x1e, 0x1d,		/* 0x70 */
		0xff, 0xff, 0xff, 0xff,		/* 0x78 */
};

__declspec(naked) static UINT __fastcall jis2sjis(UINT uJis)
{
	__asm
	{
				mov		eax, ecx
				and		eax, 7f7fh
				add		ah, 21h
				shr		ah, 1
				jnc		short jis2sjis1a
				add		al, 5eh
jis2sjis1a:		cmp		al, 60h
				sbb		al, 0e0h
				xor		ah, 0a0h
				ret
	}
}

static UINT8 *getFontAddrTurbo(tagRomImg *pImg, UINT uJis)
{
	const UINT uAddr = s_jis2x1t[((uJis >> 9) & 0x3c) + ((uJis >> 5) & 3)];
	if (uAddr & 0xc0)
	{
		return 0;
	}
	UINT8 *pRet = pImg->knjx1t;
	pRet += (uAddr << 12);
	pRet += ((uJis & 0x700) << 1);
	pRet += ((uJis & 0x01f) << 4);
	return pRet;
}

static void storeKnjTurbo(tagRomImg *pImg, const UINT8 *pcSrc, UINT uFrom, UINT uTo)
{
	for (UINT i=uFrom; i<uTo; i++)
	{
		const UINT h = i << 8;
		for (UINT l=0x21; l<0x7f; l++)
		{
			/* 漢字のポインタを求める */
			const UINT8 *p = 0;
			const UINT uSjis = jis2sjis(h + l);
			if ((uSjis >= 0x8140) && (uSjis < 0x84c0))
			{
				p = pcSrc + 0x00000 + ((uSjis - 0x8140) << 5);
			}
			else if ((uSjis >= 0x8890) && (uSjis < 0xa000))
			{
				p = pcSrc + 0x07000 + ((uSjis - 0x8890) << 5);
			}
			else if ((uSjis >= 0xe040) && (uSjis < 0xeab0))
			{
				p = pcSrc + 0x35e00 + ((uSjis - 0xe040) << 5);
			}
			if (p)								/* 規格内コードならば */
			{
				UINT8 *q = getFontAddrTurbo(pImg, h + l);
				/* コピーする */
				if (q)
				{
					for (UINT j=0; j<16; j++)
					{
						q[0] = p[0];
						q[FONTX1T_LR] = p[1];
						p += 2;
						q += 1;
					}
				}
			}
		}
	}
}

static void setMirrorTurbo(tagRomImg *pImg)
{
	CopyMemory(pImg->knjx1t + 0x03000, pImg->knjx1t + 0x02000, 0x1000);
	CopyMemory(pImg->knjx1t + 0x23000, pImg->knjx1t + 0x22000, 0x1000);
	CopyMemory(pImg->knjx1t + 0x1f000, pImg->knjx1t + 0x1e000, 0x1000);
	CopyMemory(pImg->knjx1t + 0x3f000, pImg->knjx1t + 0x3e000, 0x1000);
}

static void convertTurbo2Normal(tagRomImg *pImg)
{
	UINT8 *pDst = pImg->knjx1 + 0x0100;
	UINT i;
	for (i=0x2100; i<0x2800; i+=0x100)
	{
		for (UINT j=0x20; j<0x80; j+=0x20)
		{
			const UINT8 *pcSrc = getFontAddrTurbo(pImg, i + j);
			if (pcSrc)
			{
				CopyMemory(pDst, pcSrc, 0x200);
				CopyMemory(pDst + FONTX1_LR, pcSrc + FONTX1T_LR, 0x200);
			}
			pDst += 0x200;
		}
	}

	pDst = pImg->knjx1 + 0x4000;
	for (i=0x3000; i<0x5000; i+=0x100)
	{
		for (UINT j=0x20; j<0x80; j+=0x20)
		{
			const UINT8 *pcSrc = getFontAddrTurbo(pImg, i + j);
			if (pcSrc)
			{
				CopyMemory(pDst, pcSrc, 0x200);
				CopyMemory(pDst + FONTX1_LR, pcSrc + FONTX1T_LR, 0x200);
			}
			pDst += 0x200;
		}
	}
}

static void loadFont(tagRomImg *pImg, LPCTSTR lpcszDir, LPCTSTR lpcszFile)
{
	UINT8 sFont[306176];
	ZeroMemory(sFont, sizeof(sFont));
	loadFile(lpcszDir, lpcszFile, sFont, sizeof(sFont));

	storeKnjTurbo(pImg, sFont, 0x21, 0x50);
	storeKnjTurbo(pImg, sFont, 0x50, 0x78);
}


// ----

static bool storeRom(LPCTSTR lpcszSrc, LPCTSTR lpcszDst, LPCTSTR lpcszRomDir, LPCTSTR lpcszDisk1, LPCTSTR lpcszDisk2)
{
	unsigned char *pBuffer = 0;
	bool bRet = false;

	do
	{
		if (!lpcszDst)
		{
			lpcszDst = lpcszSrc;
		}

		UINT uSize = 0;
		pBuffer = readFile(lpcszSrc, &uSize);
		if (!pBuffer)
		{
			_tprintf(_T("error: couldn't open [%s]\n"), lpcszSrc);
			break;
		}

		// ストア位置をチェックする
		const int nPos = searchBinary(pBuffer, uSize, s_cExtRom, sizeof(s_cExtRom));
		if (nPos < 0)
		{
			_tprintf(_T("error: not found Rom Area\n"));
			break;
		}
		if ((static_cast<UINT>(nPos) + sizeof(tagRomImg)) > uSize)
		{
			_tprintf(_T("error: not found Rom Area (no left space)\n"));
			break;
		}


		// イメージを読む
		tagRomImg *pImg = reinterpret_cast<tagRomImg *>(pBuffer + nPos);
		loadFile(lpcszRomDir, _T("IPLROM.X1"), pImg->bios1, sizeof(pImg->bios1));
		loadFile(lpcszRomDir, _T("IPLROM.X1T"), pImg->bios2, sizeof(pImg->bios2));
		loadFile(lpcszRomDir, _T("FNT0808.X1"), pImg->ank, sizeof(pImg->ank));
		loadFile(lpcszRomDir, _T("FNT0816.X1"), pImg->txt, sizeof(pImg->txt));
		loadFont(pImg, lpcszRomDir, _T("FNT1616.X1"));

		setMirrorTurbo(pImg);
		convertTurbo2Normal(pImg);

		if (!lpcszDisk1)
		{
			lpcszDisk1 = _T("DISK1.D88");
		}
		if (!lpcszDisk2)
		{
			lpcszDisk2 = _T("DISK2.D88");
		}

		pImg->uDisk1Size = loadFile(lpcszRomDir, lpcszDisk1, pImg->sDisk1, sizeof(pImg->sDisk1));
		pImg->uDisk2Size = loadFile(lpcszRomDir, lpcszDisk2, pImg->sDisk2, sizeof(pImg->sDisk2));

		// 保存する
		bRet = writeFile(lpcszDst, pBuffer, uSize);
	} while(0 /*CONSTCOND*/);

	if (pBuffer)
	{
		delete[] pBuffer;
	}
	return bRet;
}



// ----

int _tmain(int argc, TCHAR **argv)
{
	if (argc < 2)
	{
		_tprintf(_T("usage: StoreRom [ELF-binary] -D [Rom-Dir]\n"));
		return 0;
	}

	LPCTSTR lpcszSrc = 0;
	LPCTSTR lpcszDst = 0;
	LPCTSTR lpcszDir = 0;
	LPCTSTR lpcszDisk1 = 0;
	LPCTSTR lpcszDisk2 = 0;
	LPCTSTR *lppcszStore = 0;

	for (int i=1; i<argc; i++)
	{
		LPCTSTR lpcsz = argv[i];
		if (lppcszStore)
		{
			*lppcszStore = lpcsz;
			lppcszStore = 0;
		}
		else if ((lpcsz[0] == '/') || (lpcsz[0] == '-'))
		{
			TCHAR cOpt = lpcsz[1];
			if ((cOpt >= 'A') && (cOpt <= 'Z'))
			{
				cOpt += 0x20;
			}
			switch(cOpt)
			{
				case 'd':
					lppcszStore = &lpcszDir;
					break;

				case '1':
					lppcszStore = &lpcszDisk1;
					break;

				case '2':
					lppcszStore = &lpcszDisk2;
					break;
			}
		}
		else if (!lpcszSrc)
		{
			lpcszSrc = lpcsz;
		}
		else if (!lpcszDst)
		{
			lpcszDst = lpcsz;
		}
	}

	storeRom(lpcszSrc, lpcszDst, lpcszDir, lpcszDisk1, lpcszDisk2);

	return 0;
}

