/* -----------------------------------------------------------------------
 *
 * Z80C : Z80 Engine - GENERIC
 *
 *                              Copyright by Studio Milmake 1999-2000,2004
 *
 *------------------------------------------------------------------------ */

#include	"compiler.h"
#include	"z80core.h"
#include	"iocore.h"
#include	"z80c.h"
#include	"z80c.mcr"


Z80FN _ld_nop(void)			{ }
Z80FN _ld_bc_word(void)		LDW_w(R_Z80BC)
Z80FN _ld_xbc_a(void)		LDx_B(R_Z80BC, R_Z80A)
Z80FN _inc_bc(void)			MCR_INC_W(R_Z80BC)
Z80FN _inc_b(void)			MCR_INC(R_Z80B)
Z80FN _dec_b(void)			MCR_DEC(R_Z80B)
Z80FN _ld_b_byte(void)		LDB_b(R_Z80B)
Z80FN _rlca(void)			MCR_RLCA
Z80FN _ex_af_af(void)		MCR_EX(R_Z80AF, R_Z80AF2)
Z80FN _add_hl_bc(void)		MCR_ADD_W(R_Z80HL, R_Z80BC)
Z80FN _ld_a_xbc(void)		LDB_x(R_Z80A, R_Z80BC)
Z80FN _dec_bc(void)			MCR_DEC_W(R_Z80BC)
Z80FN _inc_c(void)			MCR_INC(R_Z80C)
Z80FN _dec_c(void)			MCR_DEC(R_Z80C)
Z80FN _ld_c_byte(void)		LDB_b(R_Z80C)
Z80FN _rrca(void)			MCR_RRCA

Z80FN _djnz(void)			MCR_DJNZ
Z80FN _ld_de_word(void)		LDW_w(R_Z80DE)
Z80FN _ld_xde_a(void)		LDx_B(R_Z80DE, R_Z80A)
Z80FN _inc_de(void)			MCR_INC_W(R_Z80DE)
Z80FN _inc_d(void)			MCR_INC(R_Z80D)
Z80FN _dec_d(void)			MCR_DEC(R_Z80D)
Z80FN _ld_d_byte(void)		LDB_b(R_Z80D)
Z80FN _rla(void)			MCR_RLA
Z80FN _jr(void)				MCR_JR
Z80FN _add_hl_de(void)		MCR_ADD_W(R_Z80HL, R_Z80DE)
Z80FN _ld_a_xde(void)		LDB_x(R_Z80A, R_Z80DE)
Z80FN _dec_de(void)			MCR_DEC_W(R_Z80DE)
Z80FN _inc_e(void)			MCR_INC(R_Z80E)
Z80FN _dec_e(void)			MCR_DEC(R_Z80E)
Z80FN _ld_e_byte(void)		LDB_b(R_Z80E)
Z80FN _rra(void)			MCR_RRA

Z80FN _jr_nz(void)			MCR_JRNFLG(Z_FLAG)
Z80FN _ld_hl_word(void)		LDW_w(R_Z80HL)
Z80FN _ld_xword_hl(void)	LDx_W(R_Z80HL)
Z80FN _inc_hl(void)			MCR_INC_W(R_Z80HL)
Z80FN _inc_h(void)			MCR_INC(R_Z80H)
Z80FN _dec_h(void)			MCR_DEC(R_Z80H)
Z80FN _ld_h_byte(void)		LDB_b(R_Z80H)
Z80FN _daa(void)			MCR_DDA
Z80FN _jr_z(void)			MCR_JRFLG(Z_FLAG)
Z80FN _add_hl_hl(void)		MCR_ADDx2(R_Z80HL)
Z80FN _ld_hl_xword(void)	LDW_x(R_Z80HL)
Z80FN _dec_hl(void)			MCR_DEC_W(R_Z80HL)
Z80FN _inc_l(void)			MCR_INC(R_Z80L)
Z80FN _dec_l(void)			MCR_DEC(R_Z80L)
Z80FN _ld_l_byte(void)		LDB_b(R_Z80L)
Z80FN _cpl(void)			MCR_CPL

