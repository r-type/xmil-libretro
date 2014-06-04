#include	"compiler.h"
#include	"z80core.h"
#include	"pccore.h"


REG8 MEMCALL mem_read8(UINT addr) {

	if (addr & 0x8000) {
		return(mainmem[addr]);
	}
	else {
		return(z80core.e.memread[addr]);
	}
}

SINT MEMCALL mem_read8s(UINT addr) {

	if (addr & 0x8000) {
		return((SINT)(SINT8)mainmem[addr]);
	}
	else {
		return((SINT)(SINT8)z80core.e.memread[addr]);
	}
}

void MEMCALL mem_write8(UINT addr, REG8 value) {

	if (addr & 0x8000) {
		mainmem[addr] = (UINT8)value;
	}
	else {
		z80core.e.memwrite[addr] = (UINT8)value;
	}
}

REG16 MEMCALL mem_read16(UINT addr) {

	if ((addr & 0x7fff) != 0x7fff) {
		if (addr & 0x8000) {
			return(LOADINTELWORD(mainmem + addr));
		}
		else {
			return(LOADINTELWORD(z80core.e.memread + addr));
		}
	}
	else {
		return(mem_read8(addr) + (mem_read8(LOW16(addr + 1)) << 8));
	}
}

void MEMCALL mem_write16(UINT addr, REG16 value) {

	if ((addr & 0x7fff) != 0x7fff) {
		if (addr & 0x8000) {
			STOREINTELWORD(mainmem + addr, value);
		}
		else {
			STOREINTELWORD(z80core.e.memwrite + addr, value);
		}
	}
	else {
		mem_write8(addr, (REG8)value);
		mem_write8(LOW16(addr + 1), (REG8)(value >> 8));
	}
}

