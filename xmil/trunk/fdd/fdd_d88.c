#include	"compiler.h"
#include	"dosio.h"
#include	"pccore.h"
#include	"x1_io.h"
#include	"x1_fdc.h"
#include	"fdd_mtr.h"
#include	"fdd_d88.h"
#include	"fddfile.h"


static	D88_HEADER	d88head[4];
static	D88_SECTOR	cursec;
static	short		curdrv = -1;
static	WORD		curtrk = -1;
static	BYTE		curtype = 0;
static	WORD		cursct = -1;
static	long		curfp = 0;
static	DWORD		cursize = 0;
static	BYTE		curwrite = 0;
static	BYTE		curtrkerr = 0;
static	BYTE		hole = 0;
static	BYTE		*curdata;
static	WORD		crcnum = 0;
static	BYTE		crcerror = FALSE;

extern	BYTE		WRITEPT[];
extern	BYTE		DISKNUM[];

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

static UINT32 nexttrackp(D88_HEADER *head, UINT32 fptr, UINT32 last) {

	UINT	t;
const DWORD	*trkp;
	UINT32	cur;

	trkp = (DWORD *)head->trackp;
	for (t=0; t<164; t++) {
		cur = trkp[t];
		if ((cur > fptr) && (cur < last)) {
			last = cur;
		}
	}
	return(last);
}


