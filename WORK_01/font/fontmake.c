#include	"compiler.h"
#include	"parts.h"
#include	"dosio.h"
#if defined(OSLANG_UTF8) || defined(OSLANG_UCS2)
#include	"oemtext.h"
#endif
#include	"fontmng.h"
#include	"font.h"
#include	"fontdata.h"
#include	"fontmake.h"


typedef struct {
	UINT16	jis1;
	UINT16	jis2;
} JISPAIR;

static const UINT8 deltable[] = {
		/*     del         del         del         del         del */
			0x0f, 0x5f, 0,
			0x01, 0x10, 0x1a, 0x21, 0x3b, 0x41, 0x5b, 0x5f, 0,
			0x54, 0x5f, 0,
			0x57, 0x5f, 0,
			0x19, 0x21, 0x39, 0x5f, 0,
			0x22, 0x31, 0x52, 0x5f, 0,
			0x01, 0x5f, 0,
			0x01, 0x5f, 0,
			0x01, 0x5f, 0,
			0x01, 0x5f, 0,
			0x01, 0x5f, 0,
			0x1f, 0x20, 0x37, 0x3f, 0x5d, 0x5f, 0};

static const JISPAIR jis7883[] = {
			{0x3646, 0x7421}, /* ãƒ:Íü */	{0x4b6a, 0x7422}, /* ñä:Í† */
			{0x4d5a, 0x7423}, /* óy:Í° */	{0x596a, 0x7424}, /* ùä:Í¢ */ };

static const JISPAIR jis8390[] = {
			{0x724d, 0x3033}, /* ÈÀ:à± */	{0x7274, 0x3229}, /* ÈÚ:âß */
			{0x695a, 0x3342}, /* Ây:äa */	{0x5978, 0x3349}, /* ùò:äh */
			{0x635e, 0x3376}, /* ‚}:äñ */	{0x5e75, 0x3443}, /* üÛ:ä¡ */
			{0x6b5d, 0x3452}, /* Ê|:ä– */	{0x7074, 0x375b}, /* ËÚ:åz */
			{0x6268, 0x395c}, /* ·Ê:ç{ */	{0x6922, 0x3c49}, /* ÂA:é« */
			{0x7057, 0x3f59}, /* Ë’:êx */	{0x6c4d, 0x4128}, /* ÊÀ:ëG */
			{0x5464, 0x445b}, /* ö‚:íŸ */	{0x626a, 0x4557}, /* ·Ë:ìv */
			{0x5b6d, 0x456e}, /* ûç:ìé */	{0x5e39, 0x4573}, /* ü∑:ìì */
			{0x6d6e, 0x4676}, /* Áé:ìÙ */	{0x6a24, 0x4768}, /* Â¢:îà */
			{0x5b58, 0x4930}, /* ûw:ïO */	{0x5056, 0x4b79}, /* ò‘:ñô */
			{0x692e, 0x4c79}, /* ÂM:ñ˜ */	{0x6446, 0x4f36}, /* ‚ƒ:òU */ };


static UINT cnvjis(UINT jis, const JISPAIR *tbl, UINT tblsize) {

const JISPAIR	*tblterm;

	tblterm = (JISPAIR *)(((UINT8 *)tbl) + tblsize);
	while(tbl < tblterm) {
		if (jis == tbl->jis1) {
			return(tbl->jis2);
		}
		else if (jis == tbl->jis2) {
			return(tbl->jis1);
		}
		tbl++;
	}
	return(jis);
}

static BRESULT isx1jis(UINT jis) {

const UINT8	*p;
	UINT	tmp;

	switch(jis >> 8) {
		case 0x22:
		case 0x23:
		case 0x24:
		case 0x25:
		case 0x26:
		case 0x27:
		case 0x28:
		case 0x29:
		case 0x2a:
		case 0x2b:
		case 0x2c:
		case 0x2d:
			p = deltable;
			tmp = (jis >> 8) - 0x22;
			while(tmp) {
				tmp--;
				while(*p++) { }
			}
			tmp = (jis & 0xff) - 0x20;
			while(*p) {
				if ((tmp >= (UINT)p[0]) && (tmp < (UINT)p[1])) {
					return(FALSE);
				}
				p += 2;
			}
			break;

		case 0x4f:
			tmp = jis & 0xff;
			if (tmp >= 0x54) {
				return(FALSE);
			}
			break;

		case 0x7c:
			tmp = jis & 0xff;
			if ((tmp == 0x6f) || (tmp == 0x70)) {
				return(FALSE);
			}
			break;

		case 0x2e:
		case 0x2f:
		case 0x74:
		case 0x75:
		case 0x76:
		case 0x77:
		case 0x78:
		case 0x7d:
		case 0x7e:
		case 0x7f:
			return(FALSE);
	}
	return(TRUE);
}

