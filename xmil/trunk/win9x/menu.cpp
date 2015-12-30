/**
 * @file	menu.cpp
 * @brief	メニューの動作の定義を行います
 */

#include "compiler.h"
#include "resource.h"
#include "menu.h"
#include "xmil.h"
#include "joymng.h"
#include "scrnmng.h"
#include "sysmng.h"
#include "extclass.h"
#include "misc\tstring.h"
#include "misc\WndProc.h"
#include "subwnd/kdispwnd.h"
#include "subwnd/skbdwnd.h"
#include "pccore.h"
#include "iocore.h"
#include "x1f.h"

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

/**
 * システム メニュー初期化
 * @param[in] hMenu メニュー ハンドル
 */
void sysmenu_initialize(HMENU hMenu)
{
	UINT nPos = 0;

#if defined(SUPPORT_KEYDISP)
	nPos += InsertMenuString(hMenu, nPos, MF_BYPOSITION | MF_STRING, IDM_KEYDISP);
#endif	// defined(SUPPORT_KEYDISP)

#if defined(SUPPORT_SOFTKBD)
	nPos += InsertMenuString(hMenu, nPos, MF_BYPOSITION | MF_STRING, IDM_SOFTKBD);
#endif	// defined(SUPPORT_SOFTKBD)

	InsertMenuResource(hMenu, nPos, TRUE, IDR_SYS);
}

/**
 * システム メニュー更新
 * @param[in] hMenu メニュー ハンドル
 */
void sysmenu_update(HMENU hMenu)
{
#if defined(SUPPORT_KEYDISP)
	CheckMenuItem(hMenu, IDM_KEYDISP, MF_BYCOMMAND | MFCHECK(kdispwin_gethwnd() != NULL));
#endif	// defined(SUPPORT_KEYDISP)

#if defined(SUPPORT_SOFTKBD)
	CheckMenuItem(hMenu, IDM_SOFTKBD, MF_BYCOMMAND | MFCHECK(skbdwin_gethwnd() != NULL));
#endif	// defined(SUPPORT_SOFTKBD)

	CheckMenuItem(hMenu, IDM_SNAPENABLE, MF_BYCOMMAND | MFCHECK(xmiloscfg.WINSNAP));

	const UINT8 background = xmiloscfg.background ^ 3;
	EnableMenuItem(hMenu, IDM_BGSOUND, (background & 1) ? MF_ENABLED : MF_GRAYED);
	CheckMenuItem(hMenu, IDM_BACKGROUND, MF_BYCOMMAND | MFCHECK(background & 1));
	CheckMenuItem(hMenu, IDM_BGSOUND, MF_BYCOMMAND | MFCHECK(background & 2));
}

/**
 * メニュー初期化
 * @param[in] メニュー ハンドル
 */
