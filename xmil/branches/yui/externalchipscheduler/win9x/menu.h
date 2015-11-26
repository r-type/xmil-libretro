/**
 * @file	menu.h
 * @brief	メニューの宣言およびインターフェイスの定義をします
 */

#pragma once

#define	MFCHECK(a) ((a) ? MF_CHECKED : MF_UNCHECKED)

UINT InsertMenuResource(HMENU hMenu, UINT uItem, BOOL fByPosition, UINT nMenuID);

void sysmenu_initialize(HMENU hMenu);
void sysmenu_update(HMENU hMenu);

void menu_initialize(HMENU hMenu);
void menu_update(HMENU hMenu);
void menu_disablewindow(void);
