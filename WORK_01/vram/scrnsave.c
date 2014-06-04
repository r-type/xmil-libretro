#include	"compiler.h"
#include	"bmpdata.h"
#include	"dosio.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"scrndraw.h"
#include	"palettes.h"
#include	"makescrn.h"
#include	"scrnsave.h"


#if defined(SUPPORT_TURBOZ)
typedef	unsigned short	PALNUM;
#else
typedef	unsigned char	PALNUM;
#endif

typedef union {
	UINT32	d;
	UINT8	rgb[4];
} BMPPAL;

typedef struct {
	int		width;
	int		height;
	UINT	pals;
	UINT	type;
	BMPPAL	pal[XMILPAL_MAX];
	PALNUM	dat[SURFACE_WIDTH * SURFACE_HEIGHT];
} SCRNDATA;


static void screenmix32s(PALNUM *dst, const UINT8 *src) {

	int		y;
	int		x;
	REG8	c;

	for (y=0; y<(SURFACE_HEIGHT/2); y++) {
		for (x=0; x<(SURFACE_WIDTH/2); x++) {
			c = src[x];
			dst[x*2+0] = (PALNUM)c;
			dst[x*2+1] = (PALNUM)c;
			dst[x*2+SURFACE_WIDTH+0] = (PALNUM)(c + 0x40);
			dst[x*2+SURFACE_WIDTH+1] = (PALNUM)(c + 0x40);
		}
		dst += SURFACE_WIDTH * 2;
		src += SURFACE_WIDTH * 2;
	}
}

static void screenmix62s(PALNUM *dst, const UINT8 *src) {

	int		y;
	int		x;
	REG8	c;

	for (y=0; y<(SURFACE_HEIGHT/2); y++) {
		for (x=0; x<SURFACE_WIDTH; x++) {
			c = src[x];
			dst[x] = (PALNUM)c;
			dst[x+SURFACE_WIDTH] = (PALNUM)(c + 0x40);
		}
		dst += SURFACE_WIDTH * 2;
		src += SURFACE_WIDTH * 2;
	}
}

static void screenmix32h(PALNUM *dst, const UINT8 *src) {

	int		y;
	int		x;
	REG8	c;

	for (y=0; y<(SURFACE_HEIGHT/2); y++) {
		for (x=0; x<(SURFACE_WIDTH/2); x++) {
			c = src[x];
			dst[x*2+0] = (PALNUM)c;
			dst[x*2+1] = (PALNUM)c;
			dst[x*2+SURFACE_WIDTH+0] = (PALNUM)c;
			dst[x*2+SURFACE_WIDTH+1] = (PALNUM)c;
		}
		dst += SURFACE_WIDTH * 2;
		src += SURFACE_WIDTH * 2;
	}
}

static void screenmix62h(PALNUM *dst, const UINT8 *src) {

	int		y;
	int		x;
	REG8	c;

	for (y=0; y<(SURFACE_HEIGHT/2); y++) {
		for (x=0; x<SURFACE_WIDTH; x++) {
			c = src[x];
			dst[x] = (PALNUM)c;
			dst[x+SURFACE_WIDTH] = (PALNUM)c;
		}
		dst += SURFACE_WIDTH * 2;
		src += SURFACE_WIDTH * 2;
	}
}

static void screenmix34(PALNUM *dst, const UINT8 *src) {

	int		y;
	int		x;
	REG8	c;

	for (y=0; y<SURFACE_HEIGHT; y++) {
		for (x=0; x<(SURFACE_WIDTH/2); x++) {
			c = src[x];
			dst[x*2+0] = (PALNUM)c;
			dst[x*2+1] = (PALNUM)c;
		}
		dst += SURFACE_WIDTH;
		src += SURFACE_WIDTH;
	}
}

static void screenmix64(PALNUM *dst, const UINT8 *src) {

	int		i;

	for (i=0; i<(SURFACE_WIDTH * SURFACE_HEIGHT); i++) {
		dst[i] = src[i];
	}
}

#if defined(SUPPORT_TURBOZ)
static void screenmixz(PALNUM *dst, const UINT8 *src) {

	int		y;
	int		x;
	PALNUM	c;

	for (y=0; y<SURFACE_HEIGHT; y++) {
		for (x=0; x<(SURFACE_WIDTH/2); x++) {
			c = ((src[x+320] << 8) | src[x]) + XMILPAL_4096G;
			dst[x*2+0] = c;
			dst[x*2+1] = c;
		}
		dst += SURFACE_WIDTH;
		src += SURFACE_WIDTH;
	}
}
#endif

typedef void (*SCMIX)(PALNUM *dst, const UINT8 *src);

static const SCMIX scmix[] = {
			screenmix32s,	screenmix62s,
			screenmix32h,	screenmix62h,
			screenmix34,	screenmix64,
#if defined(SUPPORT_TURBOZ)
			screenmixz,
#endif
};


