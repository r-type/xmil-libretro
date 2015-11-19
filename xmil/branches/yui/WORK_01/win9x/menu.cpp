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

/**
 * メニュー内の指定された位置に、新しいメニュー項目を挿入します
 * @param[in] hMenu メニューのハンドル
 * @param[in] uItem 識別子または位置
 * @param[in] fByPosition uItem パラメータの意味
 * @param[in] hPopup 追加するメニュー
 * @return 追加した項目数
 */
static UINT InsertMenuPopup(HMENU hMenu, UINT uItem, BOOL fByPosition, HMENU hPopup)
{
	int nCount = GetMenuItemCount(hPopup);
	UINT nAdded = 0;
	for (int i = 0; i < nCount; i++)
	{
		TCHAR szString[128];

		MENUITEMINFO mii;
		ZeroMemory(&mii, sizeof(mii));
		mii.cbSize = sizeof(mii);
		mii.fMask = MIIM_TYPE | MIIM_STATE | MIIM_ID | MIIM_SUBMENU | MIIM_DATA;
		mii.dwTypeData = szString;
		mii.cch = _countof(szString);
		if (!GetMenuItemInfo(hPopup, i, TRUE, &mii))
		{
			continue;
		}
		if (mii.hSubMenu)
		{
			HMENU hSubMenu = CreatePopupMenu();
			InsertMenuPopup(hSubMenu, 0, TRUE, mii.hSubMenu);
			mii.hSubMenu = hSubMenu;
		}
		if (InsertMenuItem(hMenu, uItem, fByPosition, &mii))
		{
			nAdded++;
			if (fByPosition)
			{
				uItem++;
			}
		}
	}
	return nAdded;
}

/**
 * メニュー内の指定された位置に、新しいメニュー項目を挿入します
 * @param[in] hMenu メニューのハンドル
 * @param[in] uItem 識別子または位置
 * @param[in] fByPosition uItem パラメータの意味
 * @param[in] nMenuID 追加するメニュー ID
 * @return 追加した項目数
 */
UINT InsertMenuResource(HMENU hMenu, UINT uItem, BOOL fByPosition, UINT nMenuID)
{
	HMENU hSubMenu = LoadMenu(CWndProc::GetResourceHandle(), MAKEINTRESOURCE(nMenuID));
	const UINT nAdded = InsertMenuPopup(hMenu, uItem, fByPosition, hSubMenu);
	::DestroyMenu(hSubMenu);
	return nAdded;
}

/**
 * メニューを追加する
 * @param[in] hMenu メニューのハンドル
 * @param[in] uFlags オプション
 * @param[in] uIDNewItem 識別子、メニュー、サブメニューのいずれか
 * @retval 関数が成功すると、0 以外の値が返ります
 */
static BOOL AppendMenuString(HMENU hMenu, UINT uFlags, UINT_PTR uIDNewItem)
{
	std::tstring rString(LoadTString(uIDNewItem));
	if (rString.empty())
	{
		return FALSE;
	}

	if (uFlags & MF_POPUP)
	{
		HMENU hSubMenu = LoadMenu(CWndProc::GetResourceHandle(), MAKEINTRESOURCE(uIDNewItem));
		if (hSubMenu == NULL)
		{
			return FALSE;
		}
		uIDNewItem = reinterpret_cast<UINT_PTR>(hSubMenu);
	}
	return AppendMenu(hMenu, uFlags, uIDNewItem, rString.c_str());
}

/**
 * メニューを挿入する
 * @param[in] hMenu メニューのハンドル
 * @param[in] uItem 挿入するべきメニュー項目の直後に位置するメニュー項目の識別子または位置を指定します
 * @param[in] uFlags オプション
 * @param[in] uIDNewItem 識別子、メニュー、サブメニューのいずれか
 * @retval 関数が成功すると、0 以外の値が返ります
 */
static BOOL InsertMenuString(HMENU hMenu, UINT uItem, UINT uFlags, UINT_PTR uIDNewItem)
{
	std::tstring rString(LoadTString(uIDNewItem));
	if (rString.empty())
	{
		return FALSE;
	}

	if (uFlags & MF_POPUP)
	{
		HMENU hSubMenu = LoadMenu(CWndProc::GetResourceHandle(), MAKEINTRESOURCE(uIDNewItem));
		if (hSubMenu == NULL)
		{
			return FALSE;
		}
		uIDNewItem = reinterpret_cast<UINT_PTR>(hSubMenu);
	}
	return InsertMenu(hMenu, uItem, uFlags, uIDNewItem, rString.c_str());
}



// ----

void sysmenu_initialize()
{
	HMENU hMenu = GetSystemMenu(hWndMain, FALSE);
	InsertMenuResource(hMenu, 0, TRUE, IDR_SYS);
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
		InsertMenuString(hMenu, IDM_DISPCLOCK, MF_BYCOMMAND | MF_STRING, IDM_WAVEREC);
#endif

	if (xmiloscfg.Z80SAVE)
	{
		InsertMenuString(hMenu, 5, MF_BYPOSITION | MF_POPUP, IDR_DEBUG);
	}

#if defined(SUPPORT_TURBOZ)
	AppendMenuString(GetSubMenu(hMenu, 1), MF_BYPOSITION | MF_STRING, IDM_TURBOZ);
#endif

	UINT nPos = 1;
#if defined(SUPPORT_STATSAVE)
	if (xmiloscfg.statsave)
	{
		nPos += InsertMenuString(hMenu, nPos, MF_BYPOSITION | MF_POPUP, IDR_STAT);
	}
#endif

	for (UINT i = 0; i < 4; i++)
	{
		if (xmilcfg.fddequip & (1 << i))
		{
			nPos += InsertMenuString(hMenu, nPos, MF_BYPOSITION | MF_POPUP, IDR_FDD0MENU + i);
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

void menu_enablejoystick(bool bEnabled)
{
	HMENU hMenu = extclass_gethmenu(hWndMain);
	EnableMenuItem(hMenu, IDM_JOYSTICK, (bEnabled) ? MF_ENABLED : MF_GRAYED);
}

void menu_setjoystick(UINT8 value)
{
	xmiloscfg.JOYSTICK = value & 1;
	HMENU hMenu = extclass_gethmenu(hWndMain);
	CheckMenuItem(hMenu, IDM_JOYSTICK, MFCHECK(value & 1));
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

