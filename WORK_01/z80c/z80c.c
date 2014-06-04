/* -----------------------------------------------------------------------
 *
 * Z80C : Z80 Engine - GENERIC
 *
 *                              Copyright by Studio Milmake 1999-2000,2004
 *
 *------------------------------------------------------------------------ */

#include	"compiler.h"
#include	"parts.h"
#include	"z80core.h"
#include	"z80c.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"z80c.mcr"


	Z80CORE	z80core;
	UINT8	mainmem[0x10000];

	UINT8 	z80inc_flag2[256];
	UINT8	z80dec_flag2[256];
	UINT8	z80szc_flag[512];
	UINT8	z80szp_flag[256];

	const UINT8 cycles_main[256] = {
					 4,10, 7, 6, 4, 4, 7, 4, 4,11, 7, 6, 4, 4, 7, 4,
					 8,10, 7, 6, 4, 4, 7, 4, 7,11, 7, 6, 4, 4, 7, 4,
					 7,10,16, 6, 4, 4, 7, 4, 7,11,16, 6, 4, 4, 7, 4,
					 7,10,13, 6,11,11,10, 4, 7,11,13, 6, 4, 4, 7, 4,
					 4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4,
					 4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4,
					 4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4,
					 7, 7, 7, 7, 7, 7, 4, 7, 4, 4, 4, 4, 4, 4, 7, 4,
					 4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4,
					 4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4,
					 4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4,
					 4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4,
					 5,10,10,10,10,11, 7,11, 5, 4,10, 0,10,10, 7,11,
					 5,10,10,11,10,11, 7,11, 5, 4,10,11,10, 0, 7,11,
					 5,10,10,19,10,11, 7,11, 5, 4,10, 4,10, 0, 7,11,
					 5,10,10, 4,10,11, 7,11, 5, 6,10, 4,10, 0, 7,11};

	const UINT8 cycles_xx[256] = {
					 0, 0, 0, 0, 0, 0, 0, 0, 0,15, 0, 0, 0, 0, 0, 0,
					 0, 0, 0, 0, 0, 0, 0, 0, 0,15, 0, 0, 0, 0, 0, 0,
					 0,14,20,10, 9, 9, 9, 0, 0,15,20,10, 9, 9, 9, 0,
					 0, 0, 0, 0,23,23,19, 0, 0,15, 0, 0, 0, 0, 0, 0,
					 0, 0, 0, 0, 9, 9,19, 0, 0, 0, 0, 0, 9, 9,19, 0,
					 0, 0, 0, 0, 9, 9,19, 0, 0, 0, 0, 0, 9, 9,19, 0,
					 9, 9, 9, 9, 9, 9,19, 9, 9, 9, 9, 9, 9, 9,19, 9,
					19,19,19,19,19,19,19,19, 0, 0, 0, 0, 9, 9,19, 0,
					 0, 0, 0, 0, 9, 9,19, 0, 0, 0, 0, 0, 9, 9,19, 0,
					 0, 0, 0, 0, 9, 9,19, 0, 0, 0, 0, 0, 9, 9,19, 0,
					 0, 0, 0, 0, 9, 9,19, 0, 0, 0, 0, 0, 9, 9,19, 0,
					 0, 0, 0, 0, 9, 9,19, 0, 0, 0, 0, 0, 9, 9,19, 0,
					 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					 0,14, 0,23, 0,15, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0,
					 0, 0, 0, 0, 0, 0, 0, 0, 0,10, 0, 0, 0, 0, 0, 0};

	const UINT8 cycles_ed[256] = {
					 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					12,12,15,20, 8, 8, 8, 9,12,12,15,20, 8, 8, 8, 9,
					12,12,15,20, 8, 8, 8, 9,12,12,15,20, 8, 8, 8, 9,
					12,12,15,20, 8, 8, 8,18,12,12,15,20, 8, 8, 8,18,
					12,12,15,20, 8, 8, 8, 0,12,12,15,20, 8, 8, 8, 0,
					 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					16,16,16,16, 0, 0, 0, 0,16,16,16,16, 0, 0, 0, 0,
					16,16,16,16, 0, 0, 0, 0,16,16,16,16, 0, 0, 0, 0,
					 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};


void CPUCALL z80c_initialize(void) {

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

		z80szp_flag[i] = (UINT8)f;

		z80inc_flag2[(i - 1) & 0xff] = (UINT8)(f & (~V_FLAG));
		if (!(i & 0x0f)) {
			z80inc_flag2[(i - 1) & 0xff] |= H_FLAG;
		}
		z80dec_flag2[(i + 1) & 0xff] = (UINT8)(f & (~V_FLAG)) | N_FLAG;
		if ((i & 0x0f) == 0x0f) {
			z80dec_flag2[(i + 1) & 0xff] |= H_FLAG;
		}

		z80szc_flag[i] = (UINT8)(f & (~V_FLAG));
		z80szc_flag[i+256] = (UINT8)(f & (~V_FLAG)) | C_FLAG;
	}
	z80inc_flag2[0x80 - 1] |= V_FLAG;
	z80dec_flag2[0x7f + 1] |= V_FLAG;
}

void CPUCALL z80c_reset(void) {

	ZeroMemory(&z80core.s, sizeof(z80core.s));
	R_Z80R = rand_get();
}

void CPUCALL z80c_interrupt(REG8 vect) {

	REG16	pc;

	if (Z80_IFF & (1 << IFF_HALT)) {
		Z80_IFF ^= (1 << IFF_HALT);
		R_Z80PC++;
	}
	Z80_IFF |= (1 << IFF_IFLAG);
	switch(R_Z80IM) {
		case 0:
			if ((vect != 0xdd) && (vect != 0xed) && (vect != 0xfd)) {
				Z80_COUNT(cycles_main[vect]);
				z80c_mainop[vect]();
			}
			break;

		case 1:
			Z80_COUNT(11);
			R_Z80SP -= 2;
			mem_write16(R_Z80SP, R_Z80PC);
			R_Z80PC = 0x38;
			break;

		case 2:
			pc = mem_read16((R_Z80I << 8) + vect);
			R_Z80SP -= 2;
			mem_write16(R_Z80SP, R_Z80PC);
			R_Z80PC = pc;
			break;
	}
}

void CPUCALL z80c_nonmaskedinterrupt(void) {

	if (!(Z80_IFF & (1 << IFF_NMI))) {
		Z80_IFF |= (1 << IFF_NMI);
		if (Z80_IFF & (1 << IFF_HALT)) {
			Z80_IFF ^= (1 << IFF_HALT);
			R_Z80PC++;
		}
		R_Z80SP -= 2;
		mem_write16(R_Z80SP, R_Z80PC);
		R_Z80PC = 0x66;
	}
}

void CPUCALL z80c_execute(void) {

	UINT	op;

#if !defined(DMAS_STOIC)
	if (!dma.working)
#else
	if (!(dma.flag & DMAF_WORKING))
#endif
	{
		do {
			R_Z80R++;
			GET_PC_BYTE(op);
			Z80_COUNT(cycles_main[op]);
			z80c_mainop[op]();
		} while(CPU_REMCLOCK > 0);
	}
	else {
		do {
			R_Z80R++;
			GET_PC_BYTE(op);
			Z80_COUNT(cycles_main[op]);
			z80c_mainop[op]();
			z80dmap();
		} while(CPU_REMCLOCK > 0);
	}
}

void CPUCALL z80c_step(void) {

	UINT	op;

	R_Z80R++;
	GET_PC_BYTE(op);
	Z80_COUNT(cycles_main[op]);
	z80c_mainop[op]();
	z80dmap();
}

