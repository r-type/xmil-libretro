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

	FILEH	fh;
	UINT32	fptr;
	UINT32	size;

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
	UINT		rem;
	UINT		secnum;
	UINT		sectors;
	UINT		size;

	sec = (D88SEC)trk->buf;
	rem = trk->size;
	sectors = LOADINTELWORD(sec->sectors);
	if (sectors) {
		secnum = 0;
		do {
			size = LOADINTELWORD(sec->size);
			size += sizeof(_D88SEC);
			if (rem < size) {
				break;
			}
			if (sec->r == r) {
				return((D88SEC)sec);
			}
			secnum++;
			sectors = LOADINTELWORD(sec->sectors);
			if (secnum >= sectors) {
				break;
			}
			sec = (D88SEC)(((UINT8 *)sec) + size);
		} while(secnum < 40);
	}
	return(NULL);
}


// ----

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

static REG8 fddd88_crc(FDDFILE fdd, REG8 media, UINT track, UINT num,
												UINT8 *ptr) {

const _D88TRK	*trk;
const _D88SEC	*sec;
	UINT		secnum;
	UINT		rem;
	UINT		size;
	UINT		sectors;

	trk = trackseek(fdd, media, track);
	if (trk == NULL) {
		return(FDDSTAT_RECNFND);
	}
	sec = (D88SEC)trk->buf;
	sectors = LOADINTELWORD(sec->sectors);
	if (sectors == 0) {
		return(FDDSTAT_RECNFND);
	}
	secnum = 0;
	rem = trk->size;
	while(1) {
		size = LOADINTELWORD(sec->size);
		size += sizeof(_D88SEC);
		if (rem < size) {
			return(FDDSTAT_RECNFND);
		}
		if (num == secnum) {
			break;
		}
		secnum++;
		sectors = LOADINTELWORD(sec->sectors);
		if (secnum >= sectors) {
			return(FDDSTAT_RECNFND);
		}
		sec = (D88SEC)(((UINT8 *)sec) + size);
	}
	ptr[0] = sec->c;
	ptr[1] = sec->h;
	ptr[2] = sec->r;
	ptr[3] = sec->n;
	ptr[4] = 0;
	ptr[5] = 0;
//	fdc.s.rreg = sec->c;								// メルヘンヴェール
	if (sec->stat) {
		return(FDDSTAT_CRCERR);
	}
	return(0x00);
}


// ----

BRESULT fddd88_set(FDDFILE fdd, const OEMCHAR *fname) {

	short	attr;
	FILEH	fh;
	BOOL	r;
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
	fdd->crc = fddd88_crc;
	return(SUCCESS);

fdst_err:
	return(FAILURE);
}

void fddd88_eject(FDDFILE fdd) {

	drvflush(fdd);
}












// ---------------------------------------------------------------------------

#if 0
// ----

static	_D88SEC		cursec;
static	REG8		curdrv = (REG8)-1;
static	UINT		curtrk = (UINT)-1;
static	BYTE		curtype = 0;
static	UINT		cursct = (UINT)-1;
static	long		curfp = 0;
static	UINT		cursize = 0;
static	BYTE		curwrite = 0;
static	BYTE		curtrkerr = 0;
static	BYTE		hole = 0;
static	BYTE		*curdata;
// static	UINT		crcnum = 0;
// static	BYTE		crcerror = FALSE;

extern	WORD		readdiag;

#define		TAO_MODE_GAP	0x4e
#define		TAO_MODE_SYNC	0x00
#define		TAO_MODE_AM		0xf5
#define		TAO_MODE_IM		0xf6
#define		TAO_MODE_ID		0xfe
#define		TAO_MODE_DATA	0xfb
#define		TAO_ENDOFDATA	0xf7

#define		TAO_CMD_GAP		0x4e
#define		TAO_CMD_SYNC	0x00
#define		TAO_CMD_IM_IN	0xf6
#define		TAO_CMD_IM		0xfc
#define		TAO_CMD_AM_IN	0xf5
#define		TAO_CMD_IAM		0xfe
#define		TAO_CMD_DAM		0xfb
#define		TAO_CMD_DDAM	0xf8
#define		TAO_CMD_CRC		0xf7

