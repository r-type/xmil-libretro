#include	"compiler.h"
#include	"xmil.h"
#include	"mousemng.h"


typedef struct {
	SINT16	x;
	SINT16	y;
	UINT8	btn;
	UINT	flag;
} MOUSEMNG;

static	MOUSEMNG	mousemng;


REG8 mousemng_getstat(SINT16 *x, SINT16 *y, BRESULT clear) {

	*x = mousemng.x;
	*y = mousemng.y;
	if (clear) {
		mousemng.x = 0;
		mousemng.y = 0;
	}
	return(mousemng.btn ^ 3);
}


// ----

void mousemng_initialize(void) {

	ZeroMemory(&mousemng, sizeof(mousemng));
	mousemng.btn = uPD8255A_LEFTBIT | uPD8255A_RIGHTBIT;
	mousemng.flag = (1 << MOUSEPROC_SYSTEM);
}

BRESULT mousemng_buttonevent(UINT event) {

	if (!mousemng.flag) {
		switch(event) {
			case MOUSEMNG_LEFTDOWN:
				mousemng.btn &= ~(uPD8255A_LEFTBIT);
				break;

			case MOUSEMNG_LEFTUP:
				mousemng.btn |= uPD8255A_LEFTBIT;
				break;

			case MOUSEMNG_RIGHTDOWN:
				mousemng.btn &= ~(uPD8255A_RIGHTBIT);
				break;

			case MOUSEMNG_RIGHTUP:
				mousemng.btn |= uPD8255A_RIGHTBIT;
				break;
		}
		return(TRUE);
	}
	else {
		return(FALSE);
	}
}

void mousemng_enable(UINT proc) {

	UINT	bit;

	bit = 1 << proc;
	if (mousemng.flag & bit) {
		mousemng.flag &= ~bit;
	}
}

void mousemng_disable(UINT proc) {

	mousemng.flag |= (1 << proc);
}

void mousemng_toggle(UINT proc) {

	mousemng.flag ^= (1 << proc);
}
