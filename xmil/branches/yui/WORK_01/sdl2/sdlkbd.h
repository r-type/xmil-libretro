/**
 *	@file	sdlkbd.h
 *	@brief	Interface of the keyboard
 */

#pragma once

#if (SDL_MAJOR_VERSION < 2)
typedef signed short SDL_Keycode;
#endif	/* (SDL_MAJOR_VERSION < 2) */

void sdlkbd_keydown(SDL_Keycode code);
void sdlkbd_keyup(SDL_Keycode code);
