#include	"compiler.h"
#include	"strres.h"
#include	"dosio.h"
#include	"profile.h"
#if defined(OSLANG_UCS2)
#include	"ucscnv.h"
#endif


typedef struct {
	UINT	applen;
	UINT	keylen;
	UINT	pos;
	UINT	size;
	UINT	apphit;
const char	*data;
	UINT	datasize;
} PFPOS;


static const UINT8 utf8header[3] = {0xef, 0xbb, 0xbf};

static BRESULT keycmp(const char *str, const char *cmp) {

	int		s;
	int		c;

	do {
		c = *cmp++;
		if (c == 0) {
			return(SUCCESS);
		}
		if ((c >= 'a') && (c <= 'z')) {
			c -= 0x20;
		}
		s = *str++;
		if ((s >= 'a') && (s <= 'z')) {
			s -= 0x20;
		}
	} while(s == c);
	return(FAILURE);
}

static char *delspace(const char *buf, UINT *len) {

	UINT	l;

	if ((buf != NULL) && (len != NULL)) {
		l = *len;
		while((l) && (buf[0] == ' ')) {
			l--;
			buf++;
		}
		while((l) && (buf[l - 1] == ' ')) {
			l--;
		}
		*len = l;
	}
	return((char *)buf);
}

static BRESULT seakey(PFILEH hdl, PFPOS *pfp,
										const char *app, const char *key) {

	PFPOS	ret;
	UINT	pos;
	UINT	size;
	char	*buf;
	UINT	len;
	UINT	cnt;

	if ((hdl == NULL) || (app == NULL) || (key == NULL)) {
		return(FAILURE);
	}
	ZeroMemory(&ret, sizeof(ret));
	ret.applen = STRLEN(app);
	ret.keylen = STRLEN(key);
	if ((ret.applen == 0) || (ret.keylen == 0)) {
		return(FAILURE);
	}

	pos = 0;
	size = hdl->size;
	while(size) {
		buf = hdl->buffer + pos;
		len = 0;
		cnt = 0;
		do {
			if (buf[len] == '\r') {
				if (((len + 1) < size) && (buf[len + 1] == '\n')) {
					cnt = 2;
				}
				else {
					cnt = 1;
				}
				break;
			}
			if (buf[len] == '\n') {
				cnt = 1;
				break;
			}
			len++;
		} while(len < size);
		cnt += len;
		buf = delspace(buf, &len);
		if ((len >= 2) && (buf[0] == '[') && (buf[len - 1] == ']')) {
			if (ret.apphit) {
				break;
			}
			buf++;
			len -= 2;
			buf = delspace(buf, &len);
			if ((len == ret.applen) && (keycmp(buf, app) == SUCCESS)) {
				ret.apphit = 1;
			}
		}
		else if ((ret.apphit) && (len > ret.keylen) &&
				(keycmp(buf, key) == SUCCESS)) {
			buf += ret.keylen;
			len -= ret.keylen;
			buf = delspace(buf, &len);
			if ((len) && (buf[0] == '=')) {
				buf++;
				len--;
				buf = delspace(buf, &len);
				ret.pos = pos;
				ret.size = cnt;
				ret.data = buf;
				ret.datasize = len;
				break;
			}
		}
		if (len) {
			ret.pos = pos + cnt;
			ret.size = 0;
		}
		pos += cnt;
		size -= cnt;
	}
	if (pfp) {
		*pfp = ret;
	}
	return(SUCCESS);
}

static BRESULT replace(PFILEH hdl, UINT pos, UINT size1, UINT size2) {

	UINT	cnt;
	UINT	size;
	char	*p;
	char	*q;

	size1 += pos;
	size2 += pos;
	if (size1 > hdl->size) {
		return(FAILURE);
	}
	cnt = hdl->size - size1;
	if (size1 < size2) {
		size = size2 - size1;
		if ((hdl->size + size) > hdl->buffers) {
			return(FAILURE);
		}
		hdl->size += size;
		if (cnt) {
			p = hdl->buffer + size1;
			q = hdl->buffer + size2;
			do {
				--cnt;
				q[cnt] = p[cnt];
			} while(cnt);
		}
	}
	else if (size1 > size2) {
		hdl->size -= (size1 - size2);
		if (cnt) {
			p = hdl->buffer + size1;
			q = hdl->buffer + size2;
			do {
				*q++ = *p++;
			} while(--cnt);
		}
	}
	hdl->flag |= PFILEH_MODIFY;
	return(SUCCESS);
}

