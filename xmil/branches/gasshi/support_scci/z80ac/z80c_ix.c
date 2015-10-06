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
#include	"z80c_i.mcr"


#define	R_Z80IXL	z80core.s.r.b.ixl
#define	R_Z80IXH	z80core.s.r.b.ixh
#undef	R_Z80IY


Z80FN _ld_nop(void)			{ }


Z80FN _no_op(void)			MCR_DEC_W(R_Z80PC)
Z80FN _add_ix_bc(void)		MCR_ADD_W(R_Z80IX, R_Z80BC)

Z80FN _add_ix_de(void)		MCR_ADD_W(R_Z80IX, R_Z80DE)

Z80FN _ld_ix_word(void)		LDW_w(R_Z80IX)
Z80FN _ld_xword_ix(void)	LDx_W(R_Z80IX)
Z80FN _inc_ix(void)			MCR_INC_W(R_Z80IX)
Z80FN _inc_ixh(void)		MCR_INC(R_Z80IXH)
Z80FN _dec_ixh(void)		MCR_DEC(R_Z80IXH)
Z80FN _ld_ixh_byte(void)	LDB_b(R_Z80IXH)
Z80FN _add_ix_ix(void)		MCR_ADDx2(R_Z80IX)
Z80FN _ld_ix_xword(void)	LDW_x(R_Z80IX)
Z80FN _dec_ix(void)			MCR_DEC_W(R_Z80IX)
Z80FN _inc_ixl(void)		MCR_INC(R_Z80IXL)
Z80FN _dec_ixl(void)		MCR_DEC(R_Z80IXL)
Z80FN _ld_ixl_byte(void)	LDB_b(R_Z80IXL)

Z80FN _inc_xix(void)		MCR_INC_XI(R_Z80IX)
Z80FN _dec_xix(void)		MCR_DEC_XI(R_Z80IX)
Z80FN _ld_xix_byte(void)	MCR_LDXIBYTE(R_Z80IX)
Z80FN _add_ix_sp(void)		MCR_ADD_W(R_Z80IX, R_Z80SP)

Z80FN _ld_b_ixh(void)		MCR_LD(R_Z80B, R_Z80IXH)
Z80FN _ld_b_ixl(void)		MCR_LD(R_Z80B, R_Z80IXL)
Z80FN _ld_b_xix(void)		MCR_LD_RXI(R_Z80B, R_Z80IX)
Z80FN _ld_c_ixh(void)		MCR_LD(R_Z80C, R_Z80IXH)
Z80FN _ld_c_ixl(void)		MCR_LD(R_Z80C, R_Z80IXL)
Z80FN _ld_c_xix(void)		MCR_LD_RXI(R_Z80C, R_Z80IX)

Z80FN _ld_d_ixh(void)		MCR_LD(R_Z80D, R_Z80IXH)
Z80FN _ld_d_ixl(void)		MCR_LD(R_Z80D, R_Z80IXL)
Z80FN _ld_d_xix(void)		MCR_LD_RXI(R_Z80D, R_Z80IX)
Z80FN _ld_e_ixh(void)		MCR_LD(R_Z80E, R_Z80IXH)
Z80FN _ld_e_ixl(void)		MCR_LD(R_Z80E, R_Z80IXL)
Z80FN _ld_e_xix(void)		MCR_LD_RXI(R_Z80E, R_Z80IX)

Z80FN _ld_ixh_b(void)		MCR_LD(R_Z80IXH, R_Z80B)
Z80FN _ld_ixh_c(void)		MCR_LD(R_Z80IXH, R_Z80C)
Z80FN _ld_ixh_d(void)		MCR_LD(R_Z80IXH, R_Z80D)
Z80FN _ld_ixh_e(void)		MCR_LD(R_Z80IXH, R_Z80E)
Z80FN _ld_ixh_ixl(void)		MCR_LD(R_Z80IXH, R_Z80IXL)
Z80FN _ld_h_xix(void)		MCR_LD_RXI(R_Z80H, R_Z80IX)
Z80FN _ld_ixh_a(void)		MCR_LD(R_Z80IXH, R_Z80A)
Z80FN _ld_ixl_b(void)		MCR_LD(R_Z80IXL, R_Z80B)
Z80FN _ld_ixl_c(void)		MCR_LD(R_Z80IXL, R_Z80C)
Z80FN _ld_ixl_d(void)		MCR_LD(R_Z80IXL, R_Z80D)
Z80FN _ld_ixl_e(void)		MCR_LD(R_Z80IXL, R_Z80E)
Z80FN _ld_ixl_ixh(void)		MCR_LD(R_Z80IXL, R_Z80IXH)
Z80FN _ld_l_xix(void)		MCR_LD_RXI(R_Z80L, R_Z80IX)
Z80FN _ld_ixl_a(void)		MCR_LD(R_Z80IXL, R_Z80A)

