#include	"compiler.h"
#include	"strres.h"
#include	"dosio.h"
#include	"sysmng.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"fddfile.h"
#include	"x1_io.h"
#include	"x1_fdc.h"
#include	"fdd_2d.h"
#include	"fdd_d88.h"


	_FDDFILE	fddfile[MAX_FDDFILE];

extern	BYTE		WRITEPT[];
extern	BYTE		DISKNUM[];


void fddfile_initialize(void) {
}

const OEMCHAR *fdd_diskname(REG8 drv) {

	if (drv >= MAX_FDDFILE) {
		return(str_null);
	}
	return(fddfile[drv].fname);
}

BRESULT fdd_diskready(REG8 drv) {

	if ((drv >= MAX_FDDFILE) || (fddfile[drv].fname[0] == '\0')) {
		return(FALSE);
	}
	return(TRUE);
}


// ----

static REG8 getfdtype(const OEMCHAR *fname) {

const OEMCHAR	*ext;

	ext = file_getext(fname);
	if (!milstr_cmp(ext, str_e2d)) {
		return(DRV_FMT2D);
	}
	if ((!milstr_cmp(ext, str_d88)) || (!milstr_cmp(ext, str_88d))) {
		return(DRV_FMT88);
	}
	return(DRV_EMPTY);
}

BRESULT fdd_set(REG8 drv, const OEMCHAR *fname, UINT ftype, int ro) {

	fdd_eject(drv);
	if (drv >= MAX_FDDFILE) {
		return(FAILURE);
	}
	switch(getfdtype(fname)) {
		case DRV_EMPTY:
			return(FAILURE);

		case DRV_FMT2D:
			return(fdd2d_set(drv, fname));

		default:
			return(fddd88_set(drv, fname));
	}
	return(FAILURE);
}

BRESULT fdd_eject(REG8 drv) {

	FDDFILE		fdd;

	if (drv >= MAX_FDDFILE) {
		return(FAILURE);
	}
	fdd = fddfile + drv;
	switch(DISKNUM[drv]) {
		case DRV_EMPTY:
			return(FAILURE);

		case DRV_FMT2D:
			return(fdd2d_eject(drv));

		default:
			return(fddd88_eject(drv));
	}
	return(FAILURE);
}

