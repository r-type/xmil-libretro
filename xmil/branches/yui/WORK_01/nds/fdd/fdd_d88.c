#include	"compiler.h"
#include	"pccore.h"
#include	"fddfile.h"
#include	"fdd_d88.h"


static UINT32 nexttrackptr(FDDFILE fdd, UINT32 fptr, UINT32 last) {

const UINT8	*ptr;
	UINT	t;
	UINT32	cur;

	ptr = fdd->inf.d88.hdr->trkptr[0];
	for (t=0; t<164; t++) {
#if defined(BYTESEX_LITTLE)
		cur = *(UINT32 *)ptr;
#else
		cur = LOADINTELDWORD(ptr);
#endif
		ptr += 4;
		if ((cur > fptr) && (cur < last)) {
			last = cur;
		}
	}
	return(last);
}

static BRESULT trackseek(FDDFILE fdd, REG8 media, UINT track) {

const _D88HEAD	*hdr;
	UINT32		fptr;
	UINT		size;
const _D88SEC	*sec;
	UINT		maxsectors;
	UINT		sectors;
	UINT		secsize;

	if ((fdd->inf.d88.curtrkp != NULL) &&
		(fdd->inf.d88.curmedia == media) &&
		(fdd->inf.d88.curtrk == track)) {
		return(SUCCESS);
	}

	fdd->inf.d88.curtrkp = NULL;
	fdd->inf.d88.cursectors = 0;
	hdr = fdd->inf.d88.hdr;
	if (media != (REG8)((hdr->fd_type >> 4))) {
		goto dtrd_err1;
	}
	if (track >= 164) {
		goto dtrd_err1;
	}
#if defined(BYTESEX_LITTLE)
	fptr = *(UINT32 *)(hdr->trkptr + track);
#else
	fptr = LOADINTELDWORD(hdr->trkptr + track);
#endif
	if (fptr == 0) {
		goto dtrd_err1;
	}
	size = nexttrackptr(fdd, fptr, fdd->inf.d88.fd_size) - fptr;

	sec = (D88SEC)(((UINT8 *)hdr) + fptr);
	fdd->inf.d88.curmedia = media;
	fdd->inf.d88.curtrk = track;
	fdd->inf.d88.curtrkp = sec;
	fdd->inf.d88.cursecsize = size;

	// セクタ数チェック
	maxsectors = 0;
	if (size >= sizeof(_D88SEC)) {
		maxsectors = LOADINTELWORD(sec->sectors);
	}
	sectors = 0;
	while(sectors < maxsectors) {
		secsize = LOADINTELWORD(sec->size);
		secsize += sizeof(_D88SEC);
		if (size < secsize) {
			break;
		}
		size -= secsize;
		maxsectors = LOADINTELWORD(sec->sectors);
		sec = (D88SEC)(((UINT8 *)sec) + secsize);
		sectors++;
	}
	fdd->inf.d88.cursectors = sectors;
	return(SUCCESS);

dtrd_err1:
	return(FAILURE);
}

static D88SEC sectorseek(FDDFILE fdd, REG8 r) {

const _D88SEC	*sec;
	UINT		sectors;
	UINT		size;

	sec = fdd->inf.d88.curtrkp;
	sectors = fdd->inf.d88.cursectors;
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


// ----

static REG8 fddd88_seek(FDDFILE fdd, REG8 media, UINT track) {

	if (trackseek(fdd, media, track) == SUCCESS) {
		return(0x00);
	}
	else {
		return(FDDSTAT_SEEKERR);
	}
}

static REG8 fddd88_readp(FDDFILE fdd, REG8 media, UINT track, REG8 sc,
												void **ptr, UINT *size) {

const _D88SEC	*sec;
	REG8		ret;

	TRACEOUT(("d88 read %d:%.2x", track, sc));
	if (trackseek(fdd, media, track) != SUCCESS) {
		goto fd8r_err;
	}
	sec = sectorseek(fdd, sc);
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
	if (ptr) {
		*ptr = (void *)(sec + 1);
	}
	*size = LOADINTELWORD(sec->size);
	return(ret);

fd8r_err:
	return(FDDSTAT_RECNFND);
}

static REG8 fddd88_crc(FDDFILE fdd, REG8 media, UINT track, UINT num,
												UINT8 *ptr) {

const _D88SEC	*sec;
	UINT		sectors;
	UINT		size;

	if (trackseek(fdd, media, track) != SUCCESS) {
		return(FDDSTAT_RECNFND);
	}
	sec = fdd->inf.d88.curtrkp;
	sectors = fdd->inf.d88.cursectors;
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
//	fdc.s.rreg = sec->c;								// メルヘンヴェール
	if (sec->stat) {
		return(FDDSTAT_CRCERR);
	}
	return(0x00);
}

#if defined(SUPPORT_DISKEXT)
static UINT32 fddd88_sec(FDDFILE fdd, REG8 media, UINT track, REG8 sc) {

const _D88SEC	*sec;
	UINT		sectors;
	UINT		num;
	UINT		size;

	if (trackseek(fdd, media, track) != SUCCESS) {
		return(0);
	}
	sec = fdd->inf.d88.curtrkp;
	sectors = fdd->inf.d88.cursectors;
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


// ----

BRESULT fddd88_set(FDDFILE fdd, const UINT8 *romptr, UINT romsize) {

const _D88HEAD	*hdr;
	UINT		fd_size;

	if (romsize < D88_HEADERSIZE) {
		goto fdst_err;
	}
	hdr = (D88HEAD)romptr;
#if defined(BYTESEX_LITTLE)
	fd_size = *(UINT32 *)hdr->fd_size;
#else
	fd_size = LOADINTELDWORD(hdr->fd_size);
#endif
	fd_size = min(fd_size, romsize);

	fdd->inf.d88.hdr = (D88HEAD)romptr;
	fdd->inf.d88.fd_size = fd_size;
	fdd->type = DISKTYPE_D88;
	fdd->seek = fddd88_seek;
	fdd->readp = fddd88_readp;
	fdd->crc = fddd88_crc;
#if defined(SUPPORT_DISKEXT)
	fdd->sec = fddd88_sec;
#endif
	return(SUCCESS);

fdst_err:
	return(FAILURE);
}

void fddd88_eject(FDDFILE fdd) {

	(void)fdd;
}

