#include	"compiler.h"
#include	"z80core.h"
#include	"pccore.h"
#include	"iocore.h"


void memio_update(void) {

#if defined(SUPPORT_EMS)
	if (memio.ems & 0x10) {
#endif
		if (!memio.ram) {
			RAM0r = mBIOS;
			RAM0w = mMAIN;
		}
		else {
			RAM0r = RAM0w = mMAIN;
		}
#if defined(SUPPORT_EMS)
	}
	else {
		RAM0r = RAM0w = mBANK[memio.ems & 15];
	}
#endif
}

#if defined(SUPPORT_EMS)
void IOOUTCALL memio_ems_o(UINT port, REG8 dat) {

	memio.ems = dat;
	memio_update();
	(void)port;
}

REG8 IOINPCALL memio_ems_i(UINT port) {

	(void)port;
	return(memio.ems);
}
#endif

void IOOUTCALL memio_rom(UINT port, REG8 dat) {

	memio.ram = 0x00;
#if defined(SUPPORT_EMS)
	if (memio.ems & 0x10) {
#endif
		RAM0r = mBIOS;
#if defined(SUPPORT_EMS)
	}
#endif
	(void)port;
	(void)dat;
}

void IOOUTCALL memio_ram(UINT port, REG8 dat) {

	memio.ram = 0x10;
#if defined(SUPPORT_EMS)
	if (memio.ems & 0x10) {
#endif
		RAM0r = mMAIN;
#if defined(SUPPORT_EMS)
	}
#endif
	(void)port;
	(void)dat;
}


// ----

void memio_reset(void) {

	memio.ram = 0x00;
#if defined(SUPPORT_EMS)
	memio.ems = 0x78;
#endif
	memio_update();
}

