#include	"compiler.h"
#include	"dosio.h"
#include	"z80core.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"defipl.res"


	UINT8	biosmem[0x8000];
#if defined(SUPPORT_BANKMEM)
	UINT8	bankmem[16][0x8000];
#endif


void memio_update(void) {

#if defined(SUPPORT_BANKMEM)
	if (iocore.s.bankmem & 0x10) {
#endif
		if (!(iocore.s.ppib & 0x10)) {
			z80core.e.memread = biosmem;
		}
		else {
			z80core.e.memread = mainmem;
		}
		z80core.e.memwrite = mainmem;
#if defined(SUPPORT_BANKMEM)
	}
	else {
		z80core.e.memread = bankmem[iocore.s.bankmem & 15];
		z80core.e.memwrite = bankmem[iocore.s.bankmem & 15];
	}
#endif
}

#if defined(SUPPORT_BANKMEM)
void IOOUTCALL memio_bank_o(UINT port, REG8 dat) {

	iocore.s.bankmem = dat;
	memio_update();
	(void)port;
}

REG8 IOINPCALL memio_bank_i(UINT port) {

	(void)port;
	return(iocore.s.bankmem);
}
#endif

void IOOUTCALL memio_rom(UINT port, REG8 dat) {

	iocore.s.ppib &= ~0x10;
#if defined(SUPPORT_BANKMEM)
	if (iocore.s.bankmem & 0x10) {
#endif
		z80core.e.memread = biosmem;
#if defined(SUPPORT_BANKMEM)
	}
#endif
	(void)port;
	(void)dat;
}

void IOOUTCALL memio_ram(UINT port, REG8 dat) {

	iocore.s.ppib |= 0x10;
#if defined(SUPPORT_BANKMEM)
	if (iocore.s.bankmem & 0x10) {
#endif
		z80core.e.memread = mainmem;
#if defined(SUPPORT_BANKMEM)
	}
#endif
	(void)port;
	(void)dat;
}


/* ---- */

static const OEMCHAR iplromx1[] = OEMTEXT("IPLROM.X1");
static const OEMCHAR iplromx1t[] = OEMTEXT("IPLROM.X1T");

void memio_reset(void) {

const OEMCHAR	*iplfile;
	UINT		iplsize;
	FILEH		fh;

	ZeroMemory(biosmem, 0x8000);
	CopyMemory(biosmem, defaultiplrom, sizeof(defaultiplrom));
	if (pccore.ROM_TYPE >= 1) {
		if (pccore.ROM_TYPE >= 2) {
			iplfile = iplromx1t;
			iplsize = 0x8000;
		}
		else {
			iplfile = iplromx1;
			iplsize = 0x8000;
		}
		fh = file_open_rb_c(iplfile);
		if (fh != FILEH_INVALID) {
			file_read(fh, biosmem, iplsize);
			file_close(fh);
		}
	}

#if defined(SUPPORT_BANKMEM)
	iocore.s.bankmem = 0x78;
#endif
	memio_update();
}

