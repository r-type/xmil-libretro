/**
 * @file	menu.cpp
 * @brief	メニューの動作の定義を行います
 */

#include "compiler.h"
#include "resource.h"
#include "menu.h"
#include "xmil.h"
#include "dosio.h"
#include "ini.h"
#include "scrnmng.h"
#include "sysmng.h"
#include "extclass.h"
#include "misc\tstring.h"
#include "misc\WndProc.h"
#include "pccore.h"
#include "palettes.h"
#include "makescrn.h"
#include "fdd_mtr.h"
#include "fdd_ini.h"

bool menu_searchmenu(HMENU hMenu, UINT uID, HMENU *phmenuRet, int *pnPos)
{
	int nCount = GetMenuItemCount(hMenu);
	for (int i = 0; i < nCount; i++)
	{
		MENUITEMINFO mii;
		ZeroMemory(&mii, sizeof(mii));
		mii.cbSize = sizeof(mii);
		mii.fMask = MIIM_ID | MIIM_SUBMENU;
		if (GetMenuItemInfo(hMenu, i, TRUE, &mii))
		{
			if (mii.wID == uID)
			{
				if (phmenuRet)
				{
					*phmenuRet = hMenu;
				}
				if (pnPos)
				{
					*pnPos = i;
				}
				return true;
			}
			else if ((mii.hSubMenu) && (menu_searchmenu(mii.hSubMenu, uID, phmenuRet, pnPos)))
			{
				return true;
			}
		}
	}
	return false;
}

// これってAPIあるのか？
int menu_addmenu(HMENU hMenu, int nPos, HMENU hmenuAdd, bool bSeparator)
{
	if (nPos < 0)
	{
		nPos = GetMenuItemCount(hMenu);
	}
	int nCount = GetMenuItemCount(hmenuAdd);
	int nAdded = 0;
	for (int i = 0; i < nCount; i++)
	{
		MENUITEMINFO mii;
		ZeroMemory(&mii, sizeof(mii));

		TCHAR szString[128];
		mii.cbSize = sizeof(mii);
		mii.fMask = MIIM_TYPE | MIIM_STATE | MIIM_ID | MIIM_SUBMENU | MIIM_DATA;
		mii.dwTypeData = szString;
		mii.cch = _countof(szString);
		if (GetMenuItemInfo(hmenuAdd, i, TRUE, &mii))
		{
			if (mii.hSubMenu)
			{
				HMENU hmenuSub = CreatePopupMenu();
				(void)menu_addmenu(hmenuSub, 0, mii.hSubMenu);
				mii.hSubMenu = hmenuSub;
			}
			if (bSeparator)
			{
				bSeparator = false;
				InsertMenu(hMenu, nPos + nAdded, MF_BYPOSITION | MF_SEPARATOR, 0, NULL);
				nAdded++;
			}
			InsertMenuItem(hMenu, nPos + nAdded, TRUE, &mii);
			nAdded++;
		}
	}
	return nAdded;
}

int menu_addmenures(HMENU hMenu, int nPos, UINT uID, bool bSeparator)
{
	int nCount = 0;

	HMENU hmenuAdd = LoadMenu(CWndProc::GetResourceHandle(), MAKEINTRESOURCE(uID));
	if (hmenuAdd)
	{
		nCount = menu_addmenu(hMenu, nPos, hmenuAdd, bSeparator);
		DestroyMenu(hmenuAdd);
	}
	return nCount;
}

int menu_addmenubyid(HMENU hMenu, UINT uByID, UINT uID)
{
	int nCount = 0;

	HMENU hmenuSub;
	int nSubPos;
	if (menu_searchmenu(hMenu, uByID, &hmenuSub, &nSubPos))
	{
		nCount = menu_addmenures(hmenuSub, nSubPos + 1, uID);
	}
	return nCount;
}

BOOL menu_insertmenures(HMENU hMenu, int nPosition, UINT uFlags, UINT_PTR uIDNewItem, UINT uID)
{
	std::tstring rString(LoadTString(uID));

	BOOL bResult = FALSE;
	if (!rString.empty())
	{
		bResult = InsertMenu(hMenu, nPosition, uFlags, uIDNewItem, rString.c_str());
	}
	return bResult;
}

int menu_addmenubar(HMENU popup, HMENU menubar)
{
	return menu_addmenu(popup, 0, menubar);
}


// ----

void sysmenu_initialize()
{
	HMENU hMenu = GetSystemMenu(hWndMain, FALSE);
	menu_addmenures(hMenu, 0, IDR_SYS);
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

void menu_initialize()
{
	HMENU hMenu = GetMenu(hWndMain);

#if defined(SUPPORT_WAVEREC)
	menu_addmenubyid(hMenu, IDM_OPMLOG, IDR_WAVEREC);
#endif

	if (xmiloscfg.Z80SAVE)
	{
		menu_addmenures(hMenu, 5, IDR_DEBUG);
	}

#if defined(SUPPORT_TURBOZ)
	menu_addmenubyid(hMenu, IDM_TURBO, IDR_TURBOZ);
#endif

	int nPos = 1;
#if defined(SUPPORT_STATSAVE)
	if (xmiloscfg.statsave)
	{
		nPos += menu_addmenures(hMenu, nPos, IDR_STAT);
	}
#endif

	for (UINT i = 0; i < 4; i++)
	{
		if (xmilcfg.fddequip & (1 << i))
		{
			nPos += menu_addmenures(hMenu, nPos, IDR_FDD0MENU + i);
		}
	}

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