Z80FN _jr_nc(void)			MCR_JRNFLG(C_FLAG)
Z80FN _ld_sp_word(void)		LDW_w(R_Z80SP)
Z80FN _ld_xbyte_a(void)		LDB_x_a
Z80FN _inc_sp(void)			MCR_INC_W(R_Z80SP)
Z80FN _inc_xhl(void)		MCR_INC_MEM(R_Z80HL)
Z80FN _dec_xhl(void)		MCR_DEC_MEM(R_Z80HL)
Z80FN _ld_xhl_byte(void)	LDB_xhl_b
Z80FN _scf(void)			MCR_SCF
Z80FN _jr_c(void)			MCR_JRFLG(C_FLAG)
Z80FN _add_hl_sp(void)		MCR_ADD_W(R_Z80HL, R_Z80SP)
Z80FN _ld_a_xbyte(void)		LDB_a_x
Z80FN _dec_sp(void)			MCR_DEC_W(R_Z80SP)
Z80FN _inc_a(void)			MCR_INC(R_Z80A)
Z80FN _dec_a(void)			MCR_DEC(R_Z80A)
Z80FN _ld_a_byte(void)		LDB_b(R_Z80A)
Z80FN _ccf(void)			MCR_CCF

Z80FN _ld_b_c(void)			MCR_LD(R_Z80B, R_Z80C)
Z80FN _ld_b_d(void)			MCR_LD(R_Z80B, R_Z80D)
Z80FN _ld_b_e(void)			MCR_LD(R_Z80B, R_Z80E)
Z80FN _ld_b_h(void)			MCR_LD(R_Z80B, R_Z80H)
Z80FN _ld_b_l(void)			MCR_LD(R_Z80B, R_Z80L)
Z80FN _ld_b_xhl(void)		LDB_x(R_Z80B, R_Z80HL)
Z80FN _ld_b_a(void)			MCR_LD(R_Z80B, R_Z80A)
Z80FN _ld_c_b(void)			MCR_LD(R_Z80C, R_Z80B)
Z80FN _ld_c_d(void)			MCR_LD(R_Z80C, R_Z80D)
Z80FN _ld_c_e(void)			MCR_LD(R_Z80C, R_Z80E)
Z80FN _ld_c_h(void)			MCR_LD(R_Z80C, R_Z80H)
Z80FN _ld_c_l(void)			MCR_LD(R_Z80C, R_Z80L)
Z80FN _ld_c_xhl(void)		LDB_x(R_Z80C, R_Z80HL)
Z80FN _ld_c_a(void)			MCR_LD(R_Z80C, R_Z80A)

Z80FN _ld_d_b(void)			MCR_LD(R_Z80D, R_Z80B)
Z80FN _ld_d_c(void)			MCR_LD(R_Z80D, R_Z80C)
Z80FN _ld_d_e(void)			MCR_LD(R_Z80D, R_Z80E)
Z80FN _ld_d_h(void)			MCR_LD(R_Z80D, R_Z80H)
Z80FN _ld_d_l(void)			MCR_LD(R_Z80D, R_Z80L)
Z80FN _ld_d_xhl(void)		LDB_x(R_Z80D, R_Z80HL)
Z80FN _ld_d_a(void)			MCR_LD(R_Z80D, R_Z80A)
Z80FN _ld_e_b(void)			MCR_LD(R_Z80E, R_Z80B)
Z80FN _ld_e_c(void)			MCR_LD(R_Z80E, R_Z80C)
Z80FN _ld_e_d(void)			MCR_LD(R_Z80E, R_Z80D)
Z80FN _ld_e_h(void)			MCR_LD(R_Z80E, R_Z80H)
Z80FN _ld_e_l(void)			MCR_LD(R_Z80E, R_Z80L)
Z80FN _ld_e_xhl(void)		LDB_x(R_Z80E, R_Z80HL)
Z80FN _ld_e_a(void)			MCR_LD(R_Z80E, R_Z80A)

