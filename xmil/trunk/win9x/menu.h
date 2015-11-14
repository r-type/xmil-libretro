/**
 * @file	menu.h
 * @brief	メニューの宣言およびインターフェイスの定義をします
 */

#pragma once

#define	MFCHECK(a) ((a) ? MF_CHECKED : MF_UNCHECKED)

bool menu_searchmenu(HMENU hMenu, UINT uID, HMENU *phmenuRet, int *pnPos);
int menu_addmenu(HMENU hMenu, int nPos, HMENU hmenuAdd, bool bSeparator = false);
int menu_addmenures(HMENU hMenu, int nPos, UINT uID, bool bSeparator = false);
int menu_addmenubyid(HMENU hMenu, UINT uByID, UINT uID);
BOOL menu_insertmenures(HMENU hMenu, int nPosition, UINT uFlags, UINT_PTR uIDNewItem, UINT uID);
int menu_addmenubar(HMENU popup, HMENU menubar);

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
void menu_setjoystick(UINT8 value);
void menu_setmouse(UINT8 value);
void menu_setmotorflg(UINT8 value);
void menu_opmlog(UINT8 value);
void menu_setdispclk(UINT8 value);
void menu_setbtnmode(UINT8 value);
void menu_setbtnrapid(UINT8 value);

