#include	"compiler.h"
#include	"resource.h"
#include	"xmil.h"
#include	"sysmng.h"
#include	"menu.h"
#include	"pccore.h"


#define	MFCHECK(a)					((a)?1:0)
#ifdef TARGET_API_MAC_CARBON
#define	_GetMenu(a)					GetMenuHandle((a))
#define	_CheckMenuItem(a, b, c)		CheckMenuItem((a), LoWord(b), (c))
#else
#define	_GetMenu(a)					GetMenu((a))
#define	_CheckMenuItem(a, b, c)		CheckItem((a), LoWord(b), (c))
#endif


void menu_setiplrom(UINT8 value) {

	MenuHandle	hmenu;

	hmenu = _GetMenu(IDM_IPLROM);
	xmilcfg.ROM_TYPE = value;
	_CheckMenuItem(hmenu, IDM_X1ROM, MFCHECK(value == 1));
	_CheckMenuItem(hmenu, IDM_TURBO, MFCHECK(value == 2));
#if defined(SUPPORT_TURBOZ)
	_CheckMenuItem(hmenu, IDM_TURBOZ, MFCHECK(value == 3));
#endif
}

void menu_setbootmedia(UINT8 value) {

	MenuHandle	hmenu;

	hmenu = _GetMenu(IDM_DIPSW);
	value &= DIPSW_BOOTMEDIA;
	xmilcfg.DIP_SW &= ~DIPSW_BOOTMEDIA;
	xmilcfg.DIP_SW |= value;
	_CheckMenuItem(hmenu, IDM_BOOT2D, MFCHECK(!value));
	_CheckMenuItem(hmenu, IDM_BOOT2HD, MFCHECK(value));
}

void menu_setresolute(UINT8 value) {

	MenuHandle	hmenu;

	hmenu = _GetMenu(IDM_DIPSW);
	value &= DIPSW_RESOLUTE;
	xmilcfg.DIP_SW &= ~DIPSW_RESOLUTE;
	xmilcfg.DIP_SW |= value;
	_CheckMenuItem(hmenu, IDM_HIGHRES, MFCHECK(!value));
	_CheckMenuItem(hmenu, IDM_LOWRES, MFCHECK(value));
}

void menu_setwidth(UINT8 value) {

	MenuHandle	hmenu;

	hmenu = _GetMenu(IDM_SCREEN);
	_CheckMenuItem(hmenu, IDM_WIDTH80, MFCHECK(!value));
	_CheckMenuItem(hmenu, IDM_WIDTH40, MFCHECK(value));
}

void menu_setdispmode(UINT8 value) {

	value &= 1;
	xmilcfg.DISPSYNC = value;
	_CheckMenuItem(_GetMenu(IDM_SCREEN), IDM_DISPSYNC, MFCHECK(value));
}

void menu_setraster(UINT8 value) {

	value &= 1;
	xmilcfg.RASTER = value;
	_CheckMenuItem(_GetMenu(IDM_SCREEN), IDM_RASTER, MFCHECK(value));
}

void menu_setwaitflg(UINT8 value) {

	value &= 1;
	xmiloscfg.NOWAIT = value;
	_CheckMenuItem(_GetMenu(IDM_SCREEN), IDM_NOWAIT, MFCHECK(value));
}

void menu_setframe(UINT8 value) {

	MenuHandle	hmenu;

	xmiloscfg.DRAW_SKIP = value;
	hmenu = _GetMenu(IDM_SCREEN);
	_CheckMenuItem(hmenu, IDM_AUTOFPS, MFCHECK(value == 0));
	_CheckMenuItem(hmenu, IDM_60FPS, MFCHECK(value == 1));
	_CheckMenuItem(hmenu, IDM_30FPS, MFCHECK(value == 2));
	_CheckMenuItem(hmenu, IDM_20FPS, MFCHECK(value == 3));
	_CheckMenuItem(hmenu, IDM_15FPS, MFCHECK(value == 4));
}

void menu_setkey(UINT8 value) {

	MenuHandle	hmenu;

	if (value > 3) {
		value = 0;
	}
	xmilcfg.KEY_MODE = value;
	hmenu = _GetMenu(IDM_DEVICE);
	_CheckMenuItem(hmenu, IDM_KEY, MFCHECK(value == 0));
	_CheckMenuItem(hmenu, IDM_JOY1, MFCHECK(value == 1));
	_CheckMenuItem(hmenu, IDM_JOY2, MFCHECK(value == 2));
//	_CheckMenuItem(hmenu, IDM_MOUSEKEY, MFCHECK(value == 3));
}

void menu_setsound(UINT8 value) {

	xmilcfg.SOUND_SW = value;
	_CheckMenuItem(_GetMenu(IDM_DEVICE), IDM_FMBOARD, MFCHECK(value));
}

void menu_setmouse(UINT8 value) {

	value &= 1;
	xmilcfg.MOUSE_SW = value;
	_CheckMenuItem(GetMenu(IDM_DEVICE), IDM_MOUSE, MFCHECK(value));
}

void menu_setmotorflg(UINT8 value) {

	value &= 1;
	xmilcfg.MOTOR = value;
	_CheckMenuItem(_GetMenu(IDM_DEVICE), IDM_SEEKSND, MFCHECK(value));
}

void menu_setdispclk(UINT8 value) {

	MenuHandle	hmenu;

	value &= 3;
	xmiloscfg.DISPCLK = value;
	hmenu = _GetMenu(IDM_OTHER);
	_CheckMenuItem(hmenu, IDM_DISPCLOCK, MFCHECK(value & 1));
	_CheckMenuItem(hmenu, IDM_DISPFRAME, MFCHECK(value & 2));
	sysmng_workclockrenewal();
	sysmng_updatecaption(3);
}

void menu_setskipline(UINT8 value) {

	(void)value;
}

void menu_setbtnmode(UINT8 value) {

	value &= 1;
	xmilcfg.BTN_MODE = value;
	_CheckMenuItem(_GetMenu(IDM_OTHER), IDM_JOYX, MFCHECK(value));
}

void menu_setbtnrapid(UINT8 value) {

	value &= 1;
	xmilcfg.BTN_RAPID = value;
	_CheckMenuItem(_GetMenu(IDM_OTHER), IDM_RAPID, MFCHECK(value));
}

