#include	"compiler.h"
#include	"z80core.h"
#include	"pccore.h"
#include	"iocore.h"


void memio_update(void) {

#if defined(SUPPORT_BANKMEM)
	if (memio.bank & 0x10) {
#endif
		if (!memio.ram) {
			RAM0r = mBIOS;
			RAM0w = mMAIN;
		}
		else {
			RAM0r = RAM0w = mMAIN;
		}
#if defined(SUPPORT_BANKMEM)
	}
	else {
		RAM0r = RAM0w = mBANK[memio.bank & 15];
	}
#endif
}

#if defined(SUPPORT_BANKMEM)
void IOOUTCALL memio_bank_o(UINT port, REG8 dat) {

	memio.bank = dat;
	memio_update();
	(void)port;
}

REG8 IOINPCALL memio_bank_i(UINT port) {

	(void)port;
	return(memio.bank);
}
#endif

void IOOUTCALL memio_rom(UINT port, REG8 dat) {

	memio.ram = 0x00;
#if defined(SUPPORT_BANKMEM)
	if (memio.bank & 0x10) {
#endif
		RAM0r = mBIOS;
#if defined(SUPPORT_BANKMEM)
	}
#endif
	(void)port;
	(void)dat;
}

void IOOUTCALL memio_ram(UINT port, REG8 dat) {

	memio.ram = 0x10;
#if defined(SUPPORT_BANKMEM)
	if (memio.bank & 0x10) {
#endif
		RAM0r = mMAIN;
#if defined(SUPPORT_BANKMEM)
	}
#endif
	(void)port;
	(void)dat;
}


// ----

void memio_reset(void) {

	memio.ram = 0x00;
#if defined(SUPPORT_BANKMEM)
	memio.bank = 0x78;
#endif
	memio_update();
}

