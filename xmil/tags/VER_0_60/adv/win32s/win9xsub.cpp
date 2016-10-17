#include	"compiler.h"
#include	"parts.h"
#include	"dosio.h"
#include	"font.h"
#include	"defipl.res"


const ROMIMG	__extromimage = {{'R','O','M','A','R','E','A'}};


// ---- bios

static const OEMCHAR iplx1[] = OEMTEXT("IPLROM.X1");
static const OEMCHAR iplx1t[] = OEMTEXT("IPLROM.X1T");

static void loadbios(ROMIMG *img) {

	FILEH	fh;

	CopyMemory(img->bios1, defaultiplrom, sizeof(defaultiplrom));
	CopyMemory(img->bios2, defaultiplrom, sizeof(defaultiplrom));
	fh = file_open_rb_c(iplx1);
	if (fh != FILEH_INVALID) {
		file_read(fh, img->bios1, 0x1000);
		file_close(fh);
	}
	fh = file_open_rb_c(iplx1t);
	if (fh != FILEH_INVALID) {
		file_read(fh, img->bios2, 0x8000);
		file_close(fh);
	}
}


// ---- font

static const OEMCHAR font0808[] = OEMTEXT("FNT0808.X1");
static const OEMCHAR font0816[] = OEMTEXT("FNT0816.X1");
static const OEMCHAR font1616[] = OEMTEXT("FNT1616.X1");

static const UINT8 jis2x1t[64] = {
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
					0xff, 0xff, 0xff, 0xff};	// 0x78

static UINT32 fadrsx1t(UINT jis) {

	UINT	badr;
	UINT32	ret;

	badr = jis2x1t[((jis >> 9) & 0x3c) + ((jis >> 5) & 3)];
	if (badr & 0xc0) {
		return((UINT32)-1);
	}
	ret = (badr << 12);
	ret += ((jis & 0x700) << 1);
	ret += ((jis & 0x01f) << 4);
	return(ret);
}

static void knjcpy(ROMIMG *img, const UINT8 *src, UINT from, UINT to) {

	UINT	i;
	UINT	h;
	UINT	l;
	UINT	sjis;
	UINT	j;
const UINT8	*p;
	UINT32	addr;
	UINT8	*q;

	for (i=from; i<to; i++) {
		h = i << 8;
		for (l=0x21; l<0x7f; l++) {
			sjis = jis2sjis(h + l);
			if ((sjis >= 0x8140) && (sjis < 0x84c0)) {
				p = src + 0x00000 + ((sjis - 0x8140) << 5);
			}
			else if ((sjis >= 0x8890) && (sjis < 0xa000)) {
				p = src + 0x07000 + ((sjis - 0x8890) << 5);
			}
			else if ((sjis >= 0xe040) && (sjis < 0xeab0)) {
				p = src + 0x35e00 + ((sjis - 0xe040) << 5);
			}
			else {
				p = NULL;
			}
			if (p) {
				addr = fadrsx1t(h + l);
				if (addr != (UINT32)-1) {
					// ƒRƒs[‚·‚é
					q = img->knjx1t + addr;
					for (j=0; j<16; j++) {
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

static void loadfont(ROMIMG *img) {

	FILEH	fh;
	UINT8	*work;

	fh = file_open_rb_c(font0808);
	if (fh != FILEH_INVALID) {
		file_read(fh, img->ank, 0x800);
		file_close(fh);
	}
	fh = file_open_rb_c(font0816);
	if (fh != FILEH_INVALID) {
		file_read(fh, img->txt, 0x1000);
		file_close(fh);
	}

	work = (UINT8 *)_MALLOC(306176, "loadfont");
	if (work == NULL) {
		goto lf_err1;
	}
	fh = file_open_rb_c(font1616);
	if (fh == FILEH_INVALID) {
		goto lf_err2;
	}
	file_read(fh, work, 306176);
	file_close(fh);
	knjcpy(img, work, 0x21, 0x50);
	knjcpy(img, work, 0x50, 0x78);

	CopyMemory(img->knjx1t + 0x03000, img->knjx1t + 0x02000, 0x1000);
	CopyMemory(img->knjx1t + 0x23000, img->knjx1t + 0x22000, 0x1000);
	CopyMemory(img->knjx1t + 0x1f000, img->knjx1t + 0x1e000, 0x1000);
	CopyMemory(img->knjx1t + 0x3f000, img->knjx1t + 0x3e000, 0x1000);
	cnvx1t2x1(img);

lf_err2:
	_MFREE(work);

lf_err1:
	return;
}


// ----

void win9xsub_initialize(void) {

	ROMIMG	*img;
	DWORD	oldprotect;

	img = (ROMIMG *)&__extromimage;
	VirtualProtect((void *)&__extromimage, sizeof(__extromimage),
												PAGE_WRITECOPY, &oldprotect);
	loadbios(img);
	loadfont(img);
	VirtualProtect((void *)&__extromimage, sizeof(__extromimage),
												oldprotect, NULL);
}

