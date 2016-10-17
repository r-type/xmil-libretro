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

		case MID_CONFIG:
			menudlg_create(DLGCFG_WIDTH, DLGCFG_HEIGHT, mstr_cfg, dlgcfg_cmd);
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

		case MID_X1ROM:
			xmilcfg.ROM_TYPE = 1;
			update = SYS_UPDATECFG;
			break;

		case MID_TURBO:
			xmilcfg.ROM_TYPE = 2;
			update = SYS_UPDATECFG;
			break;

#if defined(SUPPORT_TURBOZ)
		case MID_TURBOZ:
			xmilcfg.ROM_TYPE = 3;
			update = SYS_UPDATECFG;
			break;
#endif

		case MID_BOOT2D:
			xmilcfg.DIP_SW &= ~DIPSW_BOOTMEDIA;
			update = SYS_UPDATECFG;
			break;

		case MID_BOOT2HD:
			xmilcfg.DIP_SW |= DIPSW_BOOTMEDIA;
			update = SYS_UPDATECFG;
			break;

		case MID_HIGHRES:
			xmilcfg.DIP_SW &= ~DIPSW_RESOLUTE;
			update = SYS_UPDATECFG;
			break;

		case MID_LOWRES:
			xmilcfg.DIP_SW |= DIPSW_RESOLUTE;
			update = SYS_UPDATECFG;
			break;

		case MID_DISPSYNC:
			xmilcfg.DISPSYNC ^= 1;
			update |= SYS_UPDATECFG;
			break;

		case MID_RASTER:
			xmilcfg.RASTER ^= 1;
			update |= SYS_UPDATECFG;
			break;

		case MID_NOWAIT:
			xmiloscfg.NOWAIT ^= 1;
			update |= SYS_UPDATECFG;
			break;

		case MID_AUTOFPS:
			xmiloscfg.DRAW_SKIP = 0;
			update |= SYS_UPDATECFG;
			break;

		case MID_60FPS:
			xmiloscfg.DRAW_SKIP = 1;
			update |= SYS_UPDATECFG;
			break;

		case MID_30FPS:
			xmiloscfg.DRAW_SKIP = 2;
			update |= SYS_UPDATECFG;
			break;

		case MID_20FPS:
			xmiloscfg.DRAW_SKIP = 3;
			update |= SYS_UPDATECFG;
			break;

		case MID_15FPS:
			xmiloscfg.DRAW_SKIP = 4;
			update |= SYS_UPDATECFG;
			break;

		case MID_KEY:
			xmilcfg.KEY_MODE = 0;
			keystat_resetjoykey();
			update |= SYS_UPDATECFG;
			break;

		case MID_JOY1:
			xmilcfg.KEY_MODE = 1;
			keystat_resetjoykey();
			update |= SYS_UPDATECFG;
			break;

		case MID_JOY2:
			xmilcfg.KEY_MODE = 2;
			keystat_resetjoykey();
			update |= SYS_UPDATECFG;
			break;

#if 0
		case MID_MOUSEKEY:
			xmilcfg.KEY_MODE = 3;
			keystat_resetjoykey();
			update |= SYS_UPDATECFG;
			break;
#endif

#if defined(SUPPORT_TURBOZ) || defined(SUPPORT_OPM)
		case MID_FMBOARD:
			xmilcfg.SOUND_SW ^= 1;
			update = SYS_UPDATECFG;
			break;
#endif	/* defined(SUPPORT_TURBOZ) || defined(SUPPORT_OPM) */

#if !defined(DISABLE_SOUND)
		case MID_SEEKSND:
			xmilcfg.MOTOR ^= 1;
			update |= SYS_UPDATECFG;
			break;
#endif	/* !defined(DISABLE_SOUND) */

		case MID_JOYX:
			xmilcfg.BTN_MODE ^= 1;
			update |= SYS_UPDATECFG;
			break;

		case MID_RAPID:
			xmilcfg.BTN_RAPID ^= 1;
			update |= SYS_UPDATECFG;
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

	UINT8	b;

	b = xmilcfg.ROM_TYPE;
	menusys_setcheck(MID_X1ROM, (b == 1));
	menusys_setcheck(MID_TURBO, (b == 2));
	menusys_setcheck(MID_TURBOZ, (b == 3));
	b = xmilcfg.DIP_SW & DIPSW_BOOTMEDIA;
	menusys_setcheck(MID_BOOT2D, (b == 0));
	menusys_setcheck(MID_BOOT2HD, (b != 0));
	b = xmilcfg.DIP_SW & DIPSW_RESOLUTE;
	menusys_setcheck(MID_HIGHRES, (b == 0));
	menusys_setcheck(MID_LOWRES, (b != 0));
	menusys_setcheck(MID_DISPSYNC, (xmilcfg.DISPSYNC & 1));
	menusys_setcheck(MID_RASTER, (xmilcfg.RASTER & 1));
	menusys_setcheck(MID_NOWAIT, (xmiloscfg.NOWAIT & 1));
	b = xmiloscfg.DRAW_SKIP;
	menusys_setcheck(MID_AUTOFPS, (b == 0));
	menusys_setcheck(MID_60FPS, (b == 1));
	menusys_setcheck(MID_30FPS, (b == 2));
	menusys_setcheck(MID_20FPS, (b == 3));
	menusys_setcheck(MID_15FPS, (b == 4));
	b = xmilcfg.KEY_MODE;
	menusys_setcheck(MID_KEY, (b == 0));
	menusys_setcheck(MID_JOY1, (b == 1));
	menusys_setcheck(MID_JOY2, (b == 2));
	menusys_setcheck(MID_MOUSEKEY, (b == 3));
#if defined(SUPPORT_TURBOZ) || defined(SUPPORT_OPM)
	menusys_setcheck(MID_FMBOARD, (xmilcfg.SOUND_SW & 1));
#endif	/* defined(SUPPORT_TURBOZ) || defined(SUPPORT_OPM) */
#if !defined(DISABLE_SOUND)
	menusys_setcheck(MID_SEEKSND, (xmilcfg.MOTOR & 1));
#endif	/* !defined(DISABLE_SOUND) */
	menusys_setcheck(MID_JOYX, (xmilcfg.BTN_MODE & 1));
	menusys_setcheck(MID_RAPID, (xmilcfg.BTN_RAPID & 1));
	return(menusys_open(x, y));
}

