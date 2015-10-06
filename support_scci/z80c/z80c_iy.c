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


#define	R_Z80IYL	z80core.s.r.b.iyl
#define	R_Z80IYH	z80core.s.r.b.iyh
#undef	R_Z80IX


Z80FN _ld_nop(void)			{ }


Z80FN _no_op(void)			MCR_DEC_W(R_Z80PC)
Z80FN _add_iy_bc(void)		MCR_ADD_W(R_Z80IY, R_Z80BC)

Z80FN _add_iy_de(void)		MCR_ADD_W(R_Z80IY, R_Z80DE)

Z80FN _ld_iy_word(void)		LDW_w(R_Z80IY)
Z80FN _ld_xword_iy(void)	LDx_W(R_Z80IY)
Z80FN _inc_iy(void)			MCR_INC_W(R_Z80IY)
Z80FN _inc_iyh(void)		MCR_INC(R_Z80IYH)
Z80FN _dec_iyh(void)		MCR_DEC(R_Z80IYH)
Z80FN _ld_iyh_byte(void)	LDB_b(R_Z80IYH)
Z80FN _add_iy_iy(void)		MCR_ADDx2(R_Z80IY)
Z80FN _ld_iy_xword(void)	LDW_x(R_Z80IY)
Z80FN _dec_iy(void)			MCR_DEC_W(R_Z80IY)
Z80FN _inc_iyl(void)		MCR_INC(R_Z80IYL)
Z80FN _dec_iyl(void)		MCR_DEC(R_Z80IYL)
Z80FN _ld_iyl_byte(void)	LDB_b(R_Z80IYL)

Z80FN _inc_xiy(void)		MCR_INC_XI(R_Z80IY)
Z80FN _dec_xiy(void)		MCR_DEC_XI(R_Z80IY)
Z80FN _ld_xiy_byte(void)	MCR_LDXIBYTE(R_Z80IY)
Z80FN _add_iy_sp(void)		MCR_ADD_W(R_Z80IY, R_Z80SP)

Z80FN _ld_b_iyh(void)		MCR_LD(R_Z80B, R_Z80IYH)
Z80FN _ld_b_iyl(void)		MCR_LD(R_Z80B, R_Z80IYL)
Z80FN _ld_b_xiy(void)		MCR_LD_RXI(R_Z80B, R_Z80IY)
Z80FN _ld_c_iyh(void)		MCR_LD(R_Z80C, R_Z80IYH)
Z80FN _ld_c_iyl(void)		MCR_LD(R_Z80C, R_Z80IYL)
Z80FN _ld_c_xiy(void)		MCR_LD_RXI(R_Z80C, R_Z80IY)

Z80FN _ld_d_iyh(void)		MCR_LD(R_Z80D, R_Z80IYH)
Z80FN _ld_d_iyl(void)		MCR_LD(R_Z80D, R_Z80IYL)
Z80FN _ld_d_xiy(void)		MCR_LD_RXI(R_Z80D, R_Z80IY)
Z80FN _ld_e_iyh(void)		MCR_LD(R_Z80E, R_Z80IYH)
Z80FN _ld_e_iyl(void)		MCR_LD(R_Z80E, R_Z80IYL)
Z80FN _ld_e_xiy(void)		MCR_LD_RXI(R_Z80E, R_Z80IY)

Z80FN _ld_iyh_b(void)		MCR_LD(R_Z80IYH, R_Z80B)
Z80FN _ld_iyh_c(void)		MCR_LD(R_Z80IYH, R_Z80C)
Z80FN _ld_iyh_d(void)		MCR_LD(R_Z80IYH, R_Z80D)
Z80FN _ld_iyh_e(void)		MCR_LD(R_Z80IYH, R_Z80E)
Z80FN _ld_iyh_iyl(void)		MCR_LD(R_Z80IYH, R_Z80IYL)
Z80FN _ld_h_xiy(void)		MCR_LD_RXI(R_Z80H, R_Z80IY)
Z80FN _ld_iyh_a(void)		MCR_LD(R_Z80IYH, R_Z80A)
Z80FN _ld_iyl_b(void)		MCR_LD(R_Z80IYL, R_Z80B)
Z80FN _ld_iyl_c(void)		MCR_LD(R_Z80IYL, R_Z80C)
Z80FN _ld_iyl_d(void)		MCR_LD(R_Z80IYL, R_Z80D)
Z80FN _ld_iyl_e(void)		MCR_LD(R_Z80IYL, R_Z80E)
Z80FN _ld_iyl_iyh(void)		MCR_LD(R_Z80IYL, R_Z80IYH)
Z80FN _ld_l_xiy(void)		MCR_LD_RXI(R_Z80L, R_Z80IY)
Z80FN _ld_iyl_a(void)		MCR_LD(R_Z80IYL, R_Z80A)