typedef struct {
	BYTE	mode;
	BYTE	flag;
	WORD	cnt;
	WORD	size;
	WORD	ptr;
	WORD	top;
	WORD	gap;
	BYTE	sector;
	WORD	lostdatacnt;
	BYTE	maxsector;
	WORD	maxsize;
} WID_PARA;

#define LOSTDATA_COUNT	256

static	WID_PARA WID_2D	= {TAO_ENDOFDATA, 3, 0, 0, 0, 0, 0, 0, 0, 33, 0x1f80};
static	WID_PARA WID_2HD= {TAO_ENDOFDATA, 3, 0, 0, 0, 0, 0, 0, 0, 55, 0x2f80};
static	WID_PARA WID_ERR= {TAO_ENDOFDATA, 4, 0, 0, 0, 0, 0, 0, 0,  0, 0x0000};
static	WID_PARA tao	= {TAO_ENDOFDATA, 4, 0, 0, 0, 0, 0, 0, 0,  0, 0x0000};

static	BYTE		D88_BUF[0x4000];
static	BYTE		TAO_BUF[0x3000];


//----------------------------------------------------------------------


static int curdataflush(void) {

	FDDFILE	fdd;
	FILEH	fh;
	int		ret = 0;

	if ((!curfp) || (!cursize)) {
		return(-1);
	}
	if (!curwrite) {
		return(0);
	}
	curwrite = 0;
	fdd = fddfile + curdrv;
	fh = file_open(fdd->fname);
	if (fh == FILEH_INVALID) {
		return(-1);
	}
	if ((file_seek(fh, curfp, FSEEK_SET) != curfp) ||
		(file_write(fh, D88_BUF, cursize) != cursize)) {
		ret = -1;
	}
	if (file_close(fh)) {
		ret = -1;
	}
	return(ret);
}


static DWORD read_d88track(REG8 drv, UINT track, REG8 media) {

	FDDFILE	fdd;
	FILEH	hdr;

	curdataflush();
	curdrv = drv;
	curtrk = track;
	curtype = media;
	cursct = (UINT)-1;
	curwrite = 0;
	curtrkerr = 0;
//	crcnum = 0;
	crcerror = 0;

	fdd = fddfile + drv;
	if ((drv < 0) || (drv > 3) || (track > 163) ||
		(media != (fdd->inf.d88.head.fd_type >> 4)) ||
		((curfp = fdd->inf.d88.ptr[track]) == 0) ||
		((cursize = nexttrackptr(fdd, curfp,
								fdd->inf.d88.fd_size) - curfp) > 0x4000)) {
		goto readd88_err;
	}

	if ((hdr = file_open(fdd->fname)) == FILEH_INVALID) {
		goto readd88_err;
	}
	if (file_seek(hdr, curfp, FSEEK_SET) != curfp) {
		file_close(hdr);
		goto readd88_err;
	}
	if (file_read(hdr, D88_BUF, cursize) != cursize) {
		file_close(hdr);
		goto readd88_err;
	}
	if (file_close(hdr)) {
		goto readd88_err;
	}
	return(TRUE);

readd88_err:
	curfp = 0;
	cursize = 0;
	curtrkerr = 1;
	return(FALSE);
}




static int seeksector(REG8 drv, UINT track, UINT r) {

static	int		lastflag = FALSE;

const _D88SEC	*p;
	UINT		sec;
	UINT		sectors;
	UINT		secsize;

	if ((curdrv != drv) || (curtrk != track) || (curtype != fdc.s.media)) {
		read_d88track(drv, track, fdc.s.media);
	}
	if (curtrkerr) {
		cursct = r;
		goto seekerror;
	}
	if (cursct != r) {
		cursct = r;
		p = (D88SEC)D88_BUF;
		for (sec=0; sec<40; ) {
			sectors = LOADINTELWORD(p->sectors);
			secsize = LOADINTELWORD(p->size);
			if (p->r == r) {
				CopyMemory(&cursec, p, sizeof(_D88SEC));
				curdata = (UINT8 *)(p + 1);
				lastflag = TRUE;
				break;
			}
			sec++;
			if (sec >= sectors) {
				goto seekerror;
			}
			p = (D88SEC)(((UINT8 *)(p + 1)) + secsize);
		}
		if (sec >= 40) {
			goto seekerror;
		}
	}
	return(lastflag);

seekerror:;
	ZeroMemory(&cursec, sizeof(_D88SEC));
	curdata = &D88_BUF[16];
	lastflag = FALSE;
	return(FALSE);
}