Z80FN _ld_h_b(void)			MCR_LD(R_Z80H, R_Z80B)
Z80FN _ld_h_c(void)			MCR_LD(R_Z80H, R_Z80C)
Z80FN _ld_h_d(void)			MCR_LD(R_Z80H, R_Z80D)
Z80FN _ld_h_e(void)			MCR_LD(R_Z80H, R_Z80E)
Z80FN _ld_h_l(void)			MCR_LD(R_Z80H, R_Z80L)
Z80FN _ld_h_xhl(void)		LDB_x(R_Z80H, R_Z80HL)
Z80FN _ld_h_a(void)			MCR_LD(R_Z80H, R_Z80A)
Z80FN _ld_l_b(void)			MCR_LD(R_Z80L, R_Z80B)
Z80FN _ld_l_c(void)			MCR_LD(R_Z80L, R_Z80C)
Z80FN _ld_l_d(void)			MCR_LD(R_Z80L, R_Z80D)
Z80FN _ld_l_e(void)			MCR_LD(R_Z80L, R_Z80E)
Z80FN _ld_l_h(void)			MCR_LD(R_Z80L, R_Z80H)
Z80FN _ld_l_xhl(void)		LDB_x(R_Z80L, R_Z80HL)
Z80FN _ld_l_a(void)			MCR_LD(R_Z80L, R_Z80A)

Z80FN _ld_xhl_b(void)		LDx_B(R_Z80HL, R_Z80B)
Z80FN _ld_xhl_c(void)		LDx_B(R_Z80HL, R_Z80C)
Z80FN _ld_xhl_d(void)		LDx_B(R_Z80HL, R_Z80D)
Z80FN _ld_xhl_e(void)		LDx_B(R_Z80HL, R_Z80E)
Z80FN _ld_xhl_h(void)		LDx_B(R_Z80HL, R_Z80H)
Z80FN _ld_xhl_l(void)		LDx_B(R_Z80HL, R_Z80L)
Z80FN _halt(void)			MCR_HALT
Z80FN _ld_xhl_a(void)		LDx_B(R_Z80HL, R_Z80A)
Z80FN _ld_a_b(void)			MCR_LD(R_Z80A, R_Z80B)
Z80FN _ld_a_c(void)			MCR_LD(R_Z80A, R_Z80C)
Z80FN _ld_a_d(void)			MCR_LD(R_Z80A, R_Z80D)
Z80FN _ld_a_e(void)			MCR_LD(R_Z80A, R_Z80E)
Z80FN _ld_a_h(void)			MCR_LD(R_Z80A, R_Z80H)
Z80FN _ld_a_l(void)			MCR_LD(R_Z80A, R_Z80L)
Z80FN _ld_a_xhl(void)		LDB_x(R_Z80A, R_Z80HL)

Z80FN _add_a_b(void)		MCR_ADD(R_Z80B)
Z80FN _add_a_c(void)		MCR_ADD(R_Z80C)
Z80FN _add_a_d(void)		MCR_ADD(R_Z80D)
Z80FN _add_a_e(void)		MCR_ADD(R_Z80E)
Z80FN _add_a_h(void)		MCR_ADD(R_Z80H)
Z80FN _add_a_l(void)		MCR_ADD(R_Z80L)
Z80FN _add_a_xhl(void)		MCR_ADD_XHL
Z80FN _add_a_a(void)		MCR_ADD(R_Z80A)				/* !! */
Z80FN _adc_a_b(void)		MCR_ADC(R_Z80B)
Z80FN _adc_a_c(void)		MCR_ADC(R_Z80C)
Z80FN _adc_a_d(void)		MCR_ADC(R_Z80D)
Z80FN _adc_a_e(void)		MCR_ADC(R_Z80E)
Z80FN _adc_a_h(void)		MCR_ADC(R_Z80H)
Z80FN _adc_a_l(void)		MCR_ADC(R_Z80L)
Z80FN _adc_a_xhl(void)		MCR_ADC_XHL
Z80FN _adc_a_a(void)		MCR_ADC(R_Z80A)				/* !! */

Z80FN _sub_b(void)			MCR_SUB(R_Z80B)
Z80FN _sub_c(void)			MCR_SUB(R_Z80C)
Z80FN _sub_d(void)			MCR_SUB(R_Z80D)
Z80FN _sub_e(void)			MCR_SUB(R_Z80E)
Z80FN _sub_h(void)			MCR_SUB(R_Z80H)
Z80FN _sub_l(void)			MCR_SUB(R_Z80L)
Z80FN _sub_xhl(void)		MCR_SUB_XHL
Z80FN _sub_a(void)			MCR_SUB(R_Z80A)				/* !! */
Z80FN _sbc_a_b(void)		MCR_SBC(R_Z80B)
Z80FN _sbc_a_c(void)		MCR_SBC(R_Z80C)
Z80FN _sbc_a_d(void)		MCR_SBC(R_Z80D)
Z80FN _sbc_a_e(void)		MCR_SBC(R_Z80E)
Z80FN _sbc_a_h(void)		MCR_SBC(R_Z80H)
Z80FN _sbc_a_l(void)		MCR_SBC(R_Z80L)
Z80FN _sbc_a_xhl(void)		MCR_SBC_XHL
Z80FN _sbc_a_a(void)		MCR_SBC(R_Z80A)				/* !! */

