/* -----------------------------------------------------------------------
 *
 * Z80C : Z80 Engine - GENERIC
 *
 *                              Copyright by Studio Milmake 1999-2000,2004
 *
 *------------------------------------------------------------------------ */

#include	"compiler.h"
#include	"z80core.h"
#include	"z80c.h"
#include	"z80c.mcr"


static REG8 CPUCALL _cb_rlc(REG8 v) {

	REG8	ret;

	R_Z80F = v >> 7;
	ret = (v << 1) | R_Z80F;
	R_Z80F |= z80szp_flag[(UINT8)ret];
	return(ret);
}

static REG8 CPUCALL _cb_rrc(REG8 v) {

	REG8	ret;

	R_Z80F = v & 1;
	ret = (v >> 1) | (R_Z80F << 7);
	R_Z80F |= z80szp_flag[(UINT8)ret];
	return(ret);
}

static REG8 CPUCALL _cb_rl(REG8 v) {

	REG8	ret;

	ret = (v << 1) | (R_Z80F & 1);
	R_Z80F = z80szp_flag[(UINT8)ret] | (v >> 7);
	return(ret);
}

static REG8 CPUCALL _cb_rr(REG8 v) {

	REG8	ret;

	ret = (v >> 1) | (R_Z80F << 7);
	R_Z80F = z80szp_flag[(UINT8)ret] | (v & 1);
	return(ret);
}

static REG8 CPUCALL _cb_sla(REG8 v) {

	REG8	ret;

	ret = (v << 1);
	R_Z80F = z80szp_flag[(UINT8)ret] | (v >> 7);
	return(ret);
}

static REG8 CPUCALL _cb_sra(REG8 v) {

	REG8	ret;

	ret = (((SINT8)v) >> 1);
	R_Z80F = z80szp_flag[(UINT8)ret] | (v & 1);
	return(ret);
}

static REG8 CPUCALL _cb_sll(REG8 v) {

	REG8	ret;

	ret = (v << 1) | 1;
	R_Z80F = z80szp_flag[(UINT8)ret] | (v >> 7);
	return(ret);
}

static REG8 CPUCALL _cb_srl(REG8 v) {

	REG8	ret;

	ret = v >> 1;
	R_Z80F = z80szp_flag[ret] | (v & 1);
	return(ret);
}


static REG8 (CPUCALL * rolsft_proc[8])(REG8 value) = {
		_cb_rlc,	_cb_rrc,	_cb_rl,		_cb_rr,
		_cb_sla,	_cb_sra,	_cb_sll,	_cb_srl};

static UINT8 *cb_reg[8] = {
		&R_Z80B,	&R_Z80C,	&R_Z80D,	&R_Z80E,
		&R_Z80H,	&R_Z80L,	NULL,		&R_Z80A};


void z80c_cb(void) {

	UINT	op;
	UINT8	*reg;
	REG8	xhl;
	UINT	bit;

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

			case 0x40:										/* bit */
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

			case 0x80:										/* reset */
				(*reg) &= ~(1 << bit);
				break;

			case 0xc0:										/* set */
				(*reg) |= 1 << bit;
				break;
		}
	}
	else {
		xhl = mem_read8(R_Z80HL);
		switch(op & 0xc0) {
			case 0x00:
				Z80_COUNT(15);
				xhl = rolsft_proc[bit](xhl);
				break;

			case 0x40:										/* bit */
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

			case 0x80:										/* reset */
				Z80_COUNT(15);
				xhl &= ~(1 << bit);
				break;

			case 0xc0:										/* set */
				Z80_COUNT(15);
				xhl |= 1 << bit;
				break;
		}
		mem_write8(R_Z80HL, xhl);
	}
}

void z80c_ixcb(void) {

	UINT	op;
	UINT	adrs;
	REG8	xi;
	UINT	bit;

	R_Z80R++;
	adrs = LOW16(mem_read8s(R_Z80PC++) + R_Z80IX);
	xi = mem_read8(adrs);
	GET_PC_BYTE(op);
	bit = (op >> 3) & 7;
	switch(op & 0xc0) {
		case 0x00:
			Z80_COUNT(23);
			xi = rolsft_proc[bit](xi);
			break;

		case 0x40:										/* bit */
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

		case 0x80:										/* reset */
			Z80_COUNT(23);
			xi &= ~(1 << bit);
			break;

		case 0xc0:										/* set */
			Z80_COUNT(23);
			xi |= 1 << bit;
			break;
	}
	mem_write8(adrs, xi);
}

void z80c_iycb(void) {

	UINT	op;
	UINT	adrs;
	REG8	xi;
	UINT	bit;

	R_Z80R++;
	adrs = LOW16(mem_read8s(R_Z80PC++) + R_Z80IY);
	xi = mem_read8(adrs);
	GET_PC_BYTE(op);
	bit = (op >> 3) & 7;
	switch(op & 0xc0) {
		case 0x00:
			Z80_COUNT(23);
			xi = rolsft_proc[bit](xi);
			break;

		case 0x40:										/* bit */
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

		case 0x80:										/* reset */
			Z80_COUNT(23);
			xi &= ~(1 << bit);
			break;

		case 0xc0:										/* set */
			Z80_COUNT(23);
			xi |= 1 << bit;
			break;
	}
	mem_write8(adrs, xi);
}

