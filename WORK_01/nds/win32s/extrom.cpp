#include "compiler.h"
#include "parts.h"
#include "dosio.h"
#include "font.h"
#include "defipl.res"


	ROMIMG	__extromimage;


// ---- bios

static void loadbios(ROMIMG *pImg)
{
	CopyMemory(pImg->bios1, defaultiplrom, sizeof(defaultiplrom));
	CopyMemory(pImg->bios2, defaultiplrom, sizeof(defaultiplrom));

	static const TCHAR s_iplx1[] = _T("IPLROM.X1");
	FILEH fh = file_open_rb_c(s_iplx1);
	if (fh != FILEH_INVALID)
	{
		file_read(fh, pImg->bios1, 0x1000);
		file_close(fh);
	}

	static const TCHAR s_iplx1t[] = _T("IPLROM.X1T");
	fh = file_open_rb_c(s_iplx1t);
	if (fh != FILEH_INVALID)
	{
		file_read(fh, pImg->bios2, 0x8000);
		file_close(fh);
	}
}


// ---- font


static UINT32 fadrsx1t(UINT uJis)
{
	static const UINT8 s_jis2x1t[64] =
	{
			0xff, 0xff, 0xff, 0xff,		// 0x00
			0xff, 0xff, 0xff, 0xff,		// 0x08
			0xff, 0xff, 0xff, 0xff,		// 0x10
			0xff, 0xff, 0xff, 0xff,		// 0x18
			0xff, 0x00, 0x02, 0x01,		// 0x20
			0xff, 0xff, 0xff, 0xff,		// 0x28
			0xff, 0x04, 0x06, 0x08,		// 0x30
			0xff, 0x05, 0x07, 0x09,		// 0x38

			0xff, 0x0a, 0x0c, 0x0e,		// 0x40
			0xff, 0x0b, 0x0d, 0x0f,		// 0x48
			0xff, 0x10, 0x12, 0x14,		// 0x50
			0xff, 0x11, 0x13, 0x15,		// 0x58
			0xff, 0x16, 0x18, 0x1a,		// 0x60
			0xff, 0x17, 0x19, 0x1b,		// 0x68
			0xff, 0x1c, 0x1e, 0x1d,		// 0x70
			0xff, 0xff, 0xff, 0xff,		// 0x78
	};
	const UINT uAddr = s_jis2x1t[((uJis >> 9) & 0x3c) + ((uJis >> 5) & 3)];
	if (uAddr & 0xc0)
	{
		return static_cast<UINT32>(-1);
	}
	UINT32 uRet = (uAddr << 12);
	uRet += ((uJis & 0x700) << 1);
	uRet += ((uJis & 0x01f) << 4);
	return uRet;
}

static void knjcpy(ROMIMG *pImg, const UINT8 *pcSrc, UINT uFrom, UINT uTo)
{
	for (UINT i=uFrom; i<uTo; i++)
	{
		const UINT h = i << 8;
		for (UINT l=0x21; l<0x7f; l++)
		{
			const UINT uSjis = jis2sjis(h + l);
			const UINT8 *p = 0;
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
			if (p)
			{
				UINT32 dwAddr = fadrsx1t(h + l);
				if (dwAddr != static_cast<UINT32>(-1))
				{
					// ƒRƒs[‚·‚é
					UINT8 *q = pImg->knjx1t + dwAddr;
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

static void cnvx1t2x1(ROMIMG *img) {

	UINT	i;
	UINT	j;
	UINT32	addr;
const UINT8	*src;
	UINT8	*dst;

	dst = img->knjx1 + 0x0100;
	for (i=0x2100; i<0x2800; i+=0x100) {
		for (j=0x20; j<0x80; j+=0x20) {
			addr = fadrsx1t(i + j);
			if (addr != (UINT32)-1) {
				src = img->knjx1t + addr;
				CopyMemory(dst, src, 0x200);
				CopyMemory(dst + FONTX1_LR, src + FONTX1T_LR, 0x200);
			}
			dst += 0x200;
		}
	}
	dst = img->knjx1 + 0x4000;
	for (i=0x3000; i<0x5000; i+=0x100) {
		for (j=0x20; j<0x80; j+=0x20) {
			addr = fadrsx1t(i + j);
			if (addr != (UINT32)-1) {
				src = img->knjx1t + addr;
				CopyMemory(dst, src, 0x200);
				CopyMemory(dst + FONTX1_LR, src + FONTX1T_LR, 0x200);
			}
			dst += 0x200;
		}
	}
}

static void loadfont(ROMIMG *pImg)
{
	static const TCHAR s_font0808[] = _T("FNT0808.X1");
	FILEH fh = file_open_rb_c(s_font0808);
	if (fh != FILEH_INVALID)
	{
		file_read(fh, pImg->ank, 0x800);
		file_close(fh);
	}

	static const TCHAR s_font0816[] = _T("FNT0816.X1");
	fh = file_open_rb_c(s_font0816);
	if (fh != FILEH_INVALID)
	{
		file_read(fh, pImg->txt, 0x1000);
		file_close(fh);
	}

	UINT8 *pWork = new UINT8 [306176];
	if (pWork)
	{
		static const OEMCHAR s_font1616[] = OEMTEXT("FNT1616.X1");
		fh = file_open_rb_c(s_font1616);
		if (fh != FILEH_INVALID)
		{
			file_read(fh, pWork, 306176);
			file_close(fh);
			knjcpy(pImg, pWork, 0x21, 0x50);
			knjcpy(pImg, pWork, 0x50, 0x78);

			CopyMemory(pImg->knjx1t + 0x03000, pImg->knjx1t + 0x02000, 0x1000);
			CopyMemory(pImg->knjx1t + 0x23000, pImg->knjx1t + 0x22000, 0x1000);
			CopyMemory(pImg->knjx1t + 0x1f000, pImg->knjx1t + 0x1e000, 0x1000);
			CopyMemory(pImg->knjx1t + 0x3f000, pImg->knjx1t + 0x3e000, 0x1000);
			cnvx1t2x1(pImg);
		}
		delete[] pWork;
	}
}


// ----

void extrom_initialize()
{
	loadbios(&__extromimage);
	loadfont(&__extromimage);
}

