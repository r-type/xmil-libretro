#include	"compiler.h"
#include	"xmil.h"
#include	"dosio.h"
#include	"mackbd.h"
#include	"keystat.h"
#include	"mousemng.h"
#include	"sysmng.h"


#define		NC		0xff

typedef struct {
	UINT8	f11[5];
	UINT8	f12[5];
} BINDTBL;

static const BINDTBL bindtbl = {
						//   �J�i  Stop  [��]  NFER  USER
							{0x72, 0x60, 0x4d, 0x51, 0x76},
						//         Copy  [�C]  XFER
							{NC,   0x61, 0x4f, 0x35, 0x77}};

void mackbd_resetf11(void) {

	UINT	i;

	for (i=1; i<(sizeof(bindtbl.f11)/sizeof(UINT8)); i++) {
		keystat_forcerelease(bindtbl.f11[i]);
	}
}

void mackbd_resetf12(void) {

	UINT	i;

	for (i=1; i<(sizeof(bindtbl.f12)/sizeof(UINT8)); i++) {
		keystat_forcerelease(bindtbl.f12[i]);
	}
}


typedef struct {
//	UINT32	tick;
	BYTE	keymap[16];
	BOOL	active;
//	UINT32	repbase;
//	UINT32	reptick;
//	BYTE	repkey;
	UINT32	modifier;
} MACKBD;

static	MACKBD		mackbd;

static const BYTE keymac[128] = {
			//	  �`,  �r,  �c,  �e,  �g,  �f,  �y,  �w		; 0x00
				0x1d,0x1e,0x1f,0x20,0x22,0x21,0x29,0x2a,
			//	  �b,  �u,    ,  �a,  �p,  �v,  �d,  �q		; 0x08
				0x2b,0x2c,  NC,0x2d,0x10,0x11,0x12,0x13,
			//	  �x,  �s,  �P,  �Q,  �R,  �S,  �U,  �T		; 0x10
				0x15,0x14,0x01,0x02,0x03,0x04,0x06,0x05,
			//	  �O,  �X,  �V,  �|,  �W,  �O,  �m,  �n 	; 0x18
				0x0c,0x09,0x07,0x0b,0x08,0x0a,0x1b,0x18,
			//	  �t,  ��,  �h,  �o, ret,  �k,  �i,  �F		; 0x20
				0x16,0x1a,0x17,0x19,0x1c,0x25,0x23,0x27,
			//	  �j,  �G,  �n,  �C,  �^,  �m,  �l,  �D		; 0x28
				0x24,0x26,0x28,0x30,0x32,0x2e,0x2f,0x31,
			//	 TAB, SPC,    ,  BS,    , ESC,    , apl		; 0x30
				0x0f,0x34,  NC,0x0e,  NC,0x00,  NC,  NC,
			//	 sft, cps, alt, ctl,    ,    ,    ,    		; 0x38
				0x70,0x79,0x73,0x74,  NC,  NC,  NC,  NC,
			//	    , [.],    , [*],    , [+],    ,    		; 0x40
				  NC,0x50,  NC,0x45,  NC,0x49,  NC,  NC,
			//	    ,    ,    , [/], ret,    , [-], clr		; 0x48
				  NC,  NC,  NC,0x41,0x1c,  NC,0x40,0x3e,
			//	    , [=], [0], [1], [2], [3], [4], [5]		; 0x50
				  NC,0x4d,0x4e,0x4a,0x4b,0x4c,0x46,0x47,
			//	 [6], [7],    , [8], [9],  ��,  �Q, [,]		; 0x58
				0x48,0x42,  NC,0x43,0x44,0x0d,0x33,0x4f,
			//	  F5,  F6,  F7,  F3,  F8,  F9,    , F11		; 0x60
				0x66,0x67,0x68,0x64,0x69,0x6a,  NC,  NC,
			//	kana, F13,    , F14,    , F10,    , F12		; 0x68
				0x72,  NC,  NC,  NC,  NC,0x6b,  NC,  NC,
			//	    , F15, hlp, hom,  ru, del,  F4, end		; 0x70
				  NC,  NC,0x3f,0x3e,0x37,0x39,0x65,0x3f,
			//	  F2,  rd,  F1,  ��,  ��,  ��,  ��,    		; 0x78
				0x63,0x36,0x62,0x3b,0x3c,0x3d,0x3a,  NC};

static const BYTE repkey[16] = {
		0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f,
		0xff, 0xff, 0xc1, 0xff, 0xc0, 0x0f, 0x07, 0x07};


void mackbd_initialize(void) {

//	mackbd.tick = GETTICK();
	ZeroMemory(&mackbd.keymap, sizeof(mackbd.keymap));
	mackbd.active = TRUE;
//	mackbd.repkey = NC;
	mackbd.modifier = 0;
}

BOOL mackbd_keydown(int keycode, BOOL cmd) {

	BYTE	data;

	data = NC;
	if (keycode == 0x67) {
		if (xmiloscfg.F11KEY < (sizeof(bindtbl.f11)/sizeof(UINT8))) {
			data = bindtbl.f11[xmiloscfg.F11KEY];
		}
	}
	else if (keycode == 0x6f) {
		if (xmiloscfg.F12KEY < (sizeof(bindtbl.f12)/sizeof(UINT8))) {
			data = bindtbl.f12[xmiloscfg.F12KEY];
            if (data == NC) {
                mousemng_toggle(MOUSEPROC_SYSTEM);
//                menu_setmouse(xmiloscfg.MOUSE_SW ^ 1);
                sysmng_update(SYS_UPDATECFG);
            }
		}
	}
    else if (keycode == 104) {
		data = 0x72 + 0x10;
	}
	else {
		data = keymac[keycode];
    }
	if (data != NC) {
		keystat_keydown(data);
		return(TRUE);
	}
	(void)cmd;
	return(FALSE);
}

BOOL mackbd_keyup(int keycode) {

	BYTE	data;

	data = NC;
	if (keycode == 0x67) {
		if (xmiloscfg.F11KEY < (sizeof(bindtbl.f11)/sizeof(UINT8))) {
			data = bindtbl.f11[xmiloscfg.F11KEY];
		}
	}
	else if (keycode == 0x6f) {
		if (xmiloscfg.F12KEY < (sizeof(bindtbl.f12)/sizeof(UINT8))) {
			data = bindtbl.f12[xmiloscfg.F12KEY];
		}
	}
    else if (keycode != 104) {
        data = keymac[keycode];
    }
	if (data != NC) {
		keystat_keyup(data);
		return(TRUE);
	}
	return(FALSE);
}

#if 0
void mackbd_activate(BOOL active) {

	if (mackbd.active != active) {
		mackbd.active = active;
		if (!active) {
			ZeroMemory(&mackbd.keymap, sizeof(mackbd.keymap));
			keystat_allrelease();
		}
	}
}
#endif

void mackbd_modifiers(UInt32 modif) {
	UInt32  change = mackbd.modifier ^ modif;
	
	if (change & shiftKey) {
		keystat_keydown(0x70 + 0x10);
	}
	if (change & optionKey) {
		keystat_keydown(0x73 + 0x10);
	}
	if (change & controlKey) {
		keystat_keydown(0x74 + 0x10);
	}
	if (change & alphaLock) {
		keystat_keydown(0x71 + 0x10);
	}
	mackbd.modifier = modif;
}
