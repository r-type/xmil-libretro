#include	"compiler.h"
#include	"iocore.h"
#include	"advpsg.h"
#include	"makescrn.h"

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

static UINT		lastbtn;

void joymng_setflags(void) {

	REG16	flg;
	REG8	joy;

	flg = *REG_KEYINPUT;
	joy = 0xff;

	if (!(flg & ADVKEY_L)) {
		UINT modify;
		modify = lastbtn & (~flg);
		lastbtn = flg;
		if (modify & ADVKEY_RIGHT) {
			if (scrnpos.x < 80) scrnpos.x++;
		}
		if (modify & ADVKEY_LEFT) {
			if (scrnpos.x) scrnpos.x--;
		}
		if (modify & ADVKEY_UP) {
			if (scrnpos.y) scrnpos.y--;
		}
		if (modify & ADVKEY_DOWN) {
			if (scrnpos.y < 25) scrnpos.y++;
		}
		if (modify & (ADVKEY_RIGHT | ADVKEY_LEFT | ADVKEY_UP | ADVKEY_DOWN)) {
			crtc.e.scrnallflash = 1;
		}
		return;
	}
	lastbtn = 0;

	if (!(flg & ADVKEY_A)) {
		joy ^= JOY_BTN1_BIT;
	}
	if (!(flg & ADVKEY_B)) {
		joy ^= JOY_BTN2_BIT;
	}
	if (!(flg & ADVKEY_RIGHT)) {
		joy ^= JOY_RIGHT_BIT;
	}
	if (!(flg & ADVKEY_LEFT)) {
		joy	^= JOY_LEFT_BIT;
	}
	if (!(flg & ADVKEY_UP)) {
		joy ^= JOY_UP_BIT;
	}
	if (!(flg & ADVKEY_DOWN)) {
		joy ^= JOY_DOWN_BIT;
	}
	// ‚±‚Á‚¿‚©‚çƒZƒbƒg‚µ‚Ä‚â‚é
	advpsg.reg.io1 = joy;
}

