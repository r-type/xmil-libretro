#include	"compiler.h"
#include	"resource.h"
#include	"xmil.h"
#include	"dosio.h"
#include	"scrnmng.h"
#include	"sysmng.h"
#include	"extclass.h"
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

typedef struct {
	UINT16		title;
	UINT16		items;
const MENUITEMS	*item;
} SUBMENUI;

static const MENUITEMS fddsitem[4][3] = {
		{	{IDM_FDD0OPEN,		IDS_OPEN},
			{0,					0},
			{IDM_FDD0EJECT,		IDS_EJECT}},
		{	{IDM_FDD1OPEN,		IDS_OPEN},
			{0,					0},
			{IDM_FDD1EJECT,		IDS_EJECT}},
		{	{IDM_FDD2OPEN,		IDS_OPEN},
			{0,					0},
			{IDM_FDD2EJECT,		IDS_EJECT}},
		{	{IDM_FDD3OPEN,		IDS_OPEN},
			{0,					0},
			{IDM_FDD3EJECT,		IDS_EJECT}}};

static const SUBMENUI fddmenu[4] = {
					{IDS_FDD0, 3, fddsitem[0]},
					{IDS_FDD1, 3, fddsitem[1]},
					{IDS_FDD2, 3, fddsitem[2]},
					{IDS_FDD3, 3, fddsitem[3]}};

static const MENUITEMS dbgsitem[4] = {
			{IDM_WIDTH40,		IDS_WIDTH40},
			{IDM_WIDTH80,		IDS_WIDTH80},
			{0,					0},
			{IDM_Z80SAVE,		IDS_Z80SAVE}};

static const SUBMENUI dbgmenu = {IDS_DEBUG, 4, dbgsitem};


static void insertsubmenu(HMENU hMenu, UINT pos, const SUBMENUI *sm) {

	HMENU	hSubMenu;

	hSubMenu = CreatePopupMenu();
	insertresmenus(hSubMenu, 0, sm->item, sm->items);
	insertresmenu(hMenu, pos, MF_BYPOSITION | MF_POPUP,
											(UINT)hSubMenu, sm->title);
}

#if defined(SUPPORT_STATSAVE)
static const OEMCHAR xmenu_stat[] = OEMTEXT("S&tat");
static const OEMCHAR xmenu_statsave[] = OEMTEXT("Save %u");
static const OEMCHAR xmenu_statload[] = OEMTEXT("Load %u");

static void addstatsavemenu(HMENU hMenu, UINT pos) {

	HMENU	hSubMenu;
	UINT	i;
	OEMCHAR	buf[16];

	hSubMenu = CreatePopupMenu();
	for (i=0; i<SUPPORT_STATSAVE; i++) {
		OEMSPRINTF(buf, xmenu_statsave, i);
		AppendMenu(hSubMenu, MF_STRING, IDM_FLAGSAVE + i, buf);
	}
	AppendMenu(hSubMenu, MF_MENUBARBREAK, 0, NULL);
	for (i=0; i<SUPPORT_STATSAVE; i++) {
		OEMSPRINTF(buf, xmenu_statload, i);
		AppendMenu(hSubMenu, MF_STRING, IDM_FLAGLOAD + i, buf);
	}
	InsertMenu(hMenu, pos, MF_BYPOSITION | MF_POPUP,
											(UINT32)hSubMenu, xmenu_stat);
}
#endif

void menu_initialize(void) {

	HMENU	hMenu;
#if defined(SUPPORT_WAVEREC) || defined(SUPPORT_TURBOZ)
	HMENU	hSubMenu;
#endif
	UINT	i;

	hMenu = GetMenu(hWndMain);
#if defined(SUPPORT_WAVEREC)
	hSubMenu = GetSubMenu(hMenu, 5);
	insertresmenu(hSubMenu, 2, MF_BYPOSITION | MF_STRING,
												IDM_WAVEREC, IDS_WAVEREC);
#endif

	if (xmiloscfg.Z80SAVE) {
		insertsubmenu(hMenu, 5, &dbgmenu);
	}

#if defined(SUPPORT_TURBOZ)
	hSubMenu = GetSubMenu(hMenu, 1);
	insertresmenu(hSubMenu, 2, MF_BYPOSITION | MF_STRING,
												IDM_TURBOZ, IDS_TURBOZ);
#endif

	for (i=4; i--;) {
		if (xmilcfg.fddequip & (1 << i)) {
			insertsubmenu(hMenu, 1, fddmenu + i);
		}
	}

#if defined(SUPPORT_STATSAVE)
	if (xmiloscfg.statsave) {
		addstatsavemenu(hMenu, 1);
	}
#endif
}

void menu_disablewindow(void) {

	HMENU	hmenu;

	hmenu = extclass_gethmenu(hWndMain);
	EnableMenuItem(hmenu, IDM_WINDOW, MF_GRAYED);
	EnableMenuItem(hmenu, IDM_FULLSCREEN, MF_GRAYED);
}

void menu_setiplrom(UINT8 num) {

	HMENU	hmenu;

	hmenu = extclass_gethmenu(hWndMain);
	xmilcfg.ROM_TYPE = num;
	CheckMenuItem(hmenu, IDM_TURBOZ, MFCHECK(num == 3));
	CheckMenuItem(hmenu, IDM_TURBO, MFCHECK(num == 2));
	CheckMenuItem(hmenu, IDM_X1ROM, MFCHECK(num == 1));
}