// ----




//**********************************************************************



BYTE fdd_stat_d88(void) {

	FDDFILE		fdd;
	BYTE		type;
	REG8		cmd;
	UINT		trk;
	BYTE		ans = 0;
	int			seekable;

	fdd = fddfile + fdc.s.drv;
	if (fdd->type != DISKTYPE_D88) {
		return(0x80);						// NOT READY
	}
	cmd = (REG8)(fdc.s.cmd >> 4);
	type = fdc.s.type;
	trk = (fdc.s.c << 1) + fdc.s.h;
	seekable = seeksector(fdc.s.drv, trk, fdc.s.r);
	if (!fdc.s.r) {
		seekable = TRUE;
	}

	if (type == 0 || type == 1 || type == 4 ||
		cmd == 0x0a || cmd == 0x0b || cmd == 0x0f) {
		if (fdd->protect) {					// WRITE PROTECT
			ans |= 0x40;
		}
	}
	if (type == 2 || cmd == 0x0f) {
		if (fdc.s.r && cursec.del_flg) {
			ans |= 0x20;					// RECODE TYPE / WRITE FAULT
		}
	}
	if (type == 1 || type == 2) {
		if ((trk > 163) || (!seekable)) {
			ans |= 0x10;					// SEEK ERROR / RECORD NOT FOUND
		}
		if ((!(ans & 0xf0)) && fdc.s.r && (cursec.stat)) {
			ans |= 0x08;					// CRC ERROR
		}
	}
	if (cmd == 0x0c) {
		if (curtrkerr) {
			ans |= 0x10;
		}
		if (crcerror) {
			ans |= 0x08;					// CRC ERROR
		}
	}
	if (type == 1 || type == 4) {
		if (type == 1) {					// ver0.25
			ans |= 0x20;					// HEAD ENGAGED (X1 ﾃﾞﾊ ﾂﾈﾆ 1)
			if (!fdc.s.c) {					// TRACK00
				ans |= 0x04;
			}
		}
		if (++hole < 8) {					// ver0.25
			ans |= 0x02;					// INDEX
		}
	}
	else if (!(ans & 0xf0)) {
		if ((type != 4) && (FDDMTR_BUSY)) {
			ans |= 0x01;
		}
		if (type == 2) {
			UINT secsize = LOADINTELWORD(cursec.size);
			if ((UINT)fdc.off < secsize) {
				ans |= 0x03; 					// DATA REQUEST / BUSY
			}
		}
		else if ((cmd == 0x0c) && (fdc.s.bufpos < 6)) {
			ans |= 0x03;
		}
		else if ((cmd == 0x0e) && (readdiag < 0x1a00)) {
			ans |= 0x03;
		}
		else if (cmd == 0x0f) {
			if (tao.flag == 3) {
				if (++tao.lostdatacnt > LOSTDATA_COUNT) {
					tao.flag = 4;
				}
			}
			ans |= tao.flag;
		}
	}
	return(ans);
}


//**********************************************************************

void fdd_read_d88(void) {
						// POCO:読めなかったらレジスタを変更させない
	if ((fdd_stat_d88() & 0xf3) == 3) {
		fdc.s.data = curdata[fdc.off];
	}
}

void fdd_write_d88(void) {

	if ((fdd_stat_d88() & 0xf3) == 3) {
		curdata[fdc.off] = fdc.s.data;
		curwrite = 1;
	}
}

