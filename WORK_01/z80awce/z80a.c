//----------------------------------------------------------------------------
//
//  Z80A : Z80 Engine - Strong ARM optimize
//
//                                  Copyright by Studio Milmake 1999-2000,2004
//
//----------------------------------------------------------------------------


#include	"compiler.h"
#include	"parts.h"
#include	"z80core.h"
#include	"pccore.h"
#include	"iocore.h"


	Z80CORE	z80core;
	UINT8	mainmem[0x10000];


void CPUCALL z80a_initialize(void) {

	UINT	i;
	REG8	f;
	REG8	c;

	for (i=0; i<256; i++) {
		f = V_FLAG;
		if (!i) {
			f |= Z_FLAG;
		}
		if (i & 0x80) {
			f |= S_FLAG;
		}
		for (c=0x80; c; c>>=1) {
			if (i & c) {
				f ^= V_FLAG;
			}
		}
		z80core.f.szp[i] = (UINT8)f;

		z80core.f.inc[(i - 1) & 0xff] = (UINT8)(f & (~V_FLAG));
		if (!(i & 0x0f)) {
			z80core.f.inc[(i - 1) & 0xff] |= H_FLAG;
		}
		z80core.f.dec[(i + 1) & 0xff] = (UINT8)(f & (~V_FLAG)) | N_FLAG;
		if ((i & 0x0f) == 0x0f) {
			z80core.f.dec[(i + 1) & 0xff] |= H_FLAG;
		}
	}
	z80core.f.inc[0x80 - 1] |= V_FLAG;
	z80core.f.dec[0x7f + 1] |= V_FLAG;
}

void CPUCALL z80a_reset(void) {

	ZeroMemory(&z80core.s, sizeof(z80core.s));
	z80core.s.r1 = rand_get();
	z80core.e.membase = mainmem;
}

