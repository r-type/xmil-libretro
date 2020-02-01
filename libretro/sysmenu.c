#include	"compiler.h"
#include	"strres.h"
#include	"xmil.h"
#include	"fontmng.h"
#include	"scrnmng.h"
#include	"sysmng.h"
#include	"taskmng.h"
#include	"z80core.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"keystat.h"
#include	"diskdrv.h"
#include	"sysmenu.h"
#include "embed/menu/dlgcfg.h"
#include "embed/menu/dlgabout.h"
#include "embed/menu/filesel.h"
#include "embed/menu/menustr.h"
#include "embed/menubase/menudlg.h"
#include "embed/menubase/menuicon.h"
#include "embed/menubase/menusys.h"
#include "sysmenu.res"
#include "sysmenu.str"

static void sys_cmd(MENUID id) {

	UINT	update;

	update = 0;
	switch(id) {
		case MID_IPLRESET:
			pccore_reset();
			break;

		case MID_NMIRESET:
			Z80_NMI();
			break;

		case MID_FDD0OPEN:
			filesel_fdd(0);
			break;

		case MID_FDD0EJECT:
			diskdrv_setfdd(0, NULL, 0);
			break;

		case MID_FDD1OPEN:
			filesel_fdd(1);
			break;

		case MID_FDD1EJECT:
			diskdrv_setfdd(1, NULL, 0);
			break;

		case MID_ABOUT:
			menudlg_create(DLGABOUT_WIDTH, DLGABOUT_HEIGHT,
													mstr_about, dlgabout_cmd);
			break;

#if defined(MENU_TASKMINIMIZE)
		case SID_MINIMIZE:
			taskmng_minimize();
			break;
#endif
		case MID_EXIT:
		case SID_CLOSE:
			taskmng_exit();
			break;
	}
	sysmng_update(update);
}


// ----

BRESULT sysmenu_create(void) {

	if (menubase_create() != SUCCESS) {
		goto smcre_err;
	}
	menuicon_regist(MICON_XMIL, &xmilicon);
	if (menusys_create(s_main, sys_cmd, MICON_XMIL, str_xmil)) {
		goto smcre_err;
	}
#if defined(SUPPORT_SOFTKBD)
	menusys_setstyle(MENUSTYLE_BOTTOM);
#endif
	return(SUCCESS);

smcre_err:
	return(FAILURE);
}

void sysmenu_destroy(void) {

	menubase_close();
	menubase_destroy();
	menusys_destroy();
}

BRESULT sysmenu_menuopen(UINT menutype, int x, int y) {

	return(menusys_open(x, y));
}

