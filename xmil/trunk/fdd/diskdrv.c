// �t���b�s�[�f�B�X�N�̑}�����ؗp�̃u���b�a

#include	"compiler.h"
#include	"sysmng.h"
#include	"diskdrv.h"
#include	"fddfile.h"


void diskdrv_setfdd(REG8 drv, const OEMCHAR *fname, int readonly) {

	fddfile_eject(drv);
	if (fname) {
		fddfile_set(drv, fname, 0, readonly);
	}
	sysmng_update(SYS_UPDATEFDD);
}

void diskdrv_setfddex(REG8 drv, const OEMCHAR *fname,
												UINT ftype, int readonly) {

	fddfile_eject(drv);
	if (fname) {
		fddfile_set(drv, fname, ftype, readonly);
	}
	sysmng_update(SYS_UPDATEFDD);
}

// void diskdrv_callback(void) { }

