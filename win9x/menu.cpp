#include	"compiler.h"
#include	"resource.h"
#include	"xmil.h"
#include	"dosio.h"
#include	"scrnmng.h"
#include	"sysmng.h"
#include	"pccore.h"
#include	"ini.h"
#include	"menu.h"
#include	"palettes.h"
#include	"makescrn.h"
#include	"fdd_mtr.h"
#include	"fdd_ini.h"


#define	MFCHECK(a) ((a)?MF_CHECKED:MF_UNCHECKED)

typedef struct {
	UINT16	id;
	UINT16	str;
} MENUITEMS;


static void insertresmenu(HMENU menu, UINT pos, UINT flag,
													UINT item, UINT str) {

	OEMCHAR	tmp[128];

	if (LoadString(hInst, str, tmp, NELEMENTS(tmp))) {
		InsertMenu(menu, pos, flag, item, tmp);
	}
}

static void insertresmenus(HMENU menu, UINT pos,
									const MENUITEMS *item, UINT items) {

const MENUITEMS *iterm;

	iterm = item + items;
	while(item < iterm) {
		if (item->id) {
			insertresmenu(menu, pos, MF_BYPOSITION | MF_STRING,
													item->id, item->str);
		}
		else {
			InsertMenu(menu, pos, MF_BYPOSITION | MF_SEPARATOR, 0, NULL);
		}
		item++;
		pos++;
	}
}


// ----

static const MENUITEMS smenuitem[] = {
			{IDM_SCREENCENTER,	IDS_SCREENCENTER},
			{IDM_SNAPENABLE,	IDS_SNAPENABLE},
			{IDM_BACKGROUND,	IDS_BACKGROUND},
			{IDM_BGSOUND,		IDS_BGSOUND},
			{0,					0}};

void sysmenu_initialize(void) {

	HMENU	hMenu;

	hMenu = GetSystemMenu(hWndMain, FALSE);
	insertresmenus(hMenu, 0, smenuitem, NELEMENTS(smenuitem));
}

void sysmenu_setwinsnap(UINT8 value) {

	value &= 1;
	xmiloscfg.WINSNAP = value;
	CheckMenuItem(GetSystemMenu(hWndMain, FALSE),
									IDM_SNAPENABLE, MFCHECK(value));
}

void sysmenu_setbackground(UINT8 value) {

	HMENU	hmenu;

	hmenu = GetSystemMenu(hWndMain, FALSE);
	xmiloscfg.background &= 2;
	xmiloscfg.background |= (value & 1);
	if (value & 1) {
		CheckMenuItem(hmenu, IDM_BACKGROUND, MF_UNCHECKED);
		EnableMenuItem(hmenu, IDM_BGSOUND, MF_GRAYED);
	}
	else {
		CheckMenuItem(hmenu, IDM_BACKGROUND, MF_CHECKED);
		EnableMenuItem(hmenu, IDM_BGSOUND, MF_ENABLED);
	}
}

void sysmenu_setbgsound(UINT8 value) {

	xmiloscfg.background &= 1;
	xmiloscfg.background |= (value & 2);
	CheckMenuItem(GetSystemMenu(hWndMain, FALSE),
									IDM_BGSOUND, MFCHECK((value & 2) ^ 2));
}


// ----

void xmenu_disablewindow(void) {

	HMENU	hmenu;

	hmenu = GetMenu(hWndMain);
	EnableMenuItem(hmenu, IDM_WINDOW, MF_GRAYED);
	EnableMenuItem(hmenu, IDM_FULLSCREEN, MF_GRAYED);
}

void xmenu_setiplrom(UINT8 num) {

	HMENU	hmenu;

	hmenu = GetMenu(hWndMain);
	xmilcfg.ROM_TYPE = num;
	CheckMenuItem(hmenu, IDM_TURBOZ, MFCHECK(num == 3));
	CheckMenuItem(hmenu, IDM_TURBO, MFCHECK(num == 2));
	CheckMenuItem(hmenu, IDM_X1ROM, MFCHECK(num == 1));
}

void xmenu_setbootmedia(UINT8 value) {

	HMENU	hmenu;

	hmenu = GetMenu(hWndMain);
	value &= DIPSW_BOOTMEDIA;
	xmilcfg.DIP_SW &= ~DIPSW_BOOTMEDIA;
	xmilcfg.DIP_SW |= value;
	CheckMenuItem(hmenu, IDM_BOOT2D, MFCHECK(!value));
	CheckMenuItem(hmenu, IDM_BOOT2HD, MFCHECK(value));
}

void xmenu_setwidth(UINT8 value) {

	HMENU	hmenu;

	hmenu = GetMenu(hWndMain);
	CheckMenuItem(hmenu, IDM_WIDTH80, MFCHECK(!value));
	CheckMenuItem(hmenu, IDM_WIDTH40, MFCHECK(value));
}