Z80FN _ld_xix_b(void)		MCR_LD_XIR(R_Z80B, R_Z80IX)
Z80FN _ld_xix_c(void)		MCR_LD_XIR(R_Z80C, R_Z80IX)
Z80FN _ld_xix_d(void)		MCR_LD_XIR(R_Z80D, R_Z80IX)
Z80FN _ld_xix_e(void)		MCR_LD_XIR(R_Z80E, R_Z80IX)
Z80FN _ld_xix_h(void)		MCR_LD_XIR(R_Z80H, R_Z80IX)
Z80FN _ld_xix_l(void)		MCR_LD_XIR(R_Z80L, R_Z80IX)
Z80FN _ld_xix_a(void)		MCR_LD_XIR(R_Z80A, R_Z80IX)
Z80FN _ld_a_ixh(void)		MCR_LD(R_Z80A, R_Z80IXH)
Z80FN _ld_a_ixl(void)		MCR_LD(R_Z80A, R_Z80IXL)
Z80FN _ld_a_xix(void)		MCR_LD_RXI(R_Z80A, R_Z80IX)

Z80FN _add_a_ixh(void)		MCR_ADD(R_Z80IXH)
Z80FN _add_a_ixl(void)		MCR_ADD(R_Z80IXL)
Z80FN _add_a_xix(void)		MCR_ADD_XI(R_Z80IX)
Z80FN _adc_a_ixh(void)		MCR_ADC(R_Z80IXH)
Z80FN _adc_a_ixl(void)		MCR_ADC(R_Z80IXL)
Z80FN _adc_a_xix(void)		MCR_ADC_XI(R_Z80IX)

Z80FN _sub_ixh(void)		MCR_SUB(R_Z80IXH)
Z80FN _sub_ixl(void)		MCR_SUB(R_Z80IXL)
Z80FN _sub_xix(void)		MCR_SUB_XI(R_Z80IX)
Z80FN _sbc_a_ixh(void)		MCR_SBC(R_Z80IXH)
Z80FN _sbc_a_ixl(void)		MCR_SBC(R_Z80IXL)
Z80FN _sbc_a_xix(void)		MCR_SBC_XI(R_Z80IX)

Z80FN _and_ixh(void)		MCR_AND(R_Z80IXH)
Z80FN _and_ixl(void)		MCR_AND(R_Z80IXL)
Z80FN _and_xix(void)		MCR_AND_XI(R_Z80IX)
Z80FN _xor_ixh(void)		MCR_XOR(R_Z80IXH)
Z80FN _xor_ixl(void)		MCR_XOR(R_Z80IXL)
Z80FN _xor_xix(void)		MCR_XOR_XI(R_Z80IX)

Z80FN _or_ixh(void)			MCR_OR(R_Z80IXH)
Z80FN _or_ixl(void)			MCR_OR(R_Z80IXL)
Z80FN _or_xix(void)			MCR_OR_XI(R_Z80IX)
Z80FN _cp_ixh(void)			MCR_CP(R_Z80IXH)
Z80FN _cp_ixl(void)			MCR_CP(R_Z80IXL)
Z80FN _cp_xix(void)			MCR_CP_XI(R_Z80IX)

Z80FN _pop_ix(void)			MCR_POP(R_Z80IX)
Z80FN _ex_xsp_ix(void)		MCR_EX_XSP(R_Z80IX)
Z80FN _push_ix(void)		MCR_PUSH(R_Z80IX)
Z80FN _jp_ix(void)			MCR_LD_W(R_Z80PC, R_Z80IX)

Z80FN _ld_sp_ix(void)		MCR_LD_W(R_Z80SP, R_Z80IX)


