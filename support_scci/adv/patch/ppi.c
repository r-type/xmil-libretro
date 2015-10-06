#include	"compiler.h"
#include	"z80core.h"
#include	"pccore.h"
#include	"iocore.h"


// ---- 8255 PPIÅ`

static void setportc(REG8 dat) {

	REG8	modify;

	modify = ppi.portc ^ dat;
	ppi.portc = dat;
//	cmt_write((REG8)(dat & 1));
	if ((modify & 0x20) && (!(dat & 0x20))) {
		iocore.s.mode = 1;
	}
	if (modify & 0x40) {
		crtc_setwidth((REG8)(dat & 0x40));
	}
}


// ----

void IOOUTCALL ppi_o(UINT port, REG8 value) {

	REG8	bit;

	switch(port & 0x0f) {
		case 0:
			ppi.porta = value;
			return;

		case 1:
			ppi.portb = value;
			return;

		case 2:
			setportc(value);
			break;

		case 3:
			if (value & 0x80) {
				ppi.mode = value;
				return;
			}
			else {
				bit = 1 << ((value >> 1) & 7);
				if (value & 0x01) {
					setportc((REG8)(ppi.portc | bit));
				}
				else {
					setportc((REG8)(ppi.portc & (~bit)));
				}
			}
			break;
	}
}


// ----

void ppi_initialize(void) {

	ppi.porta = 0x00;
	ppi.portb = 0xff;
	ppi.portc = 0xff;
	ppi.mode = 0x82;
}

void ppi_reset(void) {

	ppi.porta = 0x00;
	ppi.portc |= 0x40;
	ppi.mode = 0x82;
}

