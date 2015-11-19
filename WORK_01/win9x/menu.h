/**
 * @file	menu.h
 * @brief	メニューの宣言およびインターフェイスの定義をします
 */

#pragma once

#define	MFCHECK(a) ((a) ? MF_CHECKED : MF_UNCHECKED)

UINT InsertMenuResource(HMENU hMenu, UINT uItem, BOOL fByPosition, UINT nMenuID);

void sysmenu_initialize(void);
void sysmenu_setwinsnap(UINT8 value);
void sysmenu_setbackground(UINT8 value);
void sysmenu_setbgsound(UINT8 value);

void menu_initialize(void);
void menu_disablewindow(void);
void menu_setiplrom(UINT8 num);
void menu_setbootmedia(UINT8 value);
void menu_setwidth(UINT8 value);
void menu_setresolute(UINT8 value);
void menu_setdispmode(UINT8 value);
void menu_setraster(UINT8 value);
void menu_setwaitflg(UINT8 value);
void menu_setframe(UINT8 value);
void menu_setkey(UINT8 value);
void menu_setsound(UINT8 value);
void menu_enablejoystick(bool bEnabled);
void menu_setjoystick(UINT8 value);
void menu_setmouse(UINT8 value);
void menu_setmotorflg(UINT8 value);
#if defined(SUPPORT_X1F)
void menu_opmlog(UINT8 value);
#endif	// defined(SUPPORT_X1F)
void menu_setdispclk(UINT8 value);
void menu_setbtnmode(UINT8 value);
void menu_setbtnrapid(UINT8 value);