static const Z80OP z80c_ixp[256] = {
	_no_op,			_no_op,			_no_op,			_no_op,			/* 00 */
	_no_op,			_no_op,			_no_op,			_no_op,
	_no_op,			_add_ix_bc,		_no_op,			_no_op,
	_no_op,			_no_op,			_no_op,			_no_op,

	_no_op,			_no_op,			_no_op,			_no_op,			/* 10 */
	_no_op,			_no_op,			_no_op,			_no_op,
	_no_op,			_add_ix_de,		_no_op,			_no_op,
	_no_op,			_no_op,			_no_op,			_no_op,

	_no_op,			_ld_ix_word,	_ld_xword_ix,	_inc_ix,		/* 20 */
	_inc_ixh,		_dec_ixh,		_ld_ixh_byte,	_no_op,
	_no_op,			_add_ix_ix,		_ld_ix_xword,	_dec_ix,
	_inc_ixl,		_dec_ixl,		_ld_ixl_byte,	_no_op,

	_no_op,			_no_op,			_no_op,			_no_op,			/* 30 */
	_inc_xix,		_dec_xix,		_ld_xix_byte,	_no_op,
	_no_op,			_add_ix_sp,		_no_op,			_no_op,
	_no_op,			_no_op,			_no_op,			_no_op,

	_no_op,			_no_op,			_no_op,			_no_op,			/* 40 */
	_ld_b_ixh,		_ld_b_ixl,		_ld_b_xix,		_no_op,
	_no_op,			_no_op,			_no_op,			_no_op,
	_ld_c_ixh,		_ld_c_ixl,		_ld_c_xix,		_no_op,

	_no_op,			_no_op,			_no_op,			_no_op,			/* 50 */
	_ld_d_ixh,		_ld_d_ixl,		_ld_d_xix,		_no_op,
	_no_op,			_no_op,			_no_op,			_no_op,
	_ld_e_ixh,		_ld_e_ixl,		_ld_e_xix,		_no_op,

	_ld_ixh_b,		_ld_ixh_c,		_ld_ixh_d,		_ld_ixh_e,		/* 60 */
	_ld_nop,		_ld_ixh_ixl,	_ld_h_xix,		_ld_ixh_a,
	_ld_ixl_b,		_ld_ixl_c,		_ld_ixl_d,		_ld_ixl_e,
	_ld_ixl_ixh,	_ld_nop,		_ld_l_xix,		_ld_ixl_a,

	_ld_xix_b,		_ld_xix_c,		_ld_xix_d,		_ld_xix_e,		/* 70 */
	_ld_xix_h,		_ld_xix_l,		_no_op,			_ld_xix_a,
	_no_op,			_no_op,			_no_op,			_no_op,
	_ld_a_ixh,		_ld_a_ixl,		_ld_a_xix,		_no_op,

	_no_op,			_no_op,			_no_op,			_no_op,			/* 80 */
	_add_a_ixh,		_add_a_ixl,		_add_a_xix,		_no_op,
	_no_op,			_no_op,			_no_op,			_no_op,
	_adc_a_ixh,		_adc_a_ixl,		_adc_a_xix,		_no_op,

	_no_op,			_no_op,			_no_op,			_no_op,			/* 90 */
	_sub_ixh,		_sub_ixl,		_sub_xix,		_no_op,
	_no_op,			_no_op,			_no_op,			_no_op,
	_sbc_a_ixh,		_sbc_a_ixl,		_sbc_a_xix,		_no_op,

	_no_op,			_no_op,			_no_op,			_no_op,			/* a0 */
	_and_ixh,		_and_ixl,		_and_xix,		_no_op,
	_no_op,			_no_op,			_no_op,			_no_op,
	_xor_ixh,		_xor_ixl,		_xor_xix,		_no_op,

	_no_op,			_no_op,			_no_op,			_no_op,			/* b0 */
	_or_ixh,		_or_ixl,		_or_xix,		_no_op,
	_no_op,			_no_op,			_no_op,			_no_op,
	_cp_ixh,		_cp_ixl,		_cp_xix,		_no_op,

	_no_op,			_no_op,			_no_op,			_no_op,			/* c0 */
	_no_op,			_no_op,			_no_op,			_no_op,
	_no_op,			_no_op,			_no_op,			z80c_ixcb,
	_no_op,			_no_op,			_no_op,			_no_op,

	_no_op,			_no_op,			_no_op,			_no_op,			/* d0 */
	_no_op,			_no_op,			_no_op,			_no_op,
	_no_op,			_no_op,			_no_op,			_no_op,
	_no_op,			_no_op,			_no_op,			_no_op,

	_no_op,			_pop_ix,		_no_op,			_ex_xsp_ix,		/* e0 */
	_no_op,			_push_ix,		_no_op,			_no_op,
	_no_op,			_jp_ix,			_no_op,			_no_op,
	_no_op,			_no_op,			_no_op,			_no_op,

	_no_op,			_no_op,			_no_op,			_no_op,			/* f0 */
	_no_op,			_no_op,			_no_op,			_no_op,
	_no_op,			_ld_sp_ix,		_no_op,			_no_op,
	_no_op,			_no_op,			_no_op,			_no_op
};


void z80c_ix(void) {

	UINT	tmp;

	R_Z80R++;
	GET_PC_BYTE(tmp);
	Z80_COUNT(cycles_xx[tmp]);
	z80c_ixp[tmp]();
}