int curdataflush(void) {

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


DWORD read_d88track(short drv, WORD track, BYTE type) {

	FDDFILE	fdd;
	FILEH	hdr;

	curdataflush();
	curdrv = drv;
	curtrk = track;
	curtype = type;
	cursct = -1;
	curwrite = 0;
	curtrkerr = 0;
	crcnum = 0;
	crcerror = 0;

	if ((drv < 0) || (drv > 3) || (track > 163) ||
		((type == 0) && (d88head[drv].fd_type == 0x20)) ||
		((type == 1) && (d88head[drv].fd_type != 0x20)) ||
		((curfp = d88head[drv].trackp[track]) == 0) ||
		((cursize = nexttrackp(&d88head[drv], curfp,
								d88head[drv].fd_size) - curfp) > 0x4000)) {
		goto readd88_err;
	}

	fdd = fddfile + drv;
	if ((hdr = file_open(fdd->fname)) == FILEH_INVALID) {
		goto readd88_err;
	}
	if (file_seek(hdr, curfp, FSEEK_SET) != curfp) {
		file_close(hdr);
		goto readd88_err;
	}
	if (file_read(hdr, D88_BUF, (WORD)cursize) != (WORD)cursize) {
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




static int seeksector(short drv, WORD track, WORD sector) {

static	int		lastflag = FALSE;

	BYTE 		*p;
	WORD		sec;

	if ((curdrv != drv) || (curtrk != track) || (curtype != fdc.media)) {
		read_d88track(drv, track, fdc.media);
	}
	if (curtrkerr) {
		cursct = sector;
		goto seekerror;
	}
	if (cursct != sector) {
		cursct = sector;
		p = D88_BUF;
		for (sec=0; sec<40;) {
			if ((WORD)((D88_SECTOR *)p)->r == sector) {
				memcpy(&cursec, p, sizeof(D88_SECTOR));
				curdata = p + sizeof(D88_SECTOR);
				lastflag = TRUE;
				break;
			}
			if (++sec >= ((D88_SECTOR *)p)->sectors) {
				goto seekerror;
			}
			p += ((D88_SECTOR *)p)->size;
			p += sizeof(D88_SECTOR);
		}
		if (sec >= 40) {
			goto seekerror;
		}
	}
	return(lastflag);

seekerror:;
	ZeroMemory(&cursec, sizeof(D88_SECTOR));
	curdata = &D88_BUF[16];
	lastflag = FALSE;
	return(FALSE);
}


void drvflush(short drv) {

	if (curdrv == drv) {
		curdataflush();
		curdrv = -1;
		curtrk = -1;
		cursct = -1;
	}
}




//**********************************************************************

short fdd_crc_d88(void) {

	WORD		track;
	BYTE		*p;
	WORD		sec;

	ZeroMemory(fdc.crc_dat, 6);
	if ((track = (WORD)(fdc.c << 1) + (WORD)fdc.h) > 163) {
		goto crcerror_d88;
	}
	seeksector(fdc.drv, track, fdc.r);
	if (curtrkerr) {
		goto crcerror_d88;
	}
	p = D88_BUF;
	for (sec=0; sec<crcnum;) {
		if (++sec >= ((D88_SECTOR *)p)->sectors) {
			goto crcerror_d88;
		}
		p += ((D88_SECTOR *)p)->size;
		p += sizeof(D88_SECTOR);
	}
	*(long *)fdc.crc_dat = *(long *)p;
	fdc.rreg = ((D88_SECTOR *)p)->c;				// ??? ﾒﾙﾍﾝｳﾞｪｰﾙ
	crcnum++;
	if (((D88_SECTOR *)p)->stat) {
		crcerror = TRUE;
	}
	else {
		crcerror = FALSE;
	}
	return(0);

crcerror_d88:;
	crcerror = TRUE;
	return(1);
}


BYTE fdd_stat_d88(void) {

	BYTE		type, cmnd;
	BYTE		ans = 0;
	int			seekable;
	WORD		trk;

	if (DISKNUM[fdc.drv] == DRV_EMPTY) {
		return(0x80);						// NOT READY
	}
	type = fdc.type;
	cmnd = (BYTE)(fdc.cmnd >> 4);
	trk = (fdc.c << 1) + (WORD)fdc.h;
	seekable = seeksector(fdc.drv, trk, fdc.r);
	if (!fdc.r) {
		seekable = TRUE;
	}

	if (type == 0 || type == 1 || type == 4 ||
		cmnd == 0x0a || cmnd == 0x0b || cmnd == 0x0f) {
		if (d88head[fdc.drv].protect & 0x10) {	// WRITE PROTECT
			ans |= 0x40;
		}
	}
	if (type == 2 || cmnd == 0x0f) {
		if (fdc.r && cursec.del_flg) {
			ans |= 0x20;					// RECODE TYPE / WRITE FAULT
		}
	}
	if (type == 1 || type == 2) {
		if ((trk > 163) || (!seekable)) {
			ans |= 0x10;					// SEEK ERROR / RECORD NOT FOUND
		}
		if ((!(ans & 0xf0)) && fdc.r && (cursec.stat)) {
			ans |= 0x08;					// CRC ERROR
		}
	}
	if (cmnd == 0x0c) {
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
			if (!fdc.c) {					// TRACK00
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
		if ((type == 2) && ((WORD)fdc.off < cursec.size)) {
			ans |= 0x03; 					// DATA REQUEST / BUSY
		}
		else if ((cmnd == 0x0e) && (readdiag < 0x1a00)) {
			ans |= 0x03;
		}
		else if ((cmnd == 0x0c) && (fdc.crc_off < 6)) {		// ver0.25
			ans |= 0x03;
		}
		else if (cmnd == 0x0f) {
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
		fdc.data = curdata[fdc.off];
	}
}


void fdd_write_d88(void) {

	if ((fdd_stat_d88() & 0xf3) == 3) {
		curdata[fdc.off] = fdc.data;
		curwrite = 1;
	}
}


BYTE fdd_incoff_d88(void) {

	BYTE	cmnd;
	WORD	trk;

	cmnd = (BYTE)(fdc.cmnd >> 4);
	trk = (fdc.c << 1) + (WORD)fdc.h;
	seeksector(fdc.drv, trk, fdc.r);
	if ((WORD)(++fdc.off) < cursec.size) {
		return(0);
	}
	fdc.off = cursec.size;
	if (cmnd == 0x09 || cmnd == 0x0b) {
		fdc.rreg = fdc.r + 1;						// ver0.25
		if (seeksector(fdc.drv, trk, fdc.rreg)) {
			fdc.r++;
			fdc.off = 0;
			return(0);
		}
	}
	return(1);
}

// ---------------------------------------------------------------------------

void init_tao_d88(void) {

	if ((fdc.media == 0) && (d88head[fdc.drv].fd_type != 0x20)) {
		tao = WID_2D;
	}
	else if ((fdc.media == 1) && (d88head[fdc.drv].fd_type == 0x20)) {
		tao = WID_2HD;
	}
	else {
		tao = WID_ERR;
	}
}

int fileappend(FILEH hdr, D88_HEADER *head,
									long ptr, long last, long apsize) {

	long	length;
	WORD	size;
	WORD	rsize;
	int		t;
	long	*trkp;

	if ((length = last - ptr) <= 0) {			// 書き換える必要なし
		return(0);
	}
	while(length) {
		if (length >= 0x4000) {
			size = 0x4000;
		}
		else {
			size = (WORD)length;
		}
		length -= (long)size;
		file_seek(hdr, ptr+length, 0);
		rsize = file_read(hdr, D88_BUF, size);
		file_seek(hdr, ptr+length+apsize, 0);
		file_write(hdr, D88_BUF, rsize);
	}

	trkp = head->trackp;
	for (t=0; t<164; t++, trkp++) {
		if ((*trkp) && (*trkp >= ptr)) {
			(*trkp) += apsize;
		}
	}
	return(0);
}



void endoftrack(void) {

	FDDFILE		fdd;
	FILEH		hdr;
	int			i;
	WORD		trk;
	D88_HEADER	*head;
	long		fpointer;
	long		endpointer;
	long		lastpointer;
	long		trksize;
	int			ptr;
	long		apsize;

	if (!tao.sector) {
		tao.flag = 4;
		return;
	}
	tao.flag = 0;

	curdataflush();						// write cache flush &
	curdrv = -1;						// use D88_BUF[] for temp

	head = &d88head[fdc.drv];
	trk = (fdc.c << 1) + (WORD)fdc.h;

	ptr = 0;
	for (i=0; i<(int)tao.sector; i++) {
		((D88_SECTOR *)&TAO_BUF[ptr])->sectors = tao.sector;
		ptr += ((D88_SECTOR *)&TAO_BUF[ptr])->size + 16;
	}

	fdd = fddfile + fdc.drv;
	if ((hdr = file_open(fdd->fname)) == FILEH_INVALID) {
		return;
	}
	lastpointer = file_seek(hdr, 0, 2);
	if ((fpointer = head->trackp[trk]) == 0) {
		fpointer = 0;								// 新規トラック
		for (i=trk; i>=0; i--) {
			if (head->trackp[i]) {
				fpointer = head->trackp[i];
				break;
			}
		}
		if (fpointer) {								// ヒットした
			fpointer = nexttrackp(head, fpointer, lastpointer);
		}
		else {
			fpointer = sizeof(D88_HEADER);
		}
		endpointer = fpointer;
	}
	else {										// トラックデータは既にある
		endpointer = nexttrackp(head, fpointer, lastpointer);
	}
	trksize = endpointer - fpointer;
	if ((apsize = (long)tao.size - trksize) > 0) {
								// 書き込むデータのほーが大きい
		fileappend(hdr, head, endpointer, lastpointer, apsize);
		head->fd_size += apsize;
	}
	head->trackp[trk] = fpointer;
	file_seek(hdr, fpointer, 0);
	file_write(hdr, TAO_BUF, tao.size);
	file_seek(hdr, 0, 0);
	file_write(hdr, head, sizeof(D88_HEADER));
	file_close(hdr);
}


void fdd_wtao_d88(BYTE data) {

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
				((D88_SECTOR *)&TAO_BUF[tao.top])->del_flg = 1;
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
				ZeroMemory(&TAO_BUF[tao.size], 12);
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
				((D88_SECTOR *)&TAO_BUF[tao.top])->size = tao.ptr;
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


// ----

BRESULT fddd88_eject(REG8 drv) {

	FDDFILE	fdd;

	drvflush(drv);
	fdd = fddfile + drv;
	ZeroMemory(&d88head[drv], sizeof(D88_HEADER));
	fdd->fname[0] = '\0';
	DISKNUM[drv] = 0;
	return(SUCCESS);
}

BRESULT fddd88_set(REG8 drv, const OEMCHAR *fname) {

	FDDFILE	fdd;
	short	attr;
	FILEH	fh;
	UINT	rsize;

	fdd = fddfile + drv;
	attr = file_attr(fname);
	if (attr & 0x18) {
		goto fdst_err;
	}
	fh = file_open(fname);
	if (fh == FILEH_INVALID) {
		goto fdst_err;
	}
	rsize = file_read(fh, &d88head[drv], sizeof(D88_HEADER));
	file_close(fh);
	if (rsize != sizeof(D88_HEADER)) {
		goto fdst_err;
	}
	if (attr & 1) {
		d88head[drv].protect |= (WORD)0x10;
	}
	milstr_ncpy(fdd->fname, fname, NELEMENTS(fdd->fname));
	DISKNUM[drv] = 1;
	return(SUCCESS);

fdst_err:
	return(FAILURE);
}

