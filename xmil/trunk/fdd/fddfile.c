#include	"compiler.h"
#include	"strres.h"
#include	"dosio.h"
#include	"sysmng.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"fddfile.h"
#include	"fdd_2d.h"
#include	"fdd_d88.h"


	_FDDFILE	fddfile[MAX_FDDFILE];


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
		return(DISKTYPE_BETA);
	}
	if ((!milstr_cmp(ext, str_d88)) || (!milstr_cmp(ext, str_88d))) {
		return(DISKTYPE_D88);
	}
	return(DISKTYPE_NOTREADY);
}

BRESULT fdd_set(REG8 drv, const OEMCHAR *fname, UINT ftype, int ro) {

	FDDFILE		fdd;

	if (drv >= MAX_FDDFILE) {
		return(FAILURE);
	}
	fdd_eject(drv);
	fdd = fddfile + drv;
	switch(getfdtype(fname)) {
		case DISKTYPE_NOTREADY:
			return(FAILURE);

		case DISKTYPE_BETA:
			return(fdd2d_set(fdd, drv, fname));

		case DISKTYPE_D88:
		default:
			return(fddd88_set(fdd, drv, fname));
	}
	return(FAILURE);
}

BRESULT fdd_eject(REG8 drv) {

	FDDFILE		fdd;

	if (drv >= MAX_FDDFILE) {
		return(FAILURE);
	}
	fdd = fddfile + drv;
	switch(fdd->type) {
		case DISKTYPE_NOTREADY:
			return(FAILURE);

		case DISKTYPE_BETA:
			return(fdd2d_eject(fdd, drv));

		case DISKTYPE_D88:
		default:
			return(fddd88_eject(fdd, drv));
	}
	return(FAILURE);
}