Z80FN _and_b(void)			MCR_AND(R_Z80B)
Z80FN _and_c(void)			MCR_AND(R_Z80C)
Z80FN _and_d(void)			MCR_AND(R_Z80D)
Z80FN _and_e(void)			MCR_AND(R_Z80E)
Z80FN _and_h(void)			MCR_AND(R_Z80H)
Z80FN _and_l(void)			MCR_AND(R_Z80L)
Z80FN _and_xhl(void)		MCR_AND_XHL
Z80FN _and_a(void)			MCR_AND(R_Z80A)				/* !! */
Z80FN _xor_b(void)			MCR_XOR(R_Z80B)
Z80FN _xor_c(void)			MCR_XOR(R_Z80C)
Z80FN _xor_d(void)			MCR_XOR(R_Z80D)
Z80FN _xor_e(void)			MCR_XOR(R_Z80E)
Z80FN _xor_h(void)			MCR_XOR(R_Z80H)
Z80FN _xor_l(void)			MCR_XOR(R_Z80L)
Z80FN _xor_xhl(void)		MCR_XOR_XHL
Z80FN _xor_a(void)			MCR_XOR(R_Z80A)				/* !! */

Z80FN _or_b(void)			MCR_OR(R_Z80B)
Z80FN _or_c(void)			MCR_OR(R_Z80C)
Z80FN _or_d(void)			MCR_OR(R_Z80D)
Z80FN _or_e(void)			MCR_OR(R_Z80E)
Z80FN _or_h(void)			MCR_OR(R_Z80H)
Z80FN _or_l(void)			MCR_OR(R_Z80L)
Z80FN _or_xhl(void)			MCR_OR_XHL
Z80FN _or_a(void)			MCR_OR(R_Z80A)				/* !! */
Z80FN _cp_b(void)			MCR_CP(R_Z80B)
Z80FN _cp_c(void)			MCR_CP(R_Z80C)
Z80FN _cp_d(void)			MCR_CP(R_Z80D)
Z80FN _cp_e(void)			MCR_CP(R_Z80E)
Z80FN _cp_h(void)			MCR_CP(R_Z80H)
Z80FN _cp_l(void)			MCR_CP(R_Z80L)
Z80FN _cp_xhl(void)			MCR_CP_XHL
Z80FN _cp_a(void)			MCR_CP(R_Z80A)				/* !! */

Z80FN _ret_nz(void)			MCR_RETNFLG(Z_FLAG)
Z80FN _pop_bc(void)			MCR_POP(R_Z80BC)
Z80FN _jp_nz(void)			MCR_JPNFLG(Z_FLAG)
Z80FN _jp(void)				MCR_JP
Z80FN _call_nz(void)		MCR_CALLNFLG(Z_FLAG)
Z80FN _push_bc(void)		MCR_PUSH(R_Z80BC)
Z80FN _add_a_byte(void)		MCR_ADD_BYTE
Z80FN _rst_00(void)			MCR_RST(0x00)
Z80FN _ret_z(void)			MCR_RETFLG(Z_FLAG)
Z80FN _ret(void)			MCR_RET
Z80FN _jp_z(void)			MCR_JPFLG(Z_FLAG)
Z80FN _call_z(void)			MCR_CALLFLG(Z_FLAG)
Z80FN _call(void)			MCR_CALL
Z80FN _adc_a_byte(void)		MCR_ADC_BYTE
Z80FN _rst_08(void)			MCR_RST(0x08)