BYTE fdd_incoff_d88(void) {

	REG8	cmd;
	UINT	trk;
	UINT	secsize;

	cmd = (REG8)(fdc.s.cmd >> 4);
	trk = (fdc.s.c << 1) + fdc.s.h;
	seeksector(fdc.s.drv, trk, fdc.s.r);
	fdc.off++;
	secsize = LOADINTELWORD(cursec.size);
	if ((UINT)fdc.off < secsize) {
		return(0);
	}
	fdc.off = secsize;
	if ((cmd == 0x09) || (cmd == 0x0b)) {
		fdc.s.rreg = fdc.s.r + 1;						// ver0.25
		if (seeksector(fdc.s.drv, trk, fdc.s.rreg)) {
			fdc.s.r++;
			fdc.off = 0;
			return(0);
		}
	}
	return(1);
}


// ---------------------------------------------------------------------------

void init_tao_d88(void) {

	FDDFILE		fdd;

	fdd = fddfile + fdc.s.drv;
	if (fdc.s.media != (fdd->inf.d88.head.fd_type >> 4)) {
		tao = WID_ERR;
	}
	if (fdc.s.media == DISKTYPE_2D) {
		tao = WID_2D;
	}
	else if (fdc.s.media == DISKTYPE_2HD) {
		tao = WID_2HD;
	}
	else {
		tao = WID_ERR;
	}
}

static int fileappend(FILEH hdl, FDDFILE fdd,
									UINT32 ptr, long last, long apsize) {

	long	length;
	UINT	size;
	UINT	rsize;
	UINT	t;
	BYTE	tmp[0x400];
	UINT32	cur;

	length = last - ptr;
	if (length <= 0) {							// 書き換える必要なし
		return(0);
	}
	while(length) {
		if (length >= (long)(sizeof(tmp))) {
			size = sizeof(tmp);
		}
		else {
			size = length;
		}
		length -= size;
		file_seek(hdl, ptr + length, FSEEK_SET);
		rsize = file_read(hdl, tmp, size);
		file_seek(hdl, ptr + length + apsize, FSEEK_SET);
		file_write(hdl, tmp, rsize);
	}
	for (t=0; t<164; t++) {
		cur = fdd->inf.d88.ptr[t];
		if ((cur) && (cur >= ptr)) {
			fdd->inf.d88.ptr[t] = cur + apsize;
		}
	}
	return(0);
}

static void endoftrack(void) {

	FDDFILE	fdd;
	FILEH	hdr;
	int		i;
	UINT	trk;
	long	fpointer;
	long	endpointer;
	long	lastpointer;
	long	trksize;
	long	apsize;
	D88SEC	p;
	UINT	secsize;
	UINT8	ptr[D88_TRACKMAX][4];

	if (!tao.sector) {
		tao.flag = 4;
		return;
	}
	tao.flag = 0;

	curdataflush();						// write cache flush &
	curdrv = (REG8)-1;					// use D88_BUF[] for temp

	fdd = fddfile + fdc.s.drv;
	trk = (fdc.s.c << 1) + fdc.s.h;

	p = (D88SEC)TAO_BUF;
	for (i=0; i<(int)tao.sector; i++) {
		STOREINTELWORD(p->sectors, tao.sector);
		secsize = LOADINTELWORD(p->size);
		p = (D88SEC)(((UINT8 *)(p + 1)) + secsize);
	}

	if ((hdr = file_open(fdd->fname)) == FILEH_INVALID) {
		return;
	}
	lastpointer = file_seek(hdr, 0, 2);
	fpointer = fdd->inf.d88.ptr[trk];
	if (fpointer == 0) {
		for (i=trk; i>=0; i--) {					// 新規トラック
			fpointer = fdd->inf.d88.ptr[i];
			if (fpointer) {
				break;
			}
		}
		if (fpointer) {								// ヒットした
			fpointer = nexttrackptr(fdd, fpointer, lastpointer);
		}
		else {
			fpointer = D88_HEADERSIZE;
		}
		endpointer = fpointer;
	}
	else {										// トラックデータは既にある
		endpointer = nexttrackptr(fdd, fpointer, lastpointer);
	}
	trksize = endpointer - fpointer;
	if ((apsize = (long)tao.size - trksize) > 0) {
								// 書き込むデータのほーが大きい
		fileappend(hdr, fdd, endpointer, lastpointer, apsize);
		fdd->inf.d88.fd_size += apsize;
	}
	STOREINTELDWORD(fdd->inf.d88.head.fd_size, fdd->inf.d88.fd_size);
	fdd->inf.d88.ptr[trk] = fpointer;
	for (i=0; i<D88_TRACKMAX; i++) {
		STOREINTELDWORD(ptr[i], fdd->inf.d88.ptr[i]);
	}
	file_seek(hdr, 0, FSEEK_SET);
	file_write(hdr, &fdd->inf.d88.head, sizeof(fdd->inf.d88.head));
	file_write(hdr, ptr, sizeof(ptr));
	file_seek(hdr, fpointer, FSEEK_SET);
	file_write(hdr, TAO_BUF, tao.size);
	file_close(hdr);
}

