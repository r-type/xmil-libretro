#include	"compiler.h"
#include	"pccore.h"
#include	"xmil.h"
#include	"joymng.h"
#include	"palmkbd.h"
#include	"PalmNavigator.h"

typedef struct {
	UINT32	input;
	REG8	flag;
} JOYSTICK;

static	JOYSTICK	joy;


void joymng_initialize(void) {

	joy.flag = JOY_PADALL | JOY_BTNALL;
}

void joymng_sync(REG8 key) {

	joy.flag &= JOY_BTNALL;
	joy.flag |= ~key;
}

REG8 joymng_getstat(void) {

	return(joy.flag);
}

static const UINT hardkey[] = {0, 0,0x20,0x0d,0x1b,0x8a,0x8b,0x91,0x92,0x93,0x94,0x95};
static const REG8 joybit[] = {JOY_BTN1_BIT, JOY_BTN2_BIT};

static void setjoybutton (UINT32 keystate, REG8 keycode, UINT keybit) {
	if (keystate & keybit) {
		joy.flag &= ~joybit[keycode];
	} else {
		joy.flag |= joybit[keycode];
	}
}

static void setkeybutton (UINT32 keystate, REG8 keycode, UINT keybit) {
	if (keystate & keybit) {
		palmkbd_keydown(hardkey[keycode], TRUE);
	} else {
		palmkbd_keyup(hardkey[keycode]);
	}
}

static void setbutton (UINT32 state, REG8 keycode, UINT keybit) {

	switch (keycode) {
		case 0:
		case 1:
			setjoybutton(state, keycode, keybit);
			break;
		default:
			setkeybutton(state, keycode, keybit);
			break;
	}
}

void joymng_hardkey(UINT32 state) {

	setbutton(state, xmiloscfg.HARDKEY1, keyBitHard1);
	setbutton(state, xmiloscfg.HARDKEY2, keyBitHard2);
	setbutton(state, xmiloscfg.HARDKEY3, keyBitHard3);
	setbutton(state, xmiloscfg.HARDKEY4, keyBitHard4);
}

static const UINT navbits[] = { JOY_UP_BIT,	JOY_DOWN_BIT,	JOY_LEFT_BIT,	JOY_RIGHT_BIT,
								chrUpArrow,	chrDownArrow,	chrLeftArrow,	chrRightArrow};

static int setdirection(UINT data, int base) {

	if (data & JOY_RIGHT_BIT) {
		return(base+1);
	}
	else if (data & JOY_LEFT_BIT) {
		return(base-1);
	}
	else {
		return(base);
	}
}

BRESULT joymng_5way(UINT code) {

	int key, i;
	BRESULT ret = FALSE;
	
	if (xmiloscfg.FIVEWAY == 0) {
		joymng_sync(code & navBitsAll);
		ret = TRUE;
	}
	else if (xmiloscfg.FIVEWAY == 1) {
		for (i=0;i<4;i++) {
			if (code & navbits[i]) {
				palmkbd_keydown(navbits[i+4], TRUE);
			}
		}
		ret = TRUE;
	}
	else if (xmiloscfg.FIVEWAY == 2) {
		if (code & JOY_UP_BIT) {
			key = setdirection(code, 0x88);
		}
		else if (code & JOY_DOWN_BIT) {
			key = setdirection(code, 0x82);
		}
		else {
			key = setdirection(code, 0x85);
			if ((key == 0x85) && !(code & navBitSelect)) {
				return(ret);
			}
		}
		palmkbd_keydown(key, TRUE);
		ret = TRUE;
	}

	return(ret);
}