// ----

SCRNSAVE scrnsave_get(void) {

	int			width;
	int			height;
	SCRNDATA	*sd;
	PALNUM		*dat;
	UINT		scrnsize;
	PALNUM		*s;
	UINT		pals;
	PALNUM		remap[XMILPAL_MAX];
	UINT8		remapflag[XMILPAL_MAX];
	int			x;
	int			y;
	PALNUM		col;
	BMPPAL		curpal;
	UINT		pos;

	width = SURFACE_WIDTH;
	height = SURFACE_HEIGHT;
	if ((width <= 0) || (height <= 0)) {
		goto ssg_err;
	}
	sd = (SCRNDATA *)_MALLOC(sizeof(SCRNDATA), "screen data");
	if (sd == NULL) {
		goto ssg_err;
	}
	ZeroMemory(sd, sizeof(SCRNDATA));

	dat = sd->dat;
	scrnsize = SURFACE_WIDTH * SURFACE_HEIGHT;
	scmix[makescrn.drawmode](sd->dat, screenmap);

	// パレット最適化
	s = sd->dat;
	pals = 0;
	ZeroMemory(remap, sizeof(remap));
	ZeroMemory(remapflag, sizeof(remapflag));
	for (y=0; y<height; y++) {
		for (x=0; x<width; x++) {
			col = s[x];
			if (!remapflag[col]) {
				remapflag[col] = 1;
				curpal.rgb[0] = xmil_pal32[col].p.b;
				curpal.rgb[1] = xmil_pal32[col].p.g;
				curpal.rgb[2] = xmil_pal32[col].p.r;
				for (pos=0; pos<pals; pos++) {
					if (sd->pal[pos].d == curpal.d) {
						break;
					}
				}
				if (pos >= pals) {
					sd->pal[pos].d = curpal.d;
					pals++;
				}
				remap[col] = (PALNUM)pos;
			}
			s[x] = remap[col];
		}
		s += SURFACE_WIDTH;
	}
	sd->width = width;
	sd->height = height;
	sd->pals = pals;
	if (pals < 2) {
		sd->type = SCRNSAVE_1BIT;
	}
	else if (pals < 16) {
		sd->type = SCRNSAVE_4BIT;
	}
	else if (pals < 256) {
		sd->type = SCRNSAVE_8BIT;
	}
	else {
		sd->type = SCRNSAVE_24BIT;
	}
	return((SCRNSAVE)sd);

ssg_err:
	return(NULL);
}

void scrnsave_trash(SCRNSAVE hdl) {

	if (hdl) {
		_MFREE(hdl);
	}
}


// ---- BMP

BRESULT scrnsave_writebmp(SCRNSAVE hdl, const OEMCHAR *filename, UINT flag) {

const SCRNDATA	*sd;
	FILEH		fh;
	BMPDATA		bd;
	UINT		type;
	UINT		palsize;
	BMPFILE		bf;
	UINT		pos;
	BMPINFO		bi;
	UINT8		palwork[1024];
	UINT		align;
	UINT8		*work;
const PALNUM	*s;
	int			r;
	int			x;
	BMPPAL		curpal;

	(void)flag;

	if (hdl == NULL) {
		goto sswb_err1;
	}
	sd = (SCRNDATA *)hdl;

	fh = file_create(filename);
	if (fh == FILEH_INVALID) {
		goto sswb_err1;
	}

	bd.width = sd->width;
	bd.height = sd->height;
	if (sd->pals <= 2) {
		type = SCRNSAVE_1BIT;
		bd.bpp = 1;
		palsize = 4 << 1;
	}
	else if (sd->pals <= 16) {
		type = SCRNSAVE_4BIT;
		bd.bpp = 4;
		palsize = 4 << 4;
	}
	else if (sd->pals <= 256) {
		type = SCRNSAVE_8BIT;
		bd.bpp = 8;
		palsize = 4 << 8;
	}
	else {
		type = SCRNSAVE_24BIT;
		bd.bpp = 24;
		palsize = 0;
	}

	// Bitmap File
	ZeroMemory(&bf, sizeof(bf));
	bf.bfType[0] = 'B';
	bf.bfType[1] = 'M';
	pos = sizeof(BMPFILE) + sizeof(BMPINFO) + palsize;
	STOREINTELDWORD(bf.bfOffBits, pos);
	if (file_write(fh, &bf, sizeof(bf)) != sizeof(bf)) {
		goto sswb_err2;
	}

	// Bitmap Info
	bmpdata_setinfo(&bi, &bd);
	STOREINTELDWORD(bi.biClrImportant, sd->pals);
	align = bmpdata_getalign(&bi);
	if (file_write(fh, &bi, sizeof(bi)) != sizeof(bi)) {
		goto sswb_err2;
	}
	if (palsize) {
		ZeroMemory(palwork, palsize);
		CopyMemory(palwork, sd->pal, sd->pals * 4);
		if (file_write(fh, palwork, palsize) != palsize) {
			goto sswb_err2;
		}
	}

	work = (UINT8 *)_MALLOC(align, filename);
	if (work == NULL) {
		goto sswb_err2;
	}
	ZeroMemory(work, align);

	s = sd->dat + (SURFACE_WIDTH * bd.height);
	do {
		s -= SURFACE_WIDTH;
		switch(type) {
			case SCRNSAVE_1BIT:
				ZeroMemory(work, align);
				for (x=0; x<bd.width; x++) {
					if (s[x]) {
						work[x >> 3] |= 0x80 >> (x & 7);
					}
				}
				break;

			case SCRNSAVE_4BIT:
				r = bd.width / 2;
				for (x=0; x<r; x++) {
					work[x] = (s[x*2+0] << 4) + s[x*2+1];
				}
				if (bd.width & 1) {
					work[x] = s[x*2+0] << 4;
				}
				break;

			case SCRNSAVE_8BIT:
				for (x=0; x<bd.width; x++) {
					work[x] = (UINT8)s[x];
				}
				break;

			case SCRNSAVE_24BIT:
				for (x=0; x<bd.width; x++) {
					curpal.d = sd->pal[s[x]].d;
					work[x*3+0] = curpal.rgb[0];
					work[x*3+1] = curpal.rgb[1];
					work[x*3+2] = curpal.rgb[2];
				}
				break;
		}
		if (file_write(fh, work, align) != align) {
			goto sswb_err3;
		}
	} while(--bd.height);

	file_close(fh);
	_MFREE(work);
	return(SUCCESS);

sswb_err3:
	_MFREE(work);

sswb_err2:
	file_close(fh);
	file_delete(filename);

sswb_err1:
	return(FAILURE);
}


