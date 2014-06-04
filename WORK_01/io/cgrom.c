#include	"compiler.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"font.h"


void IOOUTCALL cgrom_o(UINT port, REG8 value) {

	switch(port) {
		case 0x0e80:
			cgrom.reg[0] = value;
			break;

		case 0x0e81:
			cgrom.reg[1] = value;
			break;

		case 0x0e82:
			cgrom.face = (LOADINTELWORD(cgrom.reg) & (~15));
			break;
	}
}

REG8 IOINPCALL cgrom_i(UINT port) {

	REG8	ret;

	if ((port & (~1)) != 0x0e80) {
		return(0xff);
	}
	ret = 0;
	if (cgrom.reg[1]) {
		if (!(port & 1)) {		/* 0x0e80 */
			ret = font_knjx1[cgrom.face + cgrom.count];
			cgrom.flag |= 1;
		}
		else {					/* 0x0e81 */
			ret = font_knjx1[cgrom.face + cgrom.count + FONTX1_LR];
			cgrom.flag |= 2;
		}
		/* 0x0e81 ポートリードの時だけインクリメントの筈だが…？ */
		if (cgrom.flag == 3) {
			cgrom.flag = 0;
			cgrom.count = (cgrom.count + 1) & 15;
		}
	}
	else {
		if (!(port & 1)) {		/* 0x0e80 */
			if (cgrom.reg[0] >= 0x30) {
				ret = 0x40 + ((cgrom.reg[0] - 0x30) * 6);
			}
			else {
				ret = 0x01 + ((cgrom.reg[0] - 0x21) * 6);
			}
		}
	}
	return(ret);
}


/* reset */

void cgrom_reset(void) {

	ZeroMemory(&cgrom, sizeof(cgrom));
}

