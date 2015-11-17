#include	"compiler.h"
#include	"pccore.h"
#include	"xmil.h"
#include	"joymng.h"
#include	"menu.h"


enum {
	JOY_LEFT_BIT	= 0x04,
	JOY_RIGHT_BIT	= 0x08,
	JOY_UP_BIT		= 0x01,
	JOY_DOWN_BIT	= 0x02,
	JOY_BTN1_BIT	= 0x40,
	JOY_BTN2_BIT	= 0x20,
	JOY_BTN3_BIT	= 0x80,
	JOY_BTN4_BIT	= 0x10
};

static	UINT8	joyflag;


void joymng_initialize(void) {

	JOYINFO		ji;

	if ((!joyGetNumDevs()) ||
		(joyGetPos(JOYSTICKID1, &ji) == JOYERR_UNPLUGGED)) {
		menu_setjoystick(xmiloscfg.JOYSTICK | 2);
	}
}

void joymng_sync(void) {

	xmiloscfg.JOYSTICK &= 0x7f;
	joyflag = 0xff;
}

REG8 joymng_getstat(void) {

	JOYINFO		ji;

	if ((xmiloscfg.JOYSTICK == 1) &&
		(joyGetPos(JOYSTICKID1, &ji) == JOYERR_NOERROR)) {
		xmiloscfg.JOYSTICK |= 0x80;
		joyflag = 0xff;
		if (ji.wXpos < 0x4000U) {
			joyflag &= ~JOY_LEFT_BIT;
		}
		else if (ji.wXpos > 0xc000U) {
			joyflag &= ~JOY_RIGHT_BIT;
		}
		if (ji.wYpos < 0x4000U) {
			joyflag &= ~JOY_UP_BIT;
		}
		else if (ji.wYpos > 0xc000U) {
			joyflag &= ~JOY_DOWN_BIT;
		}
		if (ji.wButtons & JOY_BUTTON1) {
			joyflag &= ~JOY_BTN1_BIT;
		}
		if (ji.wButtons & JOY_BUTTON2) {
			joyflag &= ~JOY_BTN2_BIT;
		}
		if (ji.wButtons & JOY_BUTTON3) {
			joyflag &= ~JOY_BTN3_BIT;
		}
		if (ji.wButtons & JOY_BUTTON4) {
			joyflag &= ~JOY_BTN4_BIT;
		}
	}
	return(joyflag);
}