// ---- GIF

#if 1
#define	MAXGIFBITS			12

#if MAXGIFBITS == 12
#define	HASHTBLSIZE			5003
#elif MAXGIFBITS == 13
#define	HASHTBLSIZE			9001
#elif MAXGIFBITS == 14
#define	HASHTBLSIZE			18013
#elif MAXGIFBITS == 15
#define	HASHTBLSIZE			35023
#elif MAXGIFBITS == 16
#define	HASHTBLSIZE			69001
#endif

#define GIFBITDATAWRITE(dat) 											\
	do {																\
		bitdata |= (dat) << bits;										\
		bits += bitcount;												\
		while(bits >= 8) {												\
			bitbuf[++bitdatas] = (UINT8)bitdata;						\
			if (bitdatas >= 255) {										\
				bitbuf[0] = (UINT8)bitdatas;							\
				r = 1 + bitdatas;										\
				if (file_write(fh, bitbuf, r) != r) {					\
					goto sswg_err4;										\
				}														\
				bitdatas = 0;											\
			}															\
			bitdata >>= 8;												\
			bits -= 8;													\
		}																\
	} while(/*CONSTCOND*/ 0)

#define GIFBITEXTENSION													\
	do {																\
		if (codefree > codemax) {										\
			bitcount++;													\
			if (bitcount < MAXGIFBITS) {								\
				codemax = (codemax << 1) + 1;							\
			}															\
			else {														\
				codemax = 1 << MAXGIFBITS;								\
			}															\
		}																\
	} while(/*CONSTCOND*/ 0)

#define GIFBITDATAFLASH													\
	do {																\
		if (bits) {														\
			bitbuf[++bitdatas] = (UINT8)bitdata;						\
		}																\
		if (bitdatas) {													\
			bitbuf[0] = (UINT8)bitdatas;								\
			r = 1 + bitdatas;											\
			if (file_write(fh, bitbuf, r) != r) {						\
				goto sswg_err4;											\
			}															\
		}																\
	} while(/*CONSTCOND*/ 0)