PFILEH profile_open(const OEMCHAR *filename, UINT flag) {

	FILEH	fh;
	UINT	filesize;
	UINT	size;
	PFILEH	ret;
	char	*ptr;

	if (filename == NULL) {
		goto pfore_err1;
	}
	fh = file_open(filename);
	filesize = 0;
	if (fh != FILEH_INVALID) {
		filesize = file_getsize(fh);
	}
	else if (flag & PFILEH_READONLY) {
		goto pfore_err1;
	}
	else {
#if defined(OSLANG_UTF8) || defined(OSLANG_UCS2)
		flag |= PFILEH_UTF8;
#endif
	}
	size = filesize + 0x2000;
	ret = (PFILEH)_MALLOC(sizeof(_PFILEH) + size, filename);
	if (ret == NULL) {
		goto pfore_err2;
	}
	if (fh != FILEH_INVALID) {
		if (file_read(fh, ret + 1, filesize) != filesize) {
			goto pfore_err3;
		}
		file_close(fh);
	}
	ptr = (char *)(ret + 1);
	if ((filesize >= 3) && (!memcmp(ptr, utf8header, 3))) {
		ptr += 3;
		size -= 3;
		filesize -= 3;
		flag |= PFILEH_UTF8;
	}
	ret->buffer = ptr;
	ret->buffers = size;
	ret->size = filesize;
	ret->flag = flag;
	file_cpyname(ret->path, filename, NELEMENTS(ret->path));
	return(ret);

pfore_err3:
	_MFREE(ret);

pfore_err2:
	if (fh != FILEH_INVALID) {
		file_close(fh);
	}

pfore_err1:
	return(NULL);
}

void profile_close(PFILEH hdl) {

	FILEH	fh;

	if (hdl) {
		if (hdl->flag & PFILEH_MODIFY) {
			fh = file_create(hdl->path);
			if (fh != FILEH_INVALID) {
				if (hdl->flag & PFILEH_UTF8) {
					file_write(fh, utf8header, 3);
				}
				file_write(fh, hdl->buffer, hdl->size);
				file_close(fh);
			}
		}
		_MFREE(hdl);
	}
}

BRESULT profile_read(const char *app, const char *key, const OEMCHAR *def,
										OEMCHAR *ret, UINT size, PFILEH hdl) {

	PFPOS	pfp;

	if ((seakey(hdl, &pfp, app, key) != SUCCESS) || (pfp.data == NULL)) {
		if (def == NULL) {
			def = str_null;
		}
		milstr_ncpy(ret, def, size);
		return(FAILURE);
	}
	else {
#if defined(OSLANG_UCS2)
		ucscnv_utf8toucs2(ret, size, pfp.data, pfp.datasize);
#else
		size = min(size, pfp.datasize + 1);
		milstr_ncpy(ret, pfp.data, size);
#endif
		return(SUCCESS);
	}
}