static void setank(UINT8 *ptr, void *fnt, UINT from, UINT to) {

	char	work[2];
	FNTDAT	dat;
const UINT8	*p;
	UINT8	*q;
	int		width;
	int		height;
	UINT8	bit;
	int		i;
#if defined(OSLANG_UTF8) || defined(OSLANG_UCS2)
	OEMCHAR	oemwork[4];
#endif

	ptr += from * 16;
	work[1] = '\0';
	while(from < to) {
		work[0] = (char)from;
#if defined(OSLANG_UTF8) || defined(OSLANG_UCS2)
		oemtext_sjis2oem(oemwork, NELEMENTS(oemwork), work, -1);
		dat = fontmng_get(fnt, oemwork);
#else
		dat = fontmng_get(fnt, work);
#endif
		if (dat) {
			width = min(dat->width, 8);
			height = min(dat->height, 16);
			p = (UINT8 *)(dat + 1);
			q = ptr;
			while(height > 0) {
				height--;
				bit = 0;
				for (i=0; i<width; i++) {
					if (p[i]) {
						bit |= (0x80 >> i);
					}
				}
				p += dat->width;
				*q++ = bit;
			}
		}
		from++;
		ptr += 16;
	}
}

static void setjis(UINT8 *ptr, void *fnt) {

	char	work[4];
	UINT	h;
	UINT	l;
	UINT	jis;
	UINT	sjis;
	UINT8	*q;
	FNTDAT	dat;
	int		width;
	int		height;
const UINT8	*p;
	UINT	bit;
	int		i;
#if defined(OSLANG_UTF8) || defined(OSLANG_UCS2)
	OEMCHAR	oemwork[4];
#endif

	work[2] = '\0';
	for (h=0x21; h<0x7f; h++) {
		for (l=0x21; l<0x7f; l++) {
			jis = (h << 8) + l;
			sjis = jis2sjis(jis);
			if ((sjis >= 0x8140) && (sjis < 0x84c0)) {
				q = ptr + 0x00000 + ((sjis - 0x8140) << 5);
			}
			else if ((sjis >= 0x8890) && (sjis < 0xa000)) {
				q = ptr + 0x07000 + ((sjis - 0x8890) << 5);
			}
			else if ((sjis >= 0xe040) && (sjis < 0xeab0)) {
				q = ptr + 0x35e00 + ((sjis - 0xe040) << 5);
			}
			else {
				q = NULL;
			}
			if ((q != NULL) && (isx1jis(jis))) {
				jis = cnvjis(jis, jis7883, sizeof(jis7883));
				jis = cnvjis(jis, jis8390, sizeof(jis8390));
				sjis = jis2sjis(jis);
				work[0] = (char)(sjis >> 8);
				work[1] = (char)sjis;
#if defined(OSLANG_UTF8) || defined(OSLANG_UCS2)
				oemtext_sjis2oem(oemwork, NELEMENTS(oemwork), work, -1);
				dat = fontmng_get(fnt, oemwork);
#else
				dat = fontmng_get(fnt, work);
#endif
				if (dat) {
					width = min(dat->width, 16);
					height = min(dat->height, 16);
					p = (UINT8 *)(dat + 1);
					while(height > 0) {
						height--;
						bit = 0;
						for (i=0; i<width; i++) {
							if (p[i]) {
								bit |= (0x8000 >> i);
							}
						}
						p += dat->width;
						q[0] = (UINT8)(bit >> 8);
						q[1] = (UINT8)bit;
						q += 2;
					}
				}
			}
		}
	}
}

void makex1font(REG8 loading) {

	void	*fnt;
	UINT8	*work;
	FILEH	fh;

	fnt = fontmng_create(16, 0, NULL);
	if (fnt == NULL) {
		goto mfnt_err1;
	}
	work = (UINT8 *)_MALLOC(306176, "MAKEFONT");
	if (work == NULL) {
		goto mfnt_err2;
	}
	if (loading & FONTLOAD_ANK) {
		ZeroMemory(work, 0x1000);
		setank(work, fnt, 0x20, 0x7f);
		setank(work, fnt, 0xa1, 0xe0);
		CopyMemory(work + 0x000, defrom_txt, 0x200);
		CopyMemory(work + 0x7f0, defrom_txt + 0x200, 0x210);
		CopyMemory(work + 0xe00, defrom_txt + 0x410, 0x200);
		fh = file_create_c(x1ank2tmp);
		if (fh != FILEH_INVALID) {
			file_write(fh, work, 0x1000);
			file_close(fh);
		}
	}
	if (loading & (FONT_KNJ1 | FONT_KNJ2)) {
		ZeroMemory(work, 306176);
		setjis(work, fnt);
		fh = file_create_c(x1knjtmp);
		if (fh != FILEH_INVALID) {
			file_write(fh, work, 306176);
			file_close(fh);
		}
	}
	_MFREE(work);

mfnt_err2:
	fontmng_destroy(fnt);

mfnt_err1:
	return;
}

