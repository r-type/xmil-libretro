#include	"compiler.h"
#include	"z80core.h"
#include	"pccore.h"


REG8 MEMCALL mem_read8(UINT addr) {

	if (addr & 0x8000) {
		return(mMAIN[addr]);
	}
	else {
		return(RAM0r[addr]);
	}
}

SINT MEMCALL mem_read8s(UINT addr) {

	if (addr & 0x8000) {
		return((SINT)(SINT8)mMAIN[addr]);
	}
	else {
		return((SINT)(SINT8)RAM0r[addr]);
	}
}

void MEMCALL mem_write8(UINT addr, REG8 value) {

	if (addr & 0x8000) {
		mMAIN[addr] = (UINT8)value;
	}
	else {
		RAM0w[addr] = (UINT8)value;
	}
}

REG16 MEMCALL mem_read16(UINT addr) {

	if ((addr & 0x7fff) != 0x7fff) {
		if (addr & 0x8000) {
			return(LOADINTELWORD(mMAIN + addr));
		}
		else {
			return(LOADINTELWORD(RAM0r + addr));
		}
	}
	else {
		return(mem_read8(addr) + (mem_read8(LOW16(addr + 1)) << 8));
	}
}

void MEMCALL mem_write16(UINT addr, REG16 value) {

	if ((addr & 0x7fff) != 0x7fff) {
		if (addr & 0x8000) {
			STOREINTELWORD(mMAIN + addr, value);
		}
		else {
			STOREINTELWORD(RAM0w + addr, value);
		}
	}
	else {
		mem_write8(addr, (REG8)value);
		mem_write8(LOW16(addr + 1), (REG8)(value >> 8));
	}
}