BRESULT profile_write(const char *app, const char *key,
											const OEMCHAR *data, PFILEH hdl) {

	PFPOS	pfp;
	UINT	newsize;
	UINT	datalen;
	char	*buf;

	if ((hdl == NULL) || (hdl->flag & PFILEH_READONLY) ||
		(data == NULL) || (seakey(hdl, &pfp, app, key) != SUCCESS)) {
		return(FAILURE);
	}
	if (!pfp.apphit) {
		newsize = pfp.applen + 2;
#if defined(OSLINEBREAK_CR) || defined(OSLINEBREAK_CRLF)
		newsize++;
#endif
#if defined(OSLINEBREAK_LF) || defined(OSLINEBREAK_CRLF)
		newsize++;
#endif
		if (replace(hdl, pfp.pos, 0, newsize) != SUCCESS) {
			return(FAILURE);
		}
		buf = hdl->buffer + pfp.pos;
		*buf++ = '[';
		CopyMemory(buf, app, pfp.applen);
		buf += pfp.applen;
		*buf++ = ']';
#if defined(OSLINEBREAK_CR) || defined(OSLINEBREAK_CRLF)
		*buf++ = '\r';
#endif
#if defined(OSLINEBREAK_LF) || defined(OSLINEBREAK_CRLF)
		*buf++ = '\n';
#endif
		pfp.pos += newsize;
	}
#if defined(OSLANG_UCS2)
	datalen = ucscnv_ucs2toutf8(NULL, (UINT)-1, data, (UINT)-1);
#else
	datalen = STRLEN(data);
#endif
	newsize = pfp.keylen + 1 + datalen;
#if defined(OSLINEBREAK_CR) || defined(OSLINEBREAK_CRLF)
	newsize++;
#endif
#if defined(OSLINEBREAK_LF) || defined(OSLINEBREAK_CRLF)
	newsize++;
#endif
	if (replace(hdl, pfp.pos, pfp.size, newsize) != SUCCESS) {
		return(FAILURE);
	}
	buf = hdl->buffer + pfp.pos;
	CopyMemory(buf, key, pfp.keylen);
	buf += pfp.keylen;
	*buf++ = '=';
#if defined(OSLANG_UCS2)
	ucscnv_ucs2toutf8(buf, datalen + 1, data, (UINT)-1);
#else
	CopyMemory(buf, data, datalen);
#endif
	buf += datalen;
#if defined(OSLINEBREAK_CR) || defined(OSLINEBREAK_CRLF)
	*buf++ = '\r';
#endif
#if defined(OSLINEBREAK_LF) || defined(OSLINEBREAK_CRLF)
	*buf++ = '\n';
#endif
	return(SUCCESS);
}

static void bitmapset(UINT8 *ptr, UINT pos, BRESULT set) {

	UINT8	bit;

	ptr += (pos >> 3);
	bit = 1 << (pos & 7);
	if (set) {
		*ptr |= bit;
	}
	else {
		*ptr &= ~bit;
	}
}

static BRESULT bitmapget(const UINT8 *ptr, UINT pos) {

	return((ptr[pos >> 3] >> (pos & 7)) & 1);
}

static void binset(UINT8 *bin, UINT binlen, const OEMCHAR *src) {

	UINT	i;
	UINT8	val;
	BRESULT	set;
	OEMCHAR	c;

	for (i=0; i<binlen; i++) {
		val = 0;
		set = FALSE;
		while(*src == ' ') {
			src++;
		}
		while(1) {
			c = *src;
			if ((c == '\0') || (c == ' ')) {
				break;
			}
			else if ((c >= '0') && (c <= '9')) {
				val <<= 4;
				val += c - '0';
				set = TRUE;
			}
			else {
				c |= 0x20;
				if ((c >= 'a') && (c <= 'f')) {
					val <<= 4;
					val += c - 'a' + 10;
					set = TRUE;
				}
			}
			src++;
		}
		if (set == FALSE) {
			break;
		}
		bin[i] = val;
	}
}

static void binget(OEMCHAR *work, UINT size, const UINT8 *bin, UINT binlen) {

	UINT	i;
	OEMCHAR	tmp[8];

	if (binlen) {
		OEMSPRINTF(tmp, OEMTEXT("%.2x"), bin[0]);
		milstr_ncpy(work, tmp, size);
	}
	for (i=1; i<binlen; i++) {
		OEMSPRINTF(tmp, OEMTEXT(" %.2x"), bin[i]);
		milstr_ncat(work, tmp, size);
	}
}