Z80FN _ld_xiy_b(void)		MCR_LD_XIR(R_Z80B, R_Z80IY)
Z80FN _ld_xiy_c(void)		MCR_LD_XIR(R_Z80C, R_Z80IY)
Z80FN _ld_xiy_d(void)		MCR_LD_XIR(R_Z80D, R_Z80IY)
Z80FN _ld_xiy_e(void)		MCR_LD_XIR(R_Z80E, R_Z80IY)
Z80FN _ld_xiy_h(void)		MCR_LD_XIR(R_Z80H, R_Z80IY)
Z80FN _ld_xiy_l(void)		MCR_LD_XIR(R_Z80L, R_Z80IY)
Z80FN _ld_xiy_a(void)		MCR_LD_XIR(R_Z80A, R_Z80IY)
Z80FN _ld_a_iyh(void)		MCR_LD(R_Z80A, R_Z80IYH)
Z80FN _ld_a_iyl(void)		MCR_LD(R_Z80A, R_Z80IYL)
Z80FN _ld_a_xiy(void)		MCR_LD_RXI(R_Z80A, R_Z80IY)

Z80FN _add_a_iyh(void)		MCR_ADD(R_Z80IYH)
Z80FN _add_a_iyl(void)		MCR_ADD(R_Z80IYL)
Z80FN _add_a_xiy(void)		MCR_ADD_XI(R_Z80IY)
Z80FN _adc_a_iyh(void)		MCR_ADC(R_Z80IYH)
Z80FN _adc_a_iyl(void)		MCR_ADC(R_Z80IYL)
Z80FN _adc_a_xiy(void)		MCR_ADC_XI(R_Z80IY)

Z80FN _sub_iyh(void)		MCR_SUB(R_Z80IYH)
Z80FN _sub_iyl(void)		MCR_SUB(R_Z80IYL)
Z80FN _sub_xiy(void)		MCR_SUB_XI(R_Z80IY)
Z80FN _sbc_a_iyh(void)		MCR_SBC(R_Z80IYH)
Z80FN _sbc_a_iyl(void)		MCR_SBC(R_Z80IYL)
Z80FN _sbc_a_xiy(void)		MCR_SBC_XI(R_Z80IY)

Z80FN _and_iyh(void)		MCR_AND(R_Z80IYH)
Z80FN _and_iyl(void)		MCR_AND(R_Z80IYL)
Z80FN _and_xiy(void)		MCR_AND_XI(R_Z80IY)
Z80FN _xor_iyh(void)		MCR_XOR(R_Z80IYH)
Z80FN _xor_iyl(void)		MCR_XOR(R_Z80IYL)
Z80FN _xor_xiy(void)		MCR_XOR_XI(R_Z80IY)

Z80FN _or_iyh(void)			MCR_OR(R_Z80IYH)
Z80FN _or_iyl(void)			MCR_OR(R_Z80IYL)
Z80FN _or_xiy(void)			MCR_OR_XI(R_Z80IY)
Z80FN _cp_iyh(void)			MCR_CP(R_Z80IYH)
Z80FN _cp_iyl(void)			MCR_CP(R_Z80IYL)
Z80FN _cp_xiy(void)			MCR_CP_XI(R_Z80IY)

Z80FN _pop_iy(void)			MCR_POP(R_Z80IY)
Z80FN _ex_xsp_iy(void)		MCR_EX_XSP(R_Z80IY)
Z80FN _push_iy(void)		MCR_PUSH(R_Z80IY)
Z80FN _jp_iy(void)			MCR_LD_W(R_Z80PC, R_Z80IY)

Z80FN _ld_sp_iy(void)		MCR_LD_W(R_Z80SP, R_Z80IY)


