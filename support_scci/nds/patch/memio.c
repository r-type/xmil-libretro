#include	"compiler.h"
#include	"z80core.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"nevent.h"


#if defined(SUPPORT_BANKMEM)
#error	defined SUPPORT_BANKMEM!
#endif

static const UINT8 *s_pcsIpl;

void memio_update(void)
{
	if (!(iocore.s.ppib & 0x10))
	{
		z80core.e.memread = s_pcsIpl;
	}
	else
	{
		z80core.e.memread = mainmem;
	}
}

void IOOUTCALL memio_rom(UINT uPort, REG8 cValue)
{
	if (iocore.s.ppib & 0x10)
	{
		iocore.s.ppib &= ~0x10;
		z80core.e.memread = s_pcsIpl;
		nevent_forceexit();
	}
	(void)uPort;
	(void)cValue;
}

void IOOUTCALL memio_ram(UINT uPort, REG8 cValue)
{
	if (!(iocore.s.ppib & 0x10))
	{
		iocore.s.ppib |= 0x10;
		z80core.e.memread = mainmem;
		nevent_forceexit();
	}
	(void)uPort;
	(void)cValue;
}


/* reset */

void memio_reset(void)
{
	if (pccore.ROM_TYPE >= 2)
	{
		s_pcsIpl = __extromimage.bios2;
	}
	else
	{
		s_pcsIpl = __extromimage.bios1;
	}
	memio_update();
}

