#include	"compiler.h"
#include	"palmkbd.h"
#include	"keystat.h"
#include	"resource.h"

#define		NC		0xff

static const UINT8 keypalm[256] = {
			//	    ,    ,    ,    ,    ,    ,    ,    		; 0x00
				  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
			//	  BS, TAB,  LF,    ,    ,  CR,    , SFT,	; 0x08
				0x0e,0x0f,0x1c,  NC,  NC,0x1c,  NC,0x70,
			//	    ,    ,    ,    ,    ,    ,    ,    		; 0x10
				  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
			//	    ,    ,    , ESC,  Å©,  Å®,  Å™,  Å´,		; 0x18
				  NC,  NC,  NC,0x00,0x3b,0x3c,0x3a,0x3d,
			//	 SPC,   !,   ",  # ,   $,   %,   &,   ',	; 0x20
				0x34,0x81,0x82,0x83,0x84,0x85,0x86,0x87,
			//	   (,   ),   *,   +,   ,,   -,   .,   /, 	; 0x28
				0x88,0x89,0x45,0x49,0x30,0x0b,0x31,0x32,
			//	   0,   1,   2,   3,   4,   5,   6,   7,	; 0x30
				0x0a,0x01,0x02,0x03,0x04,0x05,0x06,0x07,
			//	   8,   9,   :,   ;,   <,   =,   >,   ?, 	; 0x38
				0x08,0x09,0x27,0x26,0xb0,0x4d,0xb1,0xb2,
			//	   @,   A,   B,   C,   D,   E,   F,   G,	; 0x40
				0x1a,0x9d,0xad,0xab,0x9f,0x92,0xa0,0xa1,
			//	   H,   I,   J,   K,   L,   M,   N,   O,	; 0x48
				0xa2,0x97,0xa3,0xa4,0xa5,0xaf,0xae,0x98,
			//	   P,   Q,   R,   S,   T,   U,   V,   W,	; 0x50
				0x99,0x90,0x93,0x9e,0x94,0x96,0xac,0x91,
			//	   X,   Y,   Z,   [,   \,   ],    ,   _,	; 0x58
				0xaa,0x95,0xa9,0x1b,0x0d,0x28,  NC,0x33,
			//	    ,   a,   b,   c,   d,   e,   f,   g,	; 0x60
				  NC,0x1d,0x2d,0x2b,0x1f,0x12,0x20,0x21,
			//	   h,   i,   j,   k,   l,   m,   n,   o,	; 0x68
				0x22,0x17,0x23,0x24,0x25,0x2f,0x2e,0x18,
			//	   p,   q,   r,   s,   t,   u,   v,   w,	; 0x70
				0x19,0x10,0x13,0x1e,0x14,0x16,0x2c,0x11,
			//	   x,   y,   z,   {,   |,   },   ~, del,	; 0x78
				0x2a,0x15,0x29,0x9b,0x8d,0xa8,  NC,0x39,
			//	 [0], [1], [2], [3], [4], [5], [6], [7],	; 0x80
				  NC,0x4a,0x4b,0x4c,0x46,0x47,0x48,0x42,
			//	 [8], [9],STOP,HOME,   l,   m,   n,   o,	; 0x88
				0x43,0x44,0x60,0x3e,  NC,  NC,  NC,  NC,
			//	   p,  F1,  F2,  F3,  F4,  F5,   v,   w,	; 0x90
				  NC,0x62,0x63,0x64,0x65,0x66,  NC,  NC,
			//	   x,   y,   z,   {,   |,   },   ~, del,	; 0x98
				  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC};


typedef struct {
	UINT32	tick;
	int		inkey;
	BOOL	input;
	BOOL	modifier;
} PALMKBD;

static	PALMKBD	palmkbd;

void palmkbd_initialize(void) {

	palmkbd.input = FALSE;
	palmkbd.inkey = NC;
	palmkbd.modifier = FALSE;
}

BOOL palmkbd_keydown(int keycode, BOOL cmd) {

	BYTE	data;

	data = keypalm[keycode];
	if (data != NC) {
		keystat_keydown(data & 0x7f);
		if (cmd == TRUE) {
			palmkbd.tick = GETTICK();
			palmkbd.input = TRUE;
			palmkbd.inkey = keycode;
			if (data & 0x80) {
				if (!keystat.hit[SHIFT_CODE]) {
					palmkbd.modifier = TRUE;
					keystat_keydown(0x70);
				}
			}
		}
		return(TRUE);
	}
	return(FALSE);
}

BOOL palmkbd_keyup(int keycode) {

	BYTE	data;

	data = keypalm[keycode];
	if (data != NC) {
		keystat_keyup(data & 0x7f);
		return(TRUE);
	}
	return(FALSE);
}

void palmkbd_callback(void) {
	if (palmkbd.input) {
		if ((GETTICK() - palmkbd.tick) > 100) {
			if (palmkbd.modifier) {
				keystat_keyup(0x70);
				palmkbd.modifier = FALSE;
			}
			palmkbd_keyup(palmkbd.inkey);
			palmkbd.input = FALSE;
			palmkbd.inkey = NC;
		}
	}
}

void palmkbd_modifiers(UINT ctrl) {

	UINT	modif;
	switch (ctrl) {
		case IDC_SHIFT:
			modif = 0x70;
			break;
		case IDC_CTRL:
			modif = 0x74;
			break;
		case IDC_GRAPH:
			modif = 0x73;
			break;
		case IDC_CAPS:
			modif = 0x71;
			break;
		case IDC_KANA:
			modif = 0x72;
			break;
		default:
			return;
			break;
	}
	keystat_keydown(modif + 0x10);
}

