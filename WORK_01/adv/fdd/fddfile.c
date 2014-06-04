#include	"compiler.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"fddfile.h"
#include	"fdd_2d.h"
#include	"fdd_d88.h"


	_FDDFILE	fddfile[MAX_FDDFILE];


static REG8 dummyseek(FDDFILE fdd, REG8 media, UINT track) {

	(void)fdd;
	(void)media;
	(void)track;
	return(FDDSTAT_SEEKERR);
}

static REG8 dummyreadp(FDDFILE fdd, REG8 media, UINT track, REG8 sc,
												void **ptr, UINT *size) {

	(void)fdd;
	(void)media;
	(void)track;
	(void)sc;
	(void)ptr;
	(void)size;
	return(FDDSTAT_RECNFND);
}

static REG8 dummywrite(FDDFILE fdd, REG8 media, UINT track, REG8 sc,
												const UINT8 *ptr, UINT size) {

	(void)fdd;
	(void)media;
	(void)track;
	(void)sc;
	(void)ptr;
	(void)size;
	return(FDDSTAT_RECNFND | FDDSTAT_WRITEFAULT);
}

static REG8 dummycrc(FDDFILE fdd, REG8 media, UINT track, UINT num,
												UINT8 *ptr) {

	(void)fdd;
	(void)media;
	(void)track;
	(void)num;
	(void)ptr;
	return(FDDSTAT_RECNFND);
}

static void setempty(FDDFILE fdd) {

	ZeroMemory(fdd, sizeof(_FDDFILE));
	fdd->seek = dummyseek;
	fdd->readp = dummyreadp;
	fdd->write = dummywrite;
	fdd->crc = dummycrc;
}


// ----

void fddfile_initialize(void) {

	UINT	i;

	for (i=0; i<MAX_FDDFILE; i++) {
		setempty(fddfile + i);
	}
}

BRESULT fddfile_diskready(REG8 drv) {

	if ((drv >= MAX_FDDFILE) || (fddfile[drv].type == DISKTYPE_NOTREADY)) {
		return(FALSE);
	}
	return(TRUE);
}

BRESULT fddfile_diskprotect(REG8 drv) {

	if ((drv >= MAX_FDDFILE) || (!fddfile[drv].protect)) {
		return(FALSE);
	}
	return(TRUE);
}

BRESULT fddfile_set(REG8 drv, UINT ftype, const UINT8 *romptr, UINT romsize) {

	FDDFILE	fdd;
	BRESULT	r;

	if (drv >= MAX_FDDFILE) {
		return(FAILURE);
	}
	fddfile_eject(drv);
	fdd = fddfile + drv;
	switch(ftype) {
		case FTYPE_BETA:
			r = fdd2d_set(fdd, romptr, romsize);
			break;

		case FTYPE_D88:
			r = fddd88_set(fdd, romptr, romsize);
			break;

		default:
			r = FAILURE;
			break;
	}
	if (r == SUCCESS) {
		fdd->romptr = romptr;
		fdd->romsize = romsize;
		fdd->ftype = ftype;
		fdd->protect = TRUE;
	}
	return(r);
}

void fddfile_eject(REG8 drv) {

	FDDFILE	fdd;

	if (drv >= MAX_FDDFILE) {
		return;
	}
	fdd = fddfile + drv;
	switch(fdd->type) {
		case DISKTYPE_BETA:
			fdd2d_eject(fdd);

		case DISKTYPE_D88:
			fddd88_eject(fdd);
	}
	setempty(fdd);
}