void menu_setbootmedia(UINT8 value) {

	HMENU	hmenu;

	hmenu = extclass_gethmenu(hWndMain);
	value &= DIPSW_BOOTMEDIA;
	xmilcfg.DIP_SW &= ~DIPSW_BOOTMEDIA;
	xmilcfg.DIP_SW |= value;
	CheckMenuItem(hmenu, IDM_BOOT2D, MFCHECK(!value));
	CheckMenuItem(hmenu, IDM_BOOT2HD, MFCHECK(value));
}

void menu_setresolute(UINT8 value) {

	HMENU	hmenu;

	hmenu = extclass_gethmenu(hWndMain);
	value &= DIPSW_RESOLUTE;
	xmilcfg.DIP_SW &= ~DIPSW_RESOLUTE;
	xmilcfg.DIP_SW |= value;
	CheckMenuItem(hmenu, IDM_HIGHRES, MFCHECK(!value));
	CheckMenuItem(hmenu, IDM_LOWRES, MFCHECK(value));
}

void menu_setwidth(UINT8 value) {

	HMENU	hmenu;

	hmenu = extclass_gethmenu(hWndMain);
	CheckMenuItem(hmenu, IDM_WIDTH80, MFCHECK(!value));
	CheckMenuItem(hmenu, IDM_WIDTH40, MFCHECK(value));
}

void menu_setdispmode(UINT8 value) {

	value &= 1;
	xmilcfg.DISPSYNC = value;
	CheckMenuItem(extclass_gethmenu(hWndMain), IDM_DISPSYNC, MFCHECK(value));
}

void menu_setraster(UINT8 value) {

	value &= 1;
	xmilcfg.RASTER = value;
	CheckMenuItem(extclass_gethmenu(hWndMain), IDM_RASTER, MFCHECK(value));
}

void menu_setwaitflg(UINT8 value) {

	value &= 1;
	xmiloscfg.NOWAIT = value;
	CheckMenuItem(extclass_gethmenu(hWndMain), IDM_NOWAIT, MFCHECK(value));
}

void menu_setframe(UINT8 value) {

	HMENU	hmenu;

	hmenu = extclass_gethmenu(hWndMain);
	xmiloscfg.DRAW_SKIP = value;
	CheckMenuItem(hmenu, IDM_AUTOFPS, MFCHECK(!value));
	CheckMenuItem(hmenu, IDM_60FPS, MFCHECK(value == 1));
	CheckMenuItem(hmenu, IDM_30FPS, MFCHECK(value == 2));
	CheckMenuItem(hmenu, IDM_20FPS, MFCHECK(value == 3));
	CheckMenuItem(hmenu, IDM_15FPS, MFCHECK(value == 4));
}

void menu_setkey(UINT8 value) {

	HMENU	hmenu;

	hmenu = extclass_gethmenu(hWndMain);
	if (value >= 3) {
		value = 0;
	}
	xmilcfg.KEY_MODE = value;
	CheckMenuItem(hmenu, IDM_KEY, MFCHECK(value == 0));
	CheckMenuItem(hmenu, IDM_JOY1, MFCHECK(value == 1));
	CheckMenuItem(hmenu, IDM_JOY2, MFCHECK(value == 2));
}

void menu_setsound(UINT8 value) {

	xmilcfg.SOUND_SW = value;
	CheckMenuItem(extclass_gethmenu(hWndMain), IDM_FMBOARD, MFCHECK(value));
}

void menu_setjoystick(UINT8 value) {

	HMENU	hmenu;

	hmenu = extclass_gethmenu(hWndMain);
	xmiloscfg.JOYSTICK &= 0x80;
	xmiloscfg.JOYSTICK |= (value & 0x7f);
	if (value & 2) {
		EnableMenuItem(hmenu, IDM_JOYSTICK, MF_GRAYED);
	}
	else {
		CheckMenuItem(hmenu, IDM_JOYSTICK, MFCHECK(value & 1));
	}
}

void menu_setmouse(UINT8 value) {

	value &= 1;
	xmilcfg.MOUSE_SW = value;
	CheckMenuItem(extclass_gethmenu(hWndMain), IDM_MOUSE, MFCHECK(value));
}

void menu_setmotorflg(UINT8 value) {

	value &= 1;
	xmilcfg.MOTOR = value;
	CheckMenuItem(extclass_gethmenu(hWndMain), IDM_SEEKSND, MFCHECK(value));
}

void menu_opmlog(UINT8 value) {

	CheckMenuItem(extclass_gethmenu(hWndMain), IDM_OPMLOG, MFCHECK(value));
}

void menu_setdispclk(UINT8 value) {

	HMENU	hmenu;

	hmenu = extclass_gethmenu(hWndMain);
	value &= 3;
	xmiloscfg.DISPCLK = value;
	CheckMenuItem(hmenu, IDM_DISPCLOCK, MFCHECK(value & 1));
	CheckMenuItem(hmenu, IDM_DISPFRAME, MFCHECK(value & 2));
	sysmng_workclockrenewal();
	sysmng_updatecaption(3);
}

void menu_setbtnmode(UINT8 value) {

	value &= 1;
	xmilcfg.BTN_MODE = value;
	CheckMenuItem(extclass_gethmenu(hWndMain), IDM_JOYX, MFCHECK(value));
}

void menu_setbtnrapid(UINT8 value) {

	value &= 1;
	xmilcfg.BTN_RAPID = value;
	CheckMenuItem(extclass_gethmenu(hWndMain), IDM_RAPID, MFCHECK(value));
}

