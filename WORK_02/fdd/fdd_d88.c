#include	"compiler.h"
#include	"dosio.h"
#include	"pccore.h"
#include	"fddfile.h"
#include	"fdd_d88.h"
#include	"fdd_mtr.h"


enum {
	D88BUFSIZE		= 0x4000
};

typedef struct {
	FDDFILE	fdd;
	UINT8	media;
	UINT8	write;
	UINT8	padding[2];
	UINT	track;
	long	fptr;
	UINT	size;
	UINT	sectors;
	UINT8	buf[D88BUFSIZE];
} _D88TRK, *D88TRK;

static	_D88TRK		d88trk;


static UINT32 nexttrackptr(FDDFILE fdd, UINT32 fptr, UINT32 last) {

	UINT	t;
	UINT32	cur;

	for (t=0; t<164; t++) {
		cur = fdd->inf.d88.ptr[t];
		if ((cur > fptr) && (cur < last)) {
			last = cur;
		}
	}
	return(last);
}

static BRESULT trackflush(D88TRK trk) {

	FDDFILE	fdd;
	FILEH	fh;

	fdd = trk->fdd;
	trk->fdd = NULL;
	if ((fdd == NULL) || (trk->size == 0) || (!trk->write)) {
		goto dtfd_exit;
	}
	fh = file_open(fdd->fname);
	if (fh == FILEH_INVALID) {
		goto dtfd_err1;
	}
	if ((file_seek(fh, trk->fptr, FSEEK_SET) != trk->fptr) ||
		(file_write(fh, trk->buf, trk->size) != trk->size)) {
		goto dtfd_err2;
	}
	file_close(fh);
	trk->write = FALSE;

dtfd_exit:
	return(SUCCESS);

dtfd_err2:
	file_close(fh);

dtfd_err1:
	return(FAILURE);
}

static D88TRK trackread(D88TRK trk, FDDFILE fdd, REG8 media, UINT track) {

	FILEH		fh;
	UINT32		fptr;
	UINT		size;
const _D88SEC	*sec;
	UINT		rem;
	UINT		maxsectors;
	UINT		sectors;
	UINT		secsize;

	trackflush(trk);
	if (media != (REG8)((fdd->inf.d88.head.fd_type >> 4))) {
		goto dtrd_err1;
	}
	if (track >= 164) {
		goto dtrd_err1;
	}
	fptr = fdd->inf.d88.ptr[track];
	if (fptr == 0) {
		goto dtrd_err1;
	}
	size = nexttrackptr(fdd, fptr, fdd->inf.d88.fd_size) - fptr;
	if (size > D88BUFSIZE) {
		size = D88BUFSIZE;
	}
	fh = file_open_rb(fdd->fname);
	if (fh == FILEH_INVALID) {
		goto dtrd_err1;
	}
	if ((file_seek(fh, (long)fptr, FSEEK_SET) != (long)fptr) ||
		(file_read(fh, trk->buf, size) != size)) {
		goto dtrd_err2;
	}
	file_close(fh);

	trk->fdd = fdd;
	trk->media = media;
	trk->write = FALSE;
	trk->track = track;
	trk->fptr = fptr;
	trk->size = size;

	/* セクタ数チェック */
	sec = (D88SEC)trk->buf;
	rem = size;
	maxsectors = 0;
	if (rem >= sizeof(_D88SEC)) {
		maxsectors = LOADINTELWORD(sec->sectors);
	}
	sectors = 0;
	while(sectors < maxsectors) {
		secsize = LOADINTELWORD(sec->size);
		secsize += sizeof(_D88SEC);
		if (rem < secsize) {
			break;
		}
		rem -= secsize;
		maxsectors = LOADINTELWORD(sec->sectors);
		sec = (D88SEC)(((UINT8 *)sec) + secsize);
		sectors++;
	}
	trk->sectors = sectors;
	return(trk);

dtrd_err2:
	file_close(fh);

dtrd_err1:
	return(NULL);
}

static void drvflush(FDDFILE fdd) {

	D88TRK	trk;

	trk = &d88trk;
	if (trk->fdd == fdd) {
		trackflush(trk);
	}
}

static D88TRK trackseek(FDDFILE fdd, REG8 media, UINT track) {

	D88TRK	trk;

	trk = &d88trk;
	if ((trk->fdd != fdd) || (trk->media != media) || (trk->track != track)) {
		trk = trackread(trk, fdd, media, track);
	}
	return(trk);
}

static D88SEC sectorseek(const _D88TRK *trk, REG8 r) {

const _D88SEC	*sec;
	UINT		sectors;
	UINT		size;

	sec = (D88SEC)trk->buf;
	sectors = trk->sectors;
	while(sectors) {
		sectors--;
		if (sec->r == r) {
			return((D88SEC)sec);
		}
		size = LOADINTELWORD(sec->size);
		size += sizeof(_D88SEC);
		sec = (D88SEC)(((UINT8 *)sec) + size);
	}
	return(NULL);
}


/* ---- */