Z80FN _ret_nc(void)			MCR_RETNFLG(C_FLAG)
Z80FN _pop_de(void)			MCR_POP(R_Z80DE)
Z80FN _jp_nc(void)			MCR_JPNFLG(C_FLAG)
Z80FN _out_byte_a(void)		MCR_OUT_BYTE
Z80FN _call_nc(void)		MCR_CALLNFLG(C_FLAG)
Z80FN _push_de(void)		MCR_PUSH(R_Z80DE)
Z80FN _sub_byte(void)		MCR_SUB_BYTE
Z80FN _rst_10(void)			MCR_RST(0x10)
Z80FN _ret_c(void)			MCR_RETFLG(C_FLAG)
Z80FN _exx(void)			MCR_EXX
Z80FN _jp_c(void)			MCR_JPFLG(C_FLAG)
Z80FN _in_a_byte(void)		MCR_IN_BYTE
Z80FN _call_c(void)			MCR_CALLFLG(C_FLAG)
Z80FN _sbc_a_byte(void)		MCR_SBC_BYTE
Z80FN _rst_18(void)			MCR_RST(0x18)

Z80FN _ret_po(void)			MCR_RETNFLG(V_FLAG)
Z80FN _pop_hl(void)			MCR_POP(R_Z80HL)
Z80FN _jp_po(void)			MCR_JPNFLG(V_FLAG)
Z80FN _ex_xsp_hl(void)		MCR_EX_XSP(R_Z80HL)
Z80FN _call_po(void)		MCR_CALLNFLG(V_FLAG)
Z80FN _push_hl(void)		MCR_PUSH(R_Z80HL)
Z80FN _and_byte(void)		MCR_AND_BYTE
Z80FN _rst_20(void)			MCR_RST(0x20)
Z80FN _ret_pe(void)			MCR_RETFLG(V_FLAG)
Z80FN _jp_hl(void)			MCR_LD_W(R_Z80PC, R_Z80HL)
Z80FN _jp_pe(void)			MCR_JPFLG(V_FLAG)
Z80FN _ex_de_hl(void)		MCR_EX(R_Z80DE, R_Z80HL)
Z80FN _call_pe(void)		MCR_CALLFLG(V_FLAG)
Z80FN _xor_byte(void)		MCR_XOR_BYTE
Z80FN _rst_28(void)			MCR_RST(0x28)

Z80FN _ret_p(void)			MCR_RETNFLG(S_FLAG)
Z80FN _pop_af(void)			MCR_POP(R_Z80AF)
Z80FN _jp_p(void)			MCR_JPNFLG(S_FLAG)
Z80FN _di(void)				MCR_DI
Z80FN _call_p(void)			MCR_CALLNFLG(S_FLAG)
Z80FN _push_af(void)		MCR_PUSH(R_Z80AF)
Z80FN _or_byte(void)		MCR_OR_BYTE
Z80FN _rst_30(void)			MCR_RST(0x30)
Z80FN _ret_m(void)			MCR_RETFLG(S_FLAG)
Z80FN _ld_sp_hl(void)		MCR_LD_W(R_Z80SP, R_Z80HL)
Z80FN _jp_m(void)			MCR_JPFLG(S_FLAG)
Z80FN _ei(void)				MCR_EI
Z80FN _call_m(void)			MCR_CALLFLG(S_FLAG)
Z80FN _cp_byte(void)		MCR_CP_BYTE
Z80FN _rst_38(void)			MCR_RST(0x38)


