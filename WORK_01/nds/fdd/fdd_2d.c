#include	"compiler.h"
#include	"pccore.h"
#include	"fddfile.h"
#include	"fdd_2d.h"


static const _XDFINFO supportxdf[] = {
			{0,  80, 16, 1, DISKTYPE_2D},
			{0, 154, 26, 1, DISKTYPE_2HD}};


static REG8 fdd2d_seek(FDDFILE fdd, REG8 media, UINT track) {

	if ((media != fdd->inf.xdf.media) || (track >= fdd->inf.xdf.tracks)) {
		return(FDDSTAT_SEEKERR);
	}
	return(0x00);
}

static REG8 fdd2d_readp(FDDFILE fdd, REG8 media, UINT track, REG8 sc,
												void **ptr, UINT *size) {

	UINT	secsize;
	long	pos;

	if ((media != fdd->inf.xdf.media) || (track >= fdd->inf.xdf.tracks) ||
		(sc == 0) || (sc > fdd->inf.xdf.sectors)) {
		goto fd2r_err;
	}
	secsize = 1 << (7 + fdd->inf.xdf.n);
	*size = secsize;
	if (ptr) {
		pos = ((track * fdd->inf.xdf.sectors) + (sc - 1)) * secsize;
		pos += fdd->inf.xdf.headersize;
		*ptr = (void *)(fdd->romptr + pos);
	}
	return(0x00);

fd2r_err:
	return(FDDSTAT_RECNFND);
}

static REG8 fdd2d_crc(FDDFILE fdd, REG8 media, UINT track, UINT num,
												UINT8 *ptr) {

	if ((media != fdd->inf.xdf.media) || (track >= fdd->inf.xdf.tracks) ||
		(num >= fdd->inf.xdf.sectors)) {
		return(FDDSTAT_RECNFND);
	}
	ptr[0] = (UINT8)(track >> 1);
	ptr[1] = (UINT8)(track & 1);
	ptr[2] = (UINT8)(num + 1);
	ptr[3] = fdd->inf.xdf.n;
	ptr[4] = 0;										// CRC(Lo)
	ptr[5] = 0;										// CRC(Hi)
	return(0x00);
}


#if defined(SUPPORT_DISKEXT)
static UINT32 fdd2d_sec(FDDFILE fdd, REG8 media, UINT track, REG8 sc) {

	UINT32	ret;

	if ((media != fdd->inf.xdf.media) || (track >= fdd->inf.xdf.tracks)) {
		return(0);
	}
	if ((sc == 0) || (sc > fdd->inf.xdf.sectors)) {
		ret = fdd->inf.xdf.sectors;
	}
	else {
		ret = sc;
	}
	return((16 << fdd->inf.xdf.sectors) + ret);
}
#endif


// ----

BRESULT fdd2d_set(FDDFILE fdd, const UINT8 *romptr, UINT romsize) {

const _XDFINFO	*xdf;
const _XDFINFO	*xdfterm;
	UINT		size;

	xdf = supportxdf;
	xdfterm = supportxdf + NELEMENTS(supportxdf);
	while(xdf < xdfterm) {
		size = xdf->tracks;
		size *= xdf->sectors;
		size <<= (7 + xdf->n);
		if (size == romsize) {
			fdd->type = DISKTYPE_BETA;
			fdd->seek = fdd2d_seek;
			fdd->readp = fdd2d_readp;
			fdd->crc = fdd2d_crc;
#if defined(SUPPORT_DISKEXT)
			fdd->sec = fdd2d_sec;
#endif
			fdd->inf.xdf = *xdf;
			return(SUCCESS);
		}
		xdf++;
	}
	return(FAILURE);
}

void fdd2d_eject(FDDFILE fdd) {

	(void)fdd;
}