static BRESULT fileappend(FILEH fh, UINT32 ptr, UINT size) {

	UINT	filesize;
	UINT	r;
	UINT8	work[0x400];

	filesize = file_getsize(fh);
	if (filesize < ptr) {
		return(FAILURE);
	}
	filesize -= ptr;
	while(filesize) {
		r = min(filesize, sizeof(work));
		filesize -= r;
		file_seek(fh, ptr + filesize, FSEEK_SET);
		r = file_read(fh, work, r);
		file_seek(fh, ptr + filesize + size, FSEEK_SET);
		file_write(fh, work, r);
	}
	return(SUCCESS);
}

static BRESULT writetrack(FDDFILE fdd, REG8 media, UINT track,
													UINT8 *buf, UINT size) {

	FILEH	fh;
	UINT	i;
	UINT32	curptr;
	UINT32	nextptr;
	UINT	cursize;
	UINT	addsize;
	UINT32	cur;
	UINT8	ptr[D88_TRACKMAX][4];

	if ((track >= D88_TRACKMAX) || (size == 0)) {
		return(FAILURE);
	}

	fh = file_open(fdd->fname);
	if (fh == FILEH_INVALID) {
		return(FAILURE);
	}
	curptr = fdd->inf.d88.ptr[track];
	if (curptr == 0) {
		for (i=track; i>0;) {				/* 新規トラック */
			curptr = fdd->inf.d88.ptr[--i];
			if (curptr) {
				break;
			}
		}
		if (curptr) {						/* ヒットした */
			curptr = nexttrackptr(fdd, curptr, fdd->inf.d88.fd_size);
		}
		else {
			curptr = D88_HEADERSIZE;
		}
		nextptr = curptr;
	}
	else {									/* トラックデータは既にある */
		nextptr = nexttrackptr(fdd, curptr, fdd->inf.d88.fd_size);
	}
	cursize = nextptr - curptr;
	if (size > cursize) {
		addsize = size - cursize;
		fileappend(fh, curptr, addsize);
		fdd->inf.d88.fd_size += addsize;
		for (i=0; i<D88_TRACKMAX; i++) {
			cur = fdd->inf.d88.ptr[i];
			if ((cur) && (cur >= curptr)) {
				fdd->inf.d88.ptr[i] = cur + addsize;
			}
		}
	}
	STOREINTELDWORD(fdd->inf.d88.head.fd_size, fdd->inf.d88.fd_size);
	fdd->inf.d88.ptr[track] = curptr;
	for (i=0; i<D88_TRACKMAX; i++) {
		STOREINTELDWORD(ptr[i], fdd->inf.d88.ptr[i]);
	}
	file_seek(fh, 0, FSEEK_SET);
	file_write(fh, &fdd->inf.d88.head, sizeof(fdd->inf.d88.head));
	file_write(fh, ptr, sizeof(ptr));
	file_seek(fh, curptr, FSEEK_SET);
	file_write(fh, buf, size);
	file_close(fh);
	return(SUCCESS);
}


/* ---- */

static REG8 fddd88_seek(FDDFILE fdd, REG8 media, UINT track) {

	if (trackseek(fdd, media, track) != NULL) {
		return(0x00);
	}
	else {
		return(FDDSTAT_SEEKERR);
	}
}

static REG8 fddd88_read(FDDFILE fdd, REG8 media, UINT track, REG8 sc,
												UINT8 *ptr, UINT *size) {

const _D88TRK	*trk;
const _D88SEC	*sec;
	UINT		secsize;
	REG8		ret;

	TRACEOUT(("d88 read %d:%.2x", track, sc));
	trk = trackseek(fdd, media, track);
	if (trk == NULL) {
		goto fd8r_err;
	}
	sec = sectorseek(trk, sc);
	if (sec == NULL) {
		goto fd8r_err;
	}
	ret = 0x00;
	if (sec->del_flg) {
		ret |= FDDSTAT_RECTYPE;
	}
	if (sec->stat) {
		ret |= FDDSTAT_CRCERR;
	}
	secsize = LOADINTELWORD(sec->size);
	secsize = min(secsize, *size);
	if ((ptr) && (secsize)) {
		CopyMemory(ptr, sec + 1, secsize);
	}
	*size = secsize;
	return(ret);

fd8r_err:
	return(FDDSTAT_RECNFND);
}

static REG8 fddd88_write(FDDFILE fdd, REG8 media, UINT track, REG8 sc,
												const UINT8 *ptr, UINT size) {

	D88TRK	trk;
	D88SEC	sec;
	UINT	secsize;

	TRACEOUT(("d88 write %d:%.2x", track, sc));
	trk = trackseek(fdd, media, track);
	if (trk == NULL) {
		goto fd8w_err;
	}
	sec = sectorseek(trk, sc);
	if (sec == NULL) {
		goto fd8w_err;
	}
	secsize = LOADINTELWORD(sec->size);
	size = min(size, secsize);
	if (size) {
		CopyMemory(sec + 1, ptr, size);
		trk->write = TRUE;
	}
	return(0x00);

fd8w_err:
	return(FDDSTAT_RECNFND | FDDSTAT_WRITEFAULT);
}