void menu_initialize(HMENU hMenu)
{
#if defined(SUPPORT_TURBOZ) || defined(SUPPORT_OPM)
#if defined(SUPPORT_OPMx2)
	InsertMenuString(hMenu, IDM_JOYSTICK, MF_BYCOMMAND | MF_POPUP, IDR_OPM);
#else	// defined(SUPPORT_OPMx2)
	InsertMenuString(hMenu, IDM_JOYSTICK, MF_BYCOMMAND | MF_STRING, IDM_FMBOARD);
#endif	// defined(SUPPORT_OPMx2)
#endif	// defined(SUPPORT_TURBOZ) || defined(SUPPORT_OPM)
#if defined(SUPPORT_X1F)
	InsertMenuString(hMenu, IDM_DISPCLOCK, MF_BYCOMMAND | MF_STRING, IDM_OPMLOG);
#endif	// defined(SUPPORT_X1F)
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

/**
 * メニュー状態を更新する
 * @param[in] hMenu メニュー ハンドル
 */
void menu_update(HMENU hMenu)
{
	// IPL-ROM
	const UINT8 ROM_TYPE = xmilcfg.ROM_TYPE;
	CheckMenuItem(hMenu, IDM_X1ROM, MF_BYCOMMAND | MFCHECK(ROM_TYPE == 1));
	CheckMenuItem(hMenu, IDM_TURBO, MF_BYCOMMAND | MFCHECK(ROM_TYPE == 2));
	CheckMenuItem(hMenu, IDM_TURBOZ, MF_BYCOMMAND | MFCHECK(ROM_TYPE == 3));

	// DIP-SW
	const bool b2hd = ((xmilcfg.DIP_SW & DIPSW_BOOTMEDIA) != 0);
	CheckMenuItem(hMenu, IDM_BOOT2D, MF_BYCOMMAND | MFCHECK(!b2hd));
	CheckMenuItem(hMenu, IDM_BOOT2HD, MF_BYCOMMAND | MFCHECK(b2hd));
	const bool bHighRes = ((xmilcfg.DIP_SW & DIPSW_RESOLUTE) == 0);
	CheckMenuItem(hMenu, IDM_HIGHRES, MF_BYCOMMAND | MFCHECK(bHighRes));
	CheckMenuItem(hMenu, IDM_LOWRES, MF_BYCOMMAND | MFCHECK(!bHighRes));

	// Screen
	const bool bFullScreen = scrnmng_isfullscreen();
	CheckMenuItem(hMenu, IDM_WINDOW, MF_BYCOMMAND | MFCHECK(!bFullScreen));
	CheckMenuItem(hMenu, IDM_FULLSCREEN, MF_BYCOMMAND | MFCHECK(bFullScreen));
	CheckMenuItem(hMenu, IDM_DISPSYNC, MF_BYCOMMAND | MFCHECK(xmilcfg.DISPSYNC));
	CheckMenuItem(hMenu, IDM_RASTER, MF_BYCOMMAND | MFCHECK(xmilcfg.RASTER));
	CheckMenuItem(hMenu, IDM_NOWAIT, MF_BYCOMMAND | MFCHECK(xmiloscfg.NOWAIT));
	const UINT8 DRAW_SKIP = xmiloscfg.DRAW_SKIP;
	CheckMenuItem(hMenu, IDM_AUTOFPS, MF_BYCOMMAND | MFCHECK(DRAW_SKIP == 0));
	CheckMenuItem(hMenu, IDM_60FPS, MF_BYCOMMAND | MFCHECK(DRAW_SKIP == 1));
	CheckMenuItem(hMenu, IDM_30FPS, MF_BYCOMMAND | MFCHECK(DRAW_SKIP == 2));
	CheckMenuItem(hMenu, IDM_20FPS, MF_BYCOMMAND | MFCHECK(DRAW_SKIP == 3));
	CheckMenuItem(hMenu, IDM_15FPS, MF_BYCOMMAND | MFCHECK(DRAW_SKIP == 4));

	// Device
	const UINT8 KEY_MODE = xmilcfg.KEY_MODE;
	CheckMenuItem(hMenu, IDM_KEY, MF_BYCOMMAND | MFCHECK(KEY_MODE == 0));
	CheckMenuItem(hMenu, IDM_JOY1, MF_BYCOMMAND | MFCHECK(KEY_MODE == 1));
	CheckMenuItem(hMenu, IDM_JOY2, MF_BYCOMMAND | MFCHECK(KEY_MODE == 2));
#if defined(SUPPORT_TURBOZ) || defined(SUPPORT_OPM)
#if defined(SUPPORT_OPMx2)
	const UINT8 SOUND_SW = xmilcfg.SOUND_SW;
	CheckMenuItem(hMenu, IDM_OPM_NONE, MF_BYCOMMAND | MFCHECK(SOUND_SW == 0));
	CheckMenuItem(hMenu, IDM_OPM_1, MF_BYCOMMAND | MFCHECK(SOUND_SW == 1));
	CheckMenuItem(hMenu, IDM_OPM_2, MF_BYCOMMAND | MFCHECK(SOUND_SW == 2));
#else	// defined(SUPPORT_OPMx2)
	CheckMenuItem(hMenu, IDM_FMBOARD, MF_BYCOMMAND | MFCHECK(xmilcfg.SOUND_SW));
#endif	// defined(SUPPORT_OPMx2)
	CheckMenuItem(hMenu, IDM_FMBOARD, MF_BYCOMMAND | MFCHECK(xmilcfg.SOUND_SW));
#endif	// defined(SUPPORT_TURBOZ) || defined(SUPPORT_OPM)
	const UINT8 JOYSTICK = xmiloscfg.JOYSTICK;
	EnableMenuItem(hMenu, IDM_JOYSTICK, (joymng_isEnabled()) ? MF_GRAYED : MF_ENABLED);
	CheckMenuItem(hMenu, MF_BYCOMMAND | IDM_JOYSTICK, MFCHECK(xmiloscfg.JOYSTICK));
	CheckMenuItem(hMenu, IDM_MOUSE, MF_BYCOMMAND | MFCHECK(xmilcfg.MOUSE_SW));
#if !defined(DISABLE_SOUND)
	CheckMenuItem(hMenu, IDM_SEEKSND, MF_BYCOMMAND | MFCHECK(xmilcfg.MOTOR));
#endif	// !defined(DISABLE_SOUND)

	// Debug
	const bool bWidth80 = ((crtc.e.dispmode & DISPMODE_WIDTH80) != 0);
	CheckMenuItem(hMenu, IDM_WIDTH40, MF_BYCOMMAND | MFCHECK(!bWidth80));
	CheckMenuItem(hMenu, IDM_WIDTH80, MF_BYCOMMAND | MFCHECK(bWidth80));

	// Other
#if defined(SUPPORT_X1F)
	CheckMenuItem(hMenu, IDM_OPMLOG, MF_BYCOMMAND | MFCHECK(x1f_isopened()));
#endif	// defined(SUPPORT_X1F)
	const UINT8 DISPCLK = xmiloscfg.DISPCLK;
	CheckMenuItem(hMenu, IDM_DISPCLOCK, MF_BYCOMMAND | MFCHECK(DISPCLK & 1));
	CheckMenuItem(hMenu, IDM_DISPFRAME, MF_BYCOMMAND | MFCHECK(DISPCLK & 2));
	CheckMenuItem(hMenu, IDM_JOYX, MF_BYCOMMAND | MFCHECK(xmilcfg.BTN_MODE));
	CheckMenuItem(hMenu, IDM_RAPID, MF_BYCOMMAND | MFCHECK(xmilcfg.BTN_RAPID));
}

void menu_disablewindow(void)
{
	HMENU hMenu = extclass_gethmenu(hWndMain);
	EnableMenuItem(hMenu, IDM_WINDOW, MF_GRAYED);
	EnableMenuItem(hMenu, IDM_FULLSCREEN, MF_GRAYED);
}