void profile_iniread(const OEMCHAR *path, const char *app,
								const PFTBL *tbl, UINT count, PFREAD cb) {

	PFILEH	pfh;
const PFTBL	*p;
const PFTBL	*pterm;
	OEMCHAR	work[512];

	pfh = profile_open(path, 0);
	if (pfh == NULL) {
		return;
	}
	p = tbl;
	pterm = tbl + count;
	while(p < pterm) {
		if (profile_read(app, p->item, NULL, work, NELEMENTS(work), pfh)
																== SUCCESS) {
			switch(p->itemtype & PFITYPE_MASK) {
				case PFTYPE_STR:
					milstr_ncpy(p->value, work, p->arg);
					break;

				case PFTYPE_BOOL:
					*((UINT8 *)p->value) = (!milstr_cmp(work, str_true))?1:0;
					break;

				case PFTYPE_BITMAP:
					bitmapset((UINT8 *)p->value, p->arg,
								(BRESULT)((!milstr_cmp(work, str_true))?1:0));
					break;

				case PFTYPE_BIN:
					binset((UINT8 *)p->value, p->arg, work);
					break;

				case PFTYPE_SINT8:
				case PFTYPE_UINT8:
					*(UINT8 *)p->value = (UINT32)milstr_solveINT(work);
					break;

				case PFTYPE_SINT16:
				case PFTYPE_UINT16:
					*(UINT16 *)p->value = (UINT32)milstr_solveINT(work);
					break;

				case PFTYPE_SINT32:
				case PFTYPE_UINT32:
					*(UINT32 *)p->value = (UINT32)milstr_solveINT(work);
					break;

				case PFTYPE_HEX8:
					*(UINT8 *)p->value = (UINT8)milstr_solveHEX(work);
					break;

				case PFTYPE_HEX16:
					*(UINT16 *)p->value = (UINT16)milstr_solveHEX(work);
					break;

				case PFTYPE_HEX32:
					*(UINT32 *)p->value = (UINT32)milstr_solveHEX(work);
					break;

				default:
					if (cb != NULL) {
						(*cb)(p, work);
					}
					break;
			}
		}
		p++;
	}
	profile_close(pfh);
}

void profile_iniwrite(const OEMCHAR *path, const char *app,
								const PFTBL *tbl, UINT count, PFWRITE cb) {

	PFILEH		pfh;
const PFTBL		*p;
const PFTBL		*pterm;
const OEMCHAR	*set;
	OEMCHAR		work[512];

	pfh = profile_open(path, 0);
	if (pfh == NULL) {
		return;
	}
	p = tbl;
	pterm = tbl + count;
	while(p < pterm) {
		if (!(p->itemtype & PFFLAG_RO)) {
			work[0] = '\0';
			set = work;
			switch(p->itemtype & PFITYPE_MASK) {
				case PFTYPE_STR:
					set = (OEMCHAR *)p->value;
					break;

				case PFTYPE_BOOL:
					set = (*((UINT8 *)p->value))?str_true:str_false;
					break;

				case PFTYPE_BITMAP:
					set = (bitmapget((UINT8 *)p->value, p->arg))?
														str_true:str_false;
					break;

				case PFTYPE_BIN:
					binget(work, NELEMENTS(work), (UINT8 *)p->value, p->arg);
					break;

				case PFTYPE_SINT8:
					OEMSPRINTF(work, str_d, *((SINT8 *)p->value));
					break;

				case PFTYPE_SINT16:
					OEMSPRINTF(work, str_d, *((SINT16 *)p->value));
					break;

				case PFTYPE_SINT32:
					OEMSPRINTF(work, str_d, *((SINT32 *)p->value));
					break;

				case PFTYPE_UINT8:
					OEMSPRINTF(work, str_u, *((UINT8 *)p->value));
					break;

				case PFTYPE_UINT16:
					OEMSPRINTF(work, str_u, *((UINT16 *)p->value));
					break;

				case PFTYPE_UINT32:
					OEMSPRINTF(work, str_u, *((UINT32 *)p->value));
					break;

				case PFTYPE_HEX8:
					OEMSPRINTF(work, str_x, *((UINT8 *)p->value));
					break;

				case PFTYPE_HEX16:
					OEMSPRINTF(work, str_x, *((UINT16 *)p->value));
					break;

				case PFTYPE_HEX32:
					OEMSPRINTF(work, str_x, *((UINT32 *)p->value));
					break;

				default:
					if (cb != NULL) {
						set = (*cb)(p, work, sizeof(work));
					}
					else {
						set = NULL;
					}
					break;
			}
			if (set) {
				profile_write(app, p->item, set, pfh);
			}
		}
		p++;
	}
	profile_close(pfh);
}