static REG8 fddd88_wrtrk(FDDFILE fdd, REG8 media, UINT track, REG8 sc,
												const UINT8 *ptr, UINT size) {

	UINT	pos;
	UINT	i;
	UINT	datsize;
	D88SEC	dst;

	trackflush(&d88trk);

	/* データ作る */
	ZeroMemory(d88trk.buf, sizeof(d88trk.buf));
	pos = 0;
	for (i=0; i<sc; i++) {
		dst = (D88SEC)(d88trk.buf + pos);
		datsize = LOADINTELWORD(((TAOSEC *)ptr)->size);
		pos += sizeof(_D88SEC) + datsize;
		if (pos > D88BUFSIZE) {
			goto fd8wt_err;
		}
		dst->c = ((TAOSEC *)ptr)->c;
		dst->h = ((TAOSEC *)ptr)->h;
		dst->r = ((TAOSEC *)ptr)->r;
		dst->n = ((TAOSEC *)ptr)->n;
		dst->sectors[0] = sc;
		STOREINTELWORD(dst->size, datsize);
		CopyMemory((dst + 1), ptr + sizeof(TAOSEC), datsize);
		ptr += sizeof(TAOSEC) + datsize;
	}
	if (writetrack(fdd, media, track, d88trk.buf, pos) != SUCCESS) {
		goto fd8wt_err;
	}
	return(0);

fd8wt_err:
	return(FDDSTAT_RECNFND | FDDSTAT_WRITEFAULT);
}

static REG8 fddd88_crc(FDDFILE fdd, REG8 media, UINT track, UINT num,
												UINT8 *ptr) {

const _D88TRK	*trk;
const _D88SEC	*sec;
	UINT		sectors;
	UINT		size;

	trk = trackseek(fdd, media, track);
	if (trk == NULL) {
		return(FDDSTAT_RECNFND);
	}
	sec = (D88SEC)trk->buf;
	sectors = trk->sectors;
	if (num >= sectors) {
		return(FDDSTAT_RECNFND);
	}
	while(num) {
		num--;
		size = LOADINTELWORD(sec->size);
		size += sizeof(_D88SEC);
		sec = (D88SEC)(((UINT8 *)sec) + size);
	}
	ptr[0] = sec->c;
	ptr[1] = sec->h;
	ptr[2] = sec->r;
	ptr[3] = sec->n;
	ptr[4] = 0;
	ptr[5] = 0;
#if 0
	fdc.s.rreg = sec->c; 							/* メルヘンヴェール */
#endif	/* 0 */
	if (sec->stat) {
		return(FDDSTAT_CRCERR);
	}
	return(0x00);
}

#if defined(SUPPORT_DISKEXT)
static UINT32 fddd88_sec(FDDFILE fdd, REG8 media, UINT track, REG8 sc) {

const _D88TRK	*trk;
const _D88SEC	*sec;
	UINT		sectors;
	UINT		num;
	UINT		size;

	trk = trackseek(fdd, media, track);
	if (trk == NULL) {
		return(0);
	}
	sec = (D88SEC)trk->buf;
	sectors = trk->sectors;
	num = 0;
	while(num < sectors) {
		if (sec->r == sc) {
			break;
		}
		size = LOADINTELWORD(sec->size);
		size += sizeof(_D88SEC);
		sec = (D88SEC)(((UINT8 *)sec) + size);
		num++;
	}
	return((UINT32)(sectors << 16) + num);
}
#endif


/* ---- */

BRESULT fddd88_set(FDDFILE fdd, const OEMCHAR *fname) {

	short	attr;
	FILEH	fh;
	BRESULT	r;
	UINT8	ptr[D88_TRACKMAX][4];
	UINT	i;

	attr = file_attr(fname);
	if (attr & 0x18) {
		goto fdst_err;
	}
	fh = file_open_rb(fname);
	if (fh == FILEH_INVALID) {
		goto fdst_err;
	}
	r = (file_read(fh, &fdd->inf.d88.head, sizeof(fdd->inf.d88.head))
											!= sizeof(fdd->inf.d88.head)) ||
		(file_read(fh, ptr, sizeof(ptr)) != sizeof(ptr));
	file_close(fh);
	if (r) {
		goto fdst_err;
	}
	fdd->inf.d88.fd_size = LOADINTELDWORD(fdd->inf.d88.head.fd_size);
	for (i=0; i<D88_TRACKMAX; i++) {
		fdd->inf.d88.ptr[i] = LOADINTELDWORD(ptr[i]);
	}
	if (fdd->inf.d88.head.protect & 0x10) {
		attr |= 1;
	}
	fdd->type = DISKTYPE_D88;
	fdd->protect = (UINT8)(attr & 1);
	fdd->seek = fddd88_seek;
	fdd->read = fddd88_read;
	fdd->write = fddd88_write;
	fdd->wrtrk = fddd88_wrtrk;
	fdd->crc = fddd88_crc;
#if defined(SUPPORT_DISKEXT)
	fdd->sec = fddd88_sec;
#endif
	return(SUCCESS);

fdst_err:
	return(FAILURE);
}

void fddd88_eject(FDDFILE fdd) {

	drvflush(fdd);
}

