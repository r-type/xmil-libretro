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


void menu_disablewindow(void) {

	HMENU	hmenu;

	hmenu = GetMenu(hWndMain);
	EnableMenuItem(hmenu, IDM_WINDOW, MF_GRAYED);
	EnableMenuItem(hmenu, IDM_FULLSCREEN, MF_GRAYED);
}

void menu_setiplrom(UINT8 num) {

	HMENU	hmenu;

	hmenu = GetMenu(hWndMain);
	xmilcfg.ROM_TYPE = num;
	CheckMenuItem(hmenu, IDM_TURBOZ, MFCHECK(num == 3));
	CheckMenuItem(hmenu, IDM_TURBO, MFCHECK(num == 2));
	CheckMenuItem(hmenu, IDM_X1ROM, MFCHECK(num == 1));
}

void menu_setbootmedia(UINT8 value) {

	HMENU	hmenu;

	hmenu = GetMenu(hWndMain);
	value &= DIPSW_BOOTMEDIA;
	xmilcfg.DIP_SW &= ~DIPSW_BOOTMEDIA;
	xmilcfg.DIP_SW |= value;
	CheckMenuItem(hmenu, IDM_BOOT2D, MFCHECK(!value));
	CheckMenuItem(hmenu, IDM_BOOT2HD, MFCHECK(value));
}

void menu_setwidth(UINT8 value) {

	HMENU	hmenu;

	hmenu = GetMenu(hWndMain);
	CheckMenuItem(hmenu, IDM_WIDTH80, MFCHECK(!value));
	CheckMenuItem(hmenu, IDM_WIDTH40, MFCHECK(value));
}

void menu_setresolute(UINT8 value) {

	HMENU	hmenu;

	hmenu = GetMenu(hWndMain);
	value &= DIPSW_RESOLUTE;
	xmilcfg.DIP_SW &= ~DIPSW_RESOLUTE;
	xmilcfg.DIP_SW |= value;
	CheckMenuItem(hmenu, IDM_HIGHRES, MFCHECK(!value));
	CheckMenuItem(hmenu, IDM_LOWRES, MFCHECK(value));
}

void menu_setdispmode(UINT8 value) {

	value &= 1;
	xmilcfg.DISPSYNC = value;
	CheckMenuItem(GetMenu(hWndMain), IDM_DISPSYNC, MFCHECK(value));
}

void menu_setraster(UINT8 value) {

	value &= 1;
	xmilcfg.RASTER = value;
	CheckMenuItem(GetMenu(hWndMain), IDM_RASTER, MFCHECK(value));
}

void menu_setwaitflg(UINT8 value) {

	value &= 1;
	xmiloscfg.NOWAIT = value;
	CheckMenuItem(GetMenu(hWndMain), IDM_NOWAIT, MFCHECK(value));
}

void menu_setframe(UINT8 value) {

	HMENU	hmenu;

	hmenu = GetMenu(hWndMain);
	xmiloscfg.DRAW_SKIP = value;
	CheckMenuItem(hmenu, IDM_AUTOFPS, MFCHECK(!value));
	CheckMenuItem(hmenu, IDM_60FPS, MFCHECK(value == 1));
	CheckMenuItem(hmenu, IDM_30FPS, MFCHECK(value == 2));
	CheckMenuItem(hmenu, IDM_20FPS, MFCHECK(value == 3));
	CheckMenuItem(hmenu, IDM_15FPS, MFCHECK(value == 4));
}

void menu_setkey(UINT8 value) {

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

void menu_setsound(UINT8 value) {

	xmilcfg.SOUND_SW = value;
	CheckMenuItem(GetMenu(hWndMain), IDM_FMBOARD, MFCHECK(value));
}

void menu_setjoystick(UINT8 value) {

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

void menu_setmouse(UINT8 value) {

	value &= 1;
	xmilcfg.MOUSE_SW = value;
	CheckMenuItem(GetMenu(hWndMain), IDM_MOUSE, MFCHECK(value));
}

void menu_setcpuspeed(UINT8 value) {

	value &= 1;
	xmilcfg.CPU8MHz = value;
	CheckMenuItem(GetMenu(hWndMain), IDM_8MHZ, MFCHECK(value));
}

void menu_setmotorflg(UINT8 value) {

	value &= 1;
	xmilcfg.MOTOR = value;
	CheckMenuItem(GetMenu(hWndMain), IDM_SEEKSND, MFCHECK(value));
}

void menu_opmlog(UINT8 value) {

	CheckMenuItem(GetMenu(hWndMain), IDM_OPMLOG, MFCHECK(value));
}

void menu_setdispclk(UINT8 value) {

	HMENU	hmenu;

	hmenu = GetMenu(hWndMain);
	value &= 3;
	xmiloscfg.DISPCLK = value;
	CheckMenuItem(hmenu, IDM_DISPCLOCK, MFCHECK(value & 1));
	CheckMenuItem(hmenu, IDM_DISPFRAME, MFCHECK(value & 2));
	sysmng_workclockrenewal();
	sysmng_updatecaption(3);
}

void menu_setbtnmode(UINT8 value) {

	value &= 1;
	xmilcfg.BTN_MODE = value & 1;
	CheckMenuItem(GetMenu(hWndMain), IDM_JOYX, MFCHECK(xmilcfg.BTN_MODE));
}

void menu_setbtnrapid(UINT8 value) {

	value &= 1;
	xmilcfg.BTN_RAPID = value;
	CheckMenuItem(GetMenu(hWndMain), IDM_RAPID, MFCHECK(value));
}

void menu_setz80save(UINT8 value) {

	if (!value) {
		EnableMenuItem(GetMenu(hWndMain), IDM_Z80SAVE, MF_GRAYED);
	}
}

