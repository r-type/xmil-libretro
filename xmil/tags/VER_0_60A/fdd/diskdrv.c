/* フロッピーディスクの挿入延滞用のブリッヂ */

#include	"compiler.h"
#include	"sysmng.h"
#include	"diskdrv.h"
#include	"fddfile.h"


void diskdrv_setfddex(REG8 drv, const OEMCHAR *fname,
												UINT32 ftype, int readonly) {

	fddfile_eject(drv);
	if (fname) {
		fddfile_set(drv, fname, ftype, readonly);
	}
	sysmng_update(SYS_UPDATEFDD);
}

#if 0
void diskdrv_callback(void) { }
#endif	/* 0 */