const Z80OP z80c_mainop[256] = {
	_ld_nop,		_ld_bc_word,	_ld_xbc_a,		_inc_bc,		/* 00 */
	_inc_b,			_dec_b,			_ld_b_byte,		_rlca,
	_ex_af_af,		_add_hl_bc,		_ld_a_xbc,		_dec_bc,
	_inc_c,			_dec_c,			_ld_c_byte,		_rrca,

	_djnz,			_ld_de_word,	_ld_xde_a,		_inc_de,		/* 10 */
	_inc_d,			_dec_d,			_ld_d_byte,		_rla,
	_jr,			_add_hl_de,		_ld_a_xde,		_dec_de,
	_inc_e,			_dec_e,			_ld_e_byte,		_rra,

	_jr_nz,			_ld_hl_word,	_ld_xword_hl,	_inc_hl,		/* 20 */
	_inc_h,			_dec_h,			_ld_h_byte,		_daa,
	_jr_z,			_add_hl_hl,		_ld_hl_xword,	_dec_hl,
	_inc_l,			_dec_l,			_ld_l_byte,		_cpl,

	_jr_nc,			_ld_sp_word,	_ld_xbyte_a,	_inc_sp,		/* 30 */
	_inc_xhl,		_dec_xhl,		_ld_xhl_byte,	_scf,
	_jr_c,			_add_hl_sp,		_ld_a_xbyte,	_dec_sp,
	_inc_a,			_dec_a,			_ld_a_byte,		_ccf,

	_ld_nop,		_ld_b_c,		_ld_b_d,		_ld_b_e,		/* 40 */
	_ld_b_h,		_ld_b_l,		_ld_b_xhl,		_ld_b_a,
	_ld_c_b,		_ld_nop,		_ld_c_d,		_ld_c_e,
	_ld_c_h,		_ld_c_l,		_ld_c_xhl,		_ld_c_a,

	_ld_d_b,		_ld_d_c,		_ld_nop,		_ld_d_e,		/* 50 */
	_ld_d_h,		_ld_d_l,		_ld_d_xhl,		_ld_d_a,
	_ld_e_b,		_ld_e_c,		_ld_e_d,		_ld_nop,
	_ld_e_h,		_ld_e_l,		_ld_e_xhl,		_ld_e_a,

	_ld_h_b,		_ld_h_c,		_ld_h_d,		_ld_h_e,		/* 60 */
	_ld_nop,		_ld_h_l,		_ld_h_xhl,		_ld_h_a,
	_ld_l_b,		_ld_l_c,		_ld_l_d,		_ld_l_e,
	_ld_l_h,		_ld_nop,		_ld_l_xhl,		_ld_l_a,

	_ld_xhl_b,		_ld_xhl_c,		_ld_xhl_d,		_ld_xhl_e,		/* 70 */
	_ld_xhl_h,		_ld_xhl_l,		_halt,			_ld_xhl_a,
	_ld_a_b,		_ld_a_c,		_ld_a_d,		_ld_a_e,
	_ld_a_h,		_ld_a_l,		_ld_a_xhl,		_ld_nop,

	_add_a_b,		_add_a_c,		_add_a_d,		_add_a_e,		/* 80 */
	_add_a_h,		_add_a_l,		_add_a_xhl,		_add_a_a,
	_adc_a_b,		_adc_a_c,		_adc_a_d,		_adc_a_e,
	_adc_a_h,		_adc_a_l,		_adc_a_xhl,		_adc_a_a,

	_sub_b,			_sub_c,			_sub_d,			_sub_e,			/* 90 */
	_sub_h,			_sub_l,			_sub_xhl,		_sub_a,
	_sbc_a_b,		_sbc_a_c,		_sbc_a_d,		_sbc_a_e,
	_sbc_a_h,		_sbc_a_l,		_sbc_a_xhl,		_sbc_a_a,

	_and_b,			_and_c,			_and_d,			_and_e,			/* a0 */
	_and_h,			_and_l,			_and_xhl,		_and_a,
	_xor_b,			_xor_c,			_xor_d,			_xor_e,
	_xor_h,			_xor_l,			_xor_xhl,		_xor_a,

	_or_b,			_or_c,			_or_d,			_or_e,			/* b0 */
	_or_h,			_or_l,			_or_xhl,		_or_a,
	_cp_b,			_cp_c,			_cp_d,			_cp_e,
	_cp_h,			_cp_l,			_cp_xhl,		_cp_a,

	_ret_nz,		_pop_bc,		_jp_nz,			_jp,			/* c0 */
	_call_nz,		_push_bc,		_add_a_byte,	_rst_00,
	_ret_z,			_ret,			_jp_z,			z80c_cb,
	_call_z,		_call,			_adc_a_byte,	_rst_08,

	_ret_nc,		_pop_de,		_jp_nc,			_out_byte_a,	/* d0 */
	_call_nc,		_push_de,		_sub_byte,		_rst_10,
	_ret_c,			_exx,			_jp_c,			_in_a_byte,
	_call_c,		z80c_ix,		_sbc_a_byte,	_rst_18,

	_ret_po,		_pop_hl,		_jp_po,			_ex_xsp_hl,		/* e0 */
	_call_po,		_push_hl,		_and_byte,		_rst_20,
	_ret_pe,		_jp_hl,			_jp_pe,			_ex_de_hl,
	_call_pe,		z80c_sub,		_xor_byte,		_rst_28,

	_ret_p,			_pop_af,		_jp_p,			_di,			/* f0 */
	_call_p,		_push_af,		_or_byte,		_rst_30,
	_ret_m,			_ld_sp_hl,		_jp_m,			_ei,
	_call_m,		z80c_iy,		_cp_byte,		_rst_38
};

