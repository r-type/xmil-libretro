#include	"compiler.h"
#include	"z80core.h"
#include	"pccore.h"


REG8 MEMCALL Z80_RDMEM(REG16 addr) {

	if (addr & 0x8000) {
		return(mMAIN[addr]);
	}
	else {
		return(RAM0r[addr]);
	}
}

void MEMCALL Z80_WRMEM(REG16 addr, REG8 value) {

	if (addr & 0x8000) {
		mMAIN[addr] = (UINT8)value;
	}
	else {
		RAM0w[addr] = (UINT8)value;
	}
}

REG16 MEMCALL Z80_RDMEM_W(REG16 addr) {

	if ((addr & 0x7fff) != 0x7fff) {
		if (addr & 0x8000) {
			return(LOADINTELWORD(mMAIN + addr));
		}
		else {
			return(LOADINTELWORD(RAM0r + addr));
		}
	}
	else {
		return(Z80_RDMEM(addr) + (Z80_RDMEM(LOW16(addr + 1)) << 8));
	}
}

void MEMCALL Z80_WRMEM_W(REG16 addr, REG16 value) {

	if ((addr & 0x7fff) != 0x7fff) {
		if (addr & 0x8000) {
			STOREINTELWORD(mMAIN + addr, value);
		}
		else {
			STOREINTELWORD(RAM0w + addr, value);
		}
	}
	else {
		Z80_WRMEM(addr, (REG8)value);
		Z80_WRMEM(LOW16(addr + 1), (REG8)(value >> 8));
	}
}