void xmenu_setresolute(UINT8 value) {

	HMENU	hmenu;

	hmenu = GetMenu(hWndMain);
	value &= DIPSW_RESOLUTE;
	xmilcfg.DIP_SW &= ~DIPSW_RESOLUTE;
	xmilcfg.DIP_SW |= value;
	CheckMenuItem(hmenu, IDM_HIGHRES, MFCHECK(!value));
	CheckMenuItem(hmenu, IDM_LOWRES, MFCHECK(value));
}

void xmenu_setdispmode(UINT8 value) {

	value &= 1;
	xmilcfg.DISPSYNC = value;
	CheckMenuItem(GetMenu(hWndMain), IDM_DISPSYNC, MFCHECK(value));
}

void xmenu_setwaitflg(UINT8 value) {

	value &= 1;
	xmiloscfg.NOWAIT = value;
	CheckMenuItem(GetMenu(hWndMain), IDM_NOWAIT, MFCHECK(value));
}

void xmenu_setframe(UINT8 value) {

	HMENU	hmenu;

	hmenu = GetMenu(hWndMain);
	xmiloscfg.DRAW_SKIP = value;
	CheckMenuItem(hmenu, IDM_AUTOFPS, MFCHECK(!value));
	CheckMenuItem(hmenu, IDM_60FPS, MFCHECK(value == 1));
	CheckMenuItem(hmenu, IDM_30FPS, MFCHECK(value == 2));
	CheckMenuItem(hmenu, IDM_20FPS, MFCHECK(value == 3));
	CheckMenuItem(hmenu, IDM_15FPS, MFCHECK(value == 4));
}

void xmenu_setkey(UINT8 value) {

	HMENU	hmenu;

	hmenu = GetMenu(hWndMain);
	if (value >= 3) {
		value = 0;
	}
	xmilcfg.KEY_MODE = value;
	CheckMenuItem(hmenu, IDM_KEY, MFCHECK(value == 0));
	CheckMenuItem(hmenu, IDM_JOY1, MFCHECK(value == 1));
	CheckMenuItem(hmenu, IDM_JOY2, MFCHECK(value == 2));
}

void xmenu_setsound(UINT8 value) {

	xmilcfg.SOUND_SW = value;
	CheckMenuItem(GetMenu(hWndMain), IDM_FMBOARD, MFCHECK(value));
}

void xmenu_setjoystick(UINT8 value) {

	HMENU	hmenu;

	hmenu = GetMenu(hWndMain);
	xmiloscfg.JOYSTICK &= 0x80;
	xmiloscfg.JOYSTICK |= (value & 0x7f);
	if (value & 2) {
		EnableMenuItem(hmenu, IDM_JOYSTICK, MF_GRAYED);
	}
	else {
		CheckMenuItem(hmenu, IDM_JOYSTICK, MFCHECK(value & 1));
	}
}

void xmenu_setmouse(UINT8 value) {

	value &= 1;
	xmilcfg.MOUSE_SW = value;
	CheckMenuItem(GetMenu(hWndMain), IDM_MOUSE, MFCHECK(value));
}

void xmenu_setcpuspeed(UINT8 value) {

	value &= 1;
	xmilcfg.CPU8MHz = value;
	CheckMenuItem(GetMenu(hWndMain), IDM_8MHZ, MFCHECK(value));
}

void xmenu_setmotorflg(UINT8 value) {

	value &= 1;
	xmilcfg.MOTOR = value;
	CheckMenuItem(GetMenu(hWndMain), IDM_SEEKSND, MFCHECK(value));
}

void xmenu_opmlog(UINT8 value) {

	CheckMenuItem(GetMenu(hWndMain), IDM_OPMLOG, MFCHECK(value));
}

void xmenu_setdispclk(UINT8 value) {

	HMENU	hmenu;

	hmenu = GetMenu(hWndMain);
	value &= 3;
	xmiloscfg.DISPCLK = value;
	CheckMenuItem(hmenu, IDM_DISPCLOCK, MFCHECK(value & 1));
	CheckMenuItem(hmenu, IDM_DISPFRAME, MFCHECK(value & 2));
	sysmng_workclockrenewal();
	sysmng_updatecaption(3);
}

void xmenu_setskipline(UINT8 value) {

	value &= 1;
	xmilcfg.SKIP_LINE = value;
	CheckMenuItem(GetMenu(hWndMain), IDM_SKIPLINE, MFCHECK(value));
	pal_reset();
	makescrn.palandply = 1;
}

void xmenu_setbtnmode(UINT8 value) {

	value &= 1;
	xmilcfg.BTN_MODE = value;
	CheckMenuItem(GetMenu(hWndMain), IDM_JOYX, MFCHECK(value));
}

void xmenu_setbtnrapid(UINT8 value) {

	value &= 1;
	xmilcfg.BTN_RAPID = value;
	CheckMenuItem(GetMenu(hWndMain), IDM_RAPID, MFCHECK(value));
}

void xmenu_setz80save(UINT8 value) {

	if (!value) {
		EnableMenuItem(GetMenu(hWndMain), IDM_Z80SAVE, MF_GRAYED);
	}
}

