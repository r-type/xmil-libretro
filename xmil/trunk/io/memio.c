#include	"compiler.h"
#include	"z80core.h"
#include	"pccore.h"
#include	"iocore.h"


void memio_update(void) {

	if (memio.ems & 0x10) {
		if (!memio.ram) {
			RAM0r = mBIOS;
			RAM0w = mMAIN;
		}
		else {
			RAM0r = RAM0w = mMAIN;
		}
	}
	else {
		RAM0r = RAM0w = mBANK[memio.ems & 15];
	}
}

void IOOUTCALL memio_ems_o(UINT port, REG8 dat) {

	memio.ems = dat;
	memio_update();
	(void)port;
}

REG8 IOINPCALL memio_ems_i(UINT port) {

	(void)port;
	return(memio.ems);
}

void IOOUTCALL memio_rom(UINT port, REG8 dat) {

	memio.ram = 0x00;
	if (memio.ems & 0x10) {
		RAM0r = mBIOS;
	}
	(void)port;
	(void)dat;
}

void IOOUTCALL memio_ram(UINT port, REG8 dat) {

	memio.ram = 0x10;
	if (memio.ems & 0x10) {
		RAM0r = mMAIN;
	}
	(void)port;
	(void)dat;
}


// ----

void memio_reset(void) {

	memio.ram = 0x00;
	memio.ems = 0x78;
	memio_update();
}

