#include	"compiler.h"
#include	"dosio.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"fddfile.h"
#include	"fdd_2d.h"
#include	"fdd_mtr.h"


static REG8 fdd2d_seek(FDDFILE fdd, REG8 media, UINT track) {

	if ((media != 0) || (track >= 80)) {
		return(FDDSTAT_SEEKERR);
	}
	(void)fdd;
	return(0x00);
}

static REG8 fdd2d_read(FDDFILE fdd, REG8 media, UINT track, REG8 sc,
												UINT8 *ptr, UINT *size) {

	UINT	secsize;
	FILEH	fh;
	long	pos;
	BRESULT	b;

	if ((media != 0) || (track >= 80) || (sc == 0) || (sc > 16)) {
		goto fd2r_err;
	}
	secsize = min(*size, 256);
	if (ptr) {
		fh = file_open_rb(fdd->fname);
		if (fh == FILEH_INVALID) {
			goto fd2r_err;
		}
		pos = ((track * 16) + (sc - 1)) * 256;
		b = (file_seek(fh, pos, FSEEK_SET) != pos) ||
			(file_read(fh, ptr, secsize) != secsize);
		file_close(fh);
		if (b) {
			goto fd2r_err;
		}
	}
	*size = secsize;
	return(0x00);

fd2r_err:
	return(FDDSTAT_RECNFND);
}

static REG8 fdd2d_write(FDDFILE fdd, REG8 media, UINT track, REG8 sc,
												const UINT8 *ptr, UINT size) {

	FILEH	fh;
	long	pos;
	BRESULT	b;

	if ((media != 0) || (track >= 80) || (sc == 0) || (sc > 16)) {
		return(FDDSTAT_RECNFND | FDDSTAT_WRITEFAULT);
	}
	fh = file_open(fdd->fname);
	if (fh == FILEH_INVALID) {
		goto fd2w_err;
	}
	pos = ((track * 16) + (sc - 1)) * 256;
	size = min(size, 256);
	b = (file_seek(fh, pos, FSEEK_SET) != pos) ||
		(file_write(fh, ptr, size) != size);
	file_close(fh);
	if (b) {
		goto fd2w_err;
	}
	return(0x00);

fd2w_err:
	return(FDDSTAT_WRITEFAULT);
}

static REG8 fdd2d_crc(FDDFILE fdd, REG8 media, UINT track, UINT num,
												UINT8 *ptr) {

	if ((media != 0) || (track >= 80) || (num >= 16)) {
		return(FDDSTAT_RECNFND);
	}
	ptr[0] = (UINT8)(track >> 1);
	ptr[1] = (UINT8)(track & 1);
	ptr[2] = (UINT8)(num + 1);
	ptr[3] = 1;
	ptr[4] = 0;										// CRC(Lo)
	ptr[5] = 0;										// CRC(Hi)
//	fdc.s.rreg = fdc.s.c;							// ƒƒ‹ƒwƒ“ƒ”ƒF[ƒ‹
	(void)fdd;
	return(0x00);
}


#if defined(SUPPORT_DISKEXT)
static UINT32 fdd2d_sec(FDDFILE fdd, REG8 media, UINT track, REG8 sc) {

	UINT32	ret;

	if ((media != 0) || (track >= 80)) {
		return(0);
	}
	if ((sc == 0) || (sc > 16)) {
		ret = 16;
	}
	else {
		ret = sc;
	}
	return((16 << 16) + ret);
}
#endif


// ----

BRESULT fdd2d_set(FDDFILE fdd, const OEMCHAR *fname) {

	short	attr;

	attr = file_attr(fname);
	if (attr & 0x18) {
		return(FAILURE);
	}
	fdd->type = DISKTYPE_BETA;
	fdd->protect = (UINT8)(attr & 1);
	fdd->seek = fdd2d_seek;
	fdd->read = fdd2d_read;
	fdd->write = fdd2d_write;
	fdd->crc = fdd2d_crc;
#if defined(SUPPORT_DISKEXT)
	fdd->sec = fdd2d_sec;
#endif
	return(SUCCESS);
}

void fdd2d_eject(FDDFILE fdd) {

	(void)fdd;
}

