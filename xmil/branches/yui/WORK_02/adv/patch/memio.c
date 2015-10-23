#include	"compiler.h"
#include	"z80core.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"nevent.h"


#if defined(SUPPORT_BANKMEM)
#error	defined SUPPORT_BANKMEM!
#endif

static	const UINT8	*ipl;

void memio_update(void) {

	if (!(iocore.s.ppib & 0x10)) {
		z80core.e.memread = ipl;
	}
	else {
		z80core.e.memread = mainmem;
	}
}

void IOOUTCALL memio_rom(UINT port, REG8 dat) {

	if (iocore.s.ppib & 0x10) {
		iocore.s.ppib &= ~0x10;
		z80core.e.memread = ipl;
		nevent_forceexit();
	}
	(void)port;
	(void)dat;
}

void IOOUTCALL memio_ram(UINT port, REG8 dat) {

	if (!(iocore.s.ppib & 0x10)) {
		iocore.s.ppib |= 0x10;
		z80core.e.memread = mainmem;
		nevent_forceexit();
	}
	(void)port;
	(void)dat;
}


// ----

void memio_reset(void) {

	if (pccore.ROM_TYPE >= 2) {
		ipl = __extromimage.bios2;
	}
	else {
		ipl = __extromimage.bios1;
	}
	memio_update();
#if defined(ADV_SIMULATE)
	z80core.e.memwrite = mainmem;
#endif
}