static const Z80OP z80c_iyp[256] = {
	_no_op,			_no_op,			_no_op,			_no_op,			/* 00 */
	_no_op,			_no_op,			_no_op,			_no_op,
	_no_op,			_add_iy_bc,		_no_op,			_no_op,
	_no_op,			_no_op,			_no_op,			_no_op,

	_no_op,			_no_op,			_no_op,			_no_op,			/* 10 */
	_no_op,			_no_op,			_no_op,			_no_op,
	_no_op,			_add_iy_de,		_no_op,			_no_op,
	_no_op,			_no_op,			_no_op,			_no_op,

	_no_op,			_ld_iy_word,	_ld_xword_iy,	_inc_iy,		/* 20 */
	_inc_iyh,		_dec_iyh,		_ld_iyh_byte,	_no_op,
	_no_op,			_add_iy_iy,		_ld_iy_xword,	_dec_iy,
	_inc_iyl,		_dec_iyl,		_ld_iyl_byte,	_no_op,

	_no_op,			_no_op,			_no_op,			_no_op,			/* 30 */
	_inc_xiy,		_dec_xiy,		_ld_xiy_byte,	_no_op,
	_no_op,			_add_iy_sp,		_no_op,			_no_op,
	_no_op,			_no_op,			_no_op,			_no_op,

	_no_op,			_no_op,			_no_op,			_no_op,			/* 40 */
	_ld_b_iyh,		_ld_b_iyl,		_ld_b_xiy,		_no_op,
	_no_op,			_no_op,			_no_op,			_no_op,
	_ld_c_iyh,		_ld_c_iyl,		_ld_c_xiy,		_no_op,

	_no_op,			_no_op,			_no_op,			_no_op,			/* 50 */
	_ld_d_iyh,		_ld_d_iyl,		_ld_d_xiy,		_no_op,
	_no_op,			_no_op,			_no_op,			_no_op,
	_ld_e_iyh,		_ld_e_iyl,		_ld_e_xiy,		_no_op,

	_ld_iyh_b,		_ld_iyh_c,		_ld_iyh_d,		_ld_iyh_e,		/* 60 */
	_ld_nop,		_ld_iyh_iyl,	_ld_h_xiy,		_ld_iyh_a,
	_ld_iyl_b,		_ld_iyl_c,		_ld_iyl_d,		_ld_iyl_e,
	_ld_iyl_iyh,	_ld_nop,		_ld_l_xiy,		_ld_iyl_a,

	_ld_xiy_b,		_ld_xiy_c,		_ld_xiy_d,		_ld_xiy_e,		/* 70 */
	_ld_xiy_h,		_ld_xiy_l,		_no_op,			_ld_xiy_a,
	_no_op,			_no_op,			_no_op,			_no_op,
	_ld_a_iyh,		_ld_a_iyl,		_ld_a_xiy,		_no_op,

	_no_op,			_no_op,			_no_op,			_no_op,			/* 80 */
	_add_a_iyh,		_add_a_iyl,		_add_a_xiy,		_no_op,
	_no_op,			_no_op,			_no_op,			_no_op,
	_adc_a_iyh,		_adc_a_iyl,		_adc_a_xiy,		_no_op,

	_no_op,			_no_op,			_no_op,			_no_op,			/* 90 */
	_sub_iyh,		_sub_iyl,		_sub_xiy,		_no_op,
	_no_op,			_no_op,			_no_op,			_no_op,
	_sbc_a_iyh,		_sbc_a_iyl,		_sbc_a_xiy,		_no_op,

	_no_op,			_no_op,			_no_op,			_no_op,			/* a0 */
	_and_iyh,		_and_iyl,		_and_xiy,		_no_op,
	_no_op,			_no_op,			_no_op,			_no_op,
	_xor_iyh,		_xor_iyl,		_xor_xiy,		_no_op,

	_no_op,			_no_op,			_no_op,			_no_op,			/* b0 */
	_or_iyh,		_or_iyl,		_or_xiy,		_no_op,
	_no_op,			_no_op,			_no_op,			_no_op,
	_cp_iyh,		_cp_iyl,		_cp_xiy,		_no_op,

	_no_op,			_no_op,			_no_op,			_no_op,			/* c0 */
	_no_op,			_no_op,			_no_op,			_no_op,
	_no_op,			_no_op,			_no_op,			z80c_iycb,
	_no_op,			_no_op,			_no_op,			_no_op,

	_no_op,			_no_op,			_no_op,			_no_op,			/* d0 */
	_no_op,			_no_op,			_no_op,			_no_op,
	_no_op,			_no_op,			_no_op,			_no_op,
	_no_op,			_no_op,			_no_op,			_no_op,

	_no_op,			_pop_iy,		_no_op,			_ex_xsp_iy,		/* e0 */
	_no_op,			_push_iy,		_no_op,			_no_op,
	_no_op,			_jp_iy,			_no_op,			_no_op,
	_no_op,			_no_op,			_no_op,			_no_op,

	_no_op,			_no_op,			_no_op,			_no_op,			/* f0 */
	_no_op,			_no_op,			_no_op,			_no_op,
	_no_op,			_ld_sp_iy,		_no_op,			_no_op,
	_no_op,			_no_op,			_no_op,			_no_op
};


void z80c_iy(void) {

	UINT	tmp;

	R_Z80R++;
	GET_PC_BYTE(tmp);
	Z80_COUNT(cycles_xx[tmp]);
	z80c_iyp[tmp]();
}

