//----------------------------------------------------------------------------
//
//  Z80C : Z80 Engine - GENERIC
//
//                                  Copyright by Studio Milmake 1999-2000,2004
//
//----------------------------------------------------------------------------


#include	"compiler.h"
#include	"z80core.h"
#include	"z80c.h"
#include	"x1_io.h"
#include	"z80c.mcr"


static BYTE CPUCALL _cb_rlc(BYTE v) {

	BYTE	ret;

	R_Z80F = v >> 7;
	ret = (v << 1) | R_Z80F;
	R_Z80F |= ZSPtable[ret];
	return(ret);
}

static BYTE CPUCALL _cb_rrc(BYTE v) {

	BYTE	ret;

	R_Z80F = v & 1;
	ret = (v >> 1) | (R_Z80F << 7);
	R_Z80F |= ZSPtable[ret];
	return(ret);
}

static BYTE CPUCALL _cb_rl(BYTE v) {

	BYTE	ret;

	ret = (v << 1) | (R_Z80F & 1);
	R_Z80F = ZSPtable[ret] | (v >> 7);
	return(ret);
}

static BYTE CPUCALL _cb_rr(BYTE v) {

	BYTE	ret;

	ret = (v >> 1) | (R_Z80F << 7);
	R_Z80F = ZSPtable[ret] | (v & 1);
	return(ret);
}

static BYTE CPUCALL _cb_sla(BYTE v) {

	BYTE	ret;

	ret = (v << 1);
	R_Z80F = ZSPtable[ret] | (v >> 7);
	return(ret);
}

static BYTE CPUCALL _cb_sra(BYTE v) {

	BYTE	ret;

	ret = (((char)v) / 2);
	R_Z80F = ZSPtable[ret] | (v & 1);
	return(ret);
}

static BYTE CPUCALL _cb_sll(BYTE v) {

	BYTE	ret;

	ret = (v << 1) | 1;
	R_Z80F = ZSPtable[ret] | (v >> 7);
	return(ret);
}

static BYTE CPUCALL _cb_srl(BYTE v) {

	BYTE	ret;

	ret = v >> 1;
	R_Z80F = ZSPtable[ret] | (v & 1);
	return(ret);
}


static BYTE (CPUCALL * rolsft_proc[8])(BYTE value) = {
		_cb_rlc,	_cb_rrc,	_cb_rl,		_cb_rr,
		_cb_sla,	_cb_sra,	_cb_sll,	_cb_srl};

static BYTE *cb_reg[8] = {
		&R_Z80B,	&R_Z80C,	&R_Z80D,	&R_Z80E,
		&R_Z80H,	&R_Z80L,	NULL,		&R_Z80A};


void z80c_cb(void) {

	BYTE	op;
	BYTE	*reg;
	BYTE	xhl;
	int		bit;

	R_Z80R++;
	GET_PC_BYTE(op);

	reg = cb_reg[op & 7];
	bit = (op >> 3) & 7;
	if (reg) {
		Z80_COUNT(8);
		switch(op & 0xc0) {
			case 0x00:
				*reg = rolsft_proc[bit](*reg);
				break;

			case 0x40:										// bit
				R_Z80F &= C_FLAG;
				if ((*reg) & (1 << bit)) {
					if (bit != 7) {
						R_Z80F |= H_FLAG;
					}
					else {
						R_Z80F |= (H_FLAG | S_FLAG);
					}
				}
				else {
					R_Z80F |= (H_FLAG | Z_FLAG);
				}
				break;

			case 0x80:										// reset
				(*reg) &= ~(1 << bit);
				break;

			case 0xc0:										// set
				(*reg) |= 1 << bit;
				break;
		}
	}
	else {
		xhl = Z80_RDMEM(R_Z80HL);
		switch(op & 0xc0) {
			case 0x00:
				Z80_COUNT(15);
				xhl = rolsft_proc[bit](xhl);
				break;

			case 0x40:										// bit
				Z80_COUNT(12);
				R_Z80F &= C_FLAG;
				if (xhl & (1 << bit)) {
					if (bit != 7) {
						R_Z80F |= H_FLAG;
					}
					else {
						R_Z80F |= (H_FLAG | S_FLAG);
					}
				}
				else {
					R_Z80F |= (H_FLAG | Z_FLAG);
				}
				return;

			case 0x80:										// reset
				Z80_COUNT(15);
				xhl &= ~(1 << bit);
				break;

			case 0xc0:										// set
				Z80_COUNT(15);
				xhl |= 1 << bit;
				break;
		}
		Z80_WRMEM(R_Z80HL, xhl);
	}
}

void z80c_ixcb(void) {

	BYTE	op;
	WORD	adrs;
	BYTE	xi;
	int		bit;

	R_Z80R++;
	adrs = __CBW(Z80_RDMEM(R_Z80PC++)) + R_Z80IX;
	xi = Z80_RDMEM(adrs);
	GET_PC_BYTE(op);
	bit = (op >> 3) & 7;
	switch(op & 0xc0) {
		case 0x00:
			Z80_COUNT(23);
			xi = rolsft_proc[bit](xi);
			break;

		case 0x40:										// bit
			Z80_COUNT(20);
			R_Z80F &= C_FLAG;
			if (xi & (1 << bit)) {
				if (bit != 7) {
					R_Z80F |= H_FLAG;
				}
				else {
					R_Z80F |= (H_FLAG | S_FLAG);
				}
			}
			else {
				R_Z80F |= (H_FLAG | Z_FLAG);
			}
			return;

		case 0x80:										// reset
			Z80_COUNT(23);
			xi &= ~(1 << bit);
			break;

		case 0xc0:										// set
			Z80_COUNT(23);
			xi |= 1 << bit;
			break;
	}
	Z80_WRMEM(adrs, xi);
}

void z80c_iycb(void) {

	BYTE	op;
	WORD	adrs;
	BYTE	xi;
	int		bit;

	R_Z80R++;
	adrs = __CBW(Z80_RDMEM(R_Z80PC++)) + R_Z80IY;
	xi = Z80_RDMEM(adrs);
	GET_PC_BYTE(op);
	bit = (op >> 3) & 7;
	switch(op & 0xc0) {
		case 0x00:
			Z80_COUNT(23);
			xi = rolsft_proc[bit](xi);
			break;

		case 0x40:										// bit
			Z80_COUNT(20);
			R_Z80F &= C_FLAG;
			if (xi & (1 << bit)) {
				if (bit != 7) {
					R_Z80F |= H_FLAG;
				}
				else {
					R_Z80F |= (H_FLAG | S_FLAG);
				}
			}
			else {
				R_Z80F |= (H_FLAG | Z_FLAG);
			}
			return;

		case 0x80:										// reset
			Z80_COUNT(23);
			xi &= ~(1 << bit);
			break;

		case 0xc0:										// set
			Z80_COUNT(23);
			xi |= 1 << bit;
			break;
	}
	Z80_WRMEM(adrs, xi);
}