void fdd_wtao_d88(BYTE data) {

	D88SEC	p;

	if (tao.flag != 3) {
		return;
	}
	tao.lostdatacnt = 0;
	if (tao.cnt > tao.maxsize) {
		endoftrack();
		return;
	}
	tao.cnt++;
	switch(tao.mode) {
		case TAO_ENDOFDATA:
			if (data == TAO_MODE_GAP) {
				tao.mode = TAO_MODE_GAP;
				tao.gap = 0;
			}
			break;

		case TAO_MODE_GAP:
			if (data == TAO_MODE_GAP) {
				if (tao.gap++ > 256) {
					endoftrack();
				}
			}
			else if (data == TAO_CMD_SYNC) {
				tao.mode = TAO_MODE_SYNC;
			}
#if 1												// ver0.26 Zeliard
			else if (data == 0xf4) {
				endoftrack();
			}
#endif
			else {
				tao.flag = 4;
			}
			break;

		case TAO_MODE_SYNC:
//			tao.cnt--;								// ver0.26 Zeliard
			if (data == TAO_CMD_AM_IN) {
				tao.mode = TAO_MODE_AM;
			}
			else if (data == TAO_CMD_IM_IN) {
				tao.mode = TAO_MODE_IM;
			}
			else if (data) {
				tao.flag = 4;
			}
			break;

		case TAO_MODE_IM:
			if (data == TAO_CMD_IM) {
				tao.mode = TAO_ENDOFDATA;
			}
			else if (data != TAO_CMD_IM_IN) {
				tao.flag = 4;
			}
			break;

		case TAO_MODE_AM:
			if (data == TAO_CMD_IAM) {
				tao.mode = TAO_MODE_ID;
				tao.ptr = 0;
				tao.top = tao.size;
			}
			else if (data == TAO_CMD_DAM) {
				tao.mode = TAO_MODE_DATA;
				tao.ptr = 0;
			}
			else if (data == TAO_CMD_DDAM) {
				tao.mode = TAO_MODE_DATA;
				tao.ptr = 0;
				p = (D88SEC)(TAO_BUF + tao.top);
				p->del_flg = 1;
			}
			break;

		case TAO_MODE_ID:
			if ((data == TAO_CMD_IAM) && (!tao.ptr)) {
				break;
			}
			else if (tao.ptr < 4) {
				TAO_BUF[tao.size++] = data;
				tao.ptr++;
			}
			else if (data == TAO_CMD_CRC) {
				tao.mode = TAO_ENDOFDATA;
				ZeroMemory(TAO_BUF + tao.size, 12);
				tao.size += 12;
			}
			break;

		case TAO_MODE_DATA:						// DATA WRITE
			if ((!tao.ptr) &&
				((data == TAO_CMD_DAM) || (data == TAO_CMD_DDAM))) {
				break;
			}
			else if (data == TAO_CMD_CRC) {			// nで判定した方が無難か？
				tao.mode = TAO_ENDOFDATA;
				p = (D88SEC)(TAO_BUF + tao.top);
				STOREINTELWORD(p->size, tao.ptr);
				if (tao.sector++ > tao.maxsector) {
					tao.flag = 4;
				}
			}
			else {
				TAO_BUF[tao.size++] = data;
				tao.ptr++;
			}
			break;
	}
}
#endif

