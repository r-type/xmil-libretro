#include	"compiler.h"
#include	"pccore.h"
#include	"xmil.h"
#include	"joymng.h"
#include	"palmkbd.h"
#include	"palm5way.h"

typedef struct {
	UINT32	tick;
	BOOL	input;
	REG8	flag;
} JOYSTICK;

static	JOYSTICK	joy;


void joymng_initialize(void) {

	joy.input = FALSE;
	joy.flag = JOY_PADALL | JOY_BTNALL;
}

void joymng_sync(REG8 key) {

	joy.flag |= JOY_PADALL;
	joy.flag &= ~key;
}

void joymng_btnsync(REG8 key) {

	joy.flag |= JOY_BTNALL;
	joy.flag &= ~key;
	joy.tick = GETTICK();
	joy.input = TRUE;
}

REG8 joymng_getstat(void) {

	return(joy.flag);
}

void joymng_callback(void) {
	if (joy.input) {
		if ((GETTICK() - joy.tick) > 100) {
			joy.flag |= JOY_BTNALL;
			joy.input = FALSE;
		}
	}
}

static int setdirection(UINT data, int base) {

	if (data & navBitRight) {
		return(base+1);
	}
	else if (data & navBitLeft) {
		return(base-1);
	}
	else {
		return(base);
	}
}

BRESULT joymng_5way(UINT code) {

	int key;
	BRESULT ret = FALSE;
	
	if (xmiloscfg.FIVEWAY == 0) {
		joymng_sync(code & navBitsAll);
		ret = TRUE;
	}
	else if (xmiloscfg.FIVEWAY == 1) {
		if (code & navChangeUp) {
			if (code & navBitUp) {
				palmkbd_keydown(chrUpArrow, FALSE);
			}
			else {
				palmkbd_keyup(chrUpArrow);
			}
			ret = TRUE;
		} else if (code & navChangeDown) {
			if (code & navBitDown) {
				palmkbd_keydown(chrDownArrow, FALSE);
			}
			else {
				palmkbd_keyup(chrDownArrow);
			}
			ret = TRUE;
		}
		
		if (code & navChangeLeft) {
			if (code & navBitLeft) {
				palmkbd_keydown(chrLeftArrow, FALSE);
			}
			else {
				palmkbd_keyup(chrLeftArrow);
			}
			ret = TRUE;
		} else if (code & navChangeRight) {
			if (code & navBitRight) {
				palmkbd_keydown(chrRightArrow, FALSE);
			}
			else {
				palmkbd_keyup(chrRightArrow);
			}
			ret = TRUE;
		}
		
		if (code & navChangeSelect) {
			ret = TRUE;
		}
	}
	else if (xmiloscfg.FIVEWAY == 2) {
		if (code & navBitUp) {
			key = setdirection(code, 0x88);
		}
		else if (code & navBitDown) {
			key = setdirection(code, 0x82);
		}
		else {
			key = setdirection(code, 0x85);
			if ((key == 0x85) && !(code & navBitSelect)) {
				return(FALSE);
			}
		}
		palmkbd_keydown(key, TRUE);
		ret = TRUE;
	}

	return(ret);
}