BRESULT scrnsave_writegif(SCRNSAVE hdl, const OEMCHAR *filename, UINT flag) {

const SCRNDATA	*sd;
	UINT		bpp;
	UINT		*hash_code;
	UINT32		*hash_data;
	FILEH		fh;
	UINT		r;
const PALNUM	*s;

	UINT		codeclear;
	UINT		codeeoi;
	UINT		codefree;
	UINT		codemax;

	UINT8		bits;
	UINT8		bitcount;
	UINT		bitdata;
	UINT		bitdatas;
	UINT8		bitbuf[3+256*3];

	int			x;
	int			y;
	UINT		b;
	UINT32		c;
	int			i;
	int			disp;

	(void)flag;

	if (hdl == NULL) {
		goto sswg_err1;
	}
	sd = (SCRNDATA *)hdl;

	bpp = 1;
	while(sd->pals > (UINT)(1 << bpp)) {
		bpp++;
	}
	if (bpp > 8) {
		goto sswg_err1;
	}

	hash_code = (UINT *)_MALLOC(HASHTBLSIZE * sizeof(UINT), "hash_code");
	if (hash_code == NULL) {
		goto sswg_err1;
	}
	hash_data = (UINT32 *)_MALLOC(HASHTBLSIZE * sizeof(UINT32), "hash_data");
	if (hash_data == NULL) {
		goto sswg_err2;
	}

	fh = file_create(filename);
	if (fh == FILEH_INVALID) {
		goto sswg_err3;
	}

	CopyMemory(bitbuf, "GIF87a", 6);
	STOREINTELWORD(bitbuf + 6, sd->width);
	STOREINTELWORD(bitbuf + 8, sd->height);
	if (file_write(fh, bitbuf, 10) != 10) {
		goto sswg_err4;
	}

	ZeroMemory(bitbuf, sizeof(bitbuf));
	bitbuf[0] = (UINT8)(0x80 + ((8 - 1) << 4) + (bpp - 1));
//	bitbuf[1] = 0;									// background
//	bitbuf[2] = 0;									// reserved
	for (r=0; r<sd->pals; r++) {
		bitbuf[r*3+3] = sd->pal[r].rgb[2];			// R
		bitbuf[r*3+4] = sd->pal[r].rgb[1];			// G
		bitbuf[r*3+5] = sd->pal[r].rgb[0];			// B
	}
	r = (1 << bpp) * 3 + 3;
	if (file_write(fh, bitbuf, r) != r) {
		goto sswg_err4;
	}

	bitbuf[0] = 0x2c;							// separator
	STOREINTELWORD(bitbuf + 1, 0);				// sx
	STOREINTELWORD(bitbuf + 3, 0);				// sy
	STOREINTELWORD(bitbuf + 5, sd->width);		// cx
	STOREINTELWORD(bitbuf + 7, sd->height);		// cy
	bitbuf[9] = 0;								// noninterlace

	bpp = max(bpp, 2);
	bitbuf[10] = (UINT8)bpp;
	if (file_write(fh, bitbuf, 11) != 11) {
		goto sswg_err4;
	}

	codeclear = 1 << bpp;
	codeeoi = codeclear + 1;
	codefree = codeclear + 2;
	codemax = (codeclear << 1) - 1;

	bits = 0;
	bitdata = 0;
	bitdatas = 0;
	bitcount = (UINT8)(bpp + 1);
	GIFBITDATAWRITE(codeclear);

	ZeroMemory(hash_code, HASHTBLSIZE * sizeof(UINT));

	x = 0;
	y = 0;
	s = sd->dat;
	b = s[x++];
	do {
		while(x < sd->width) {
			c = s[x++];
			i = (c << (MAXGIFBITS - 8)) + b;
			c = (c << 16) + b;
			if (i >= HASHTBLSIZE) {
				i -= HASHTBLSIZE;
			}
			disp = (i != 0)?(i - HASHTBLSIZE):-1;
			while(1) {
				if (hash_code[i] == 0) {
					GIFBITDATAWRITE(b);
					GIFBITEXTENSION;
					if (codefree < (1 << MAXGIFBITS)) {
						hash_code[i] = codefree++;
						hash_data[i] = c;
					}
					else {
						ZeroMemory(hash_code, HASHTBLSIZE * sizeof(UINT));
						GIFBITDATAWRITE(codeclear);
						codefree = codeclear + 2;
						codemax = (codeclear << 1) - 1;
						bitcount = (UINT8)(bpp + 1);
					}
					b = c >> 16;
					break;
				}
				else if (hash_data[i] == c) {
					b = hash_code[i];
					break;
				}
				else {
					i += disp;
					if (i < 0) {
						i += HASHTBLSIZE;
					}
				}
			}
		}
		x = 0;
		s += SURFACE_WIDTH;
		y++;
	} while(y < sd->height);

	GIFBITDATAWRITE(b);
	GIFBITEXTENSION;
	GIFBITDATAWRITE(codeeoi);
	GIFBITDATAFLASH;

	bitbuf[0] = 0;
	if (file_write(fh, bitbuf, 1) != 1) {
		goto sswg_err4;
	}

	bitbuf[0] = 0x3b;								// terminator
	if (file_write(fh, bitbuf, 1) != 1) {
		goto sswg_err4;
	}

	file_close(fh);
	_MFREE(hash_data);
	_MFREE(hash_code);
	return(SUCCESS);

sswg_err4:
	file_close(fh);
	file_delete(filename);

sswg_err3:
	_MFREE(hash_data);

sswg_err2:
	_MFREE(hash_code);

sswg_err1:
	return(FAILURE);
}
#endif

