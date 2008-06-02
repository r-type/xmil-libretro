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
#include	"ievent.h"
#include	"z80c.h"
#include	"z80c.mcr"
#include	"z80c_s.mcr"


Z80FN _ld_nop(void)			{ }

Z80FN _in_b_c(void)			MCR_Z80INF(R_Z80B)
Z80FN _out_c_b(void)		MCR_Z80OUT(R_Z80B)
Z80FN _sbc_hl_bc(void)		MCR_SBC_W(R_Z80BC)
Z80FN _ld_xword_bc(void)	LDx_W(R_Z80BC)
Z80FN _neg(void)			MCR_NEG
Z80FN _retn(void)			MCR_RETN
Z80FN _im_0(void)			MCR_IM(0)
Z80FN _ld_i_a(void)			MCR_LD(R_Z80I, R_Z80A)
Z80FN _in_c_c(void)			MCR_Z80INF(R_Z80C)
Z80FN _out_c_c(void)		MCR_Z80OUT(R_Z80C)
Z80FN _adc_hl_bc(void)		MCR_ADC_W(R_Z80BC)
Z80FN _ld_bc_xword(void)	LDW_x(R_Z80BC)
Z80FN _reti(void)			MCR_RETI
Z80FN _ld_r_a(void)			MCR_LD_RA

Z80FN _in_d_c(void)			MCR_Z80INF(R_Z80D)
Z80FN _out_c_d(void)		MCR_Z80OUT(R_Z80D)
Z80FN _sbc_hl_de(void)		MCR_SBC_W(R_Z80DE)
Z80FN _ld_xword_de(void)	LDx_W(R_Z80DE)
Z80FN _im_1(void)			MCR_IM(1)
Z80FN _ld_a_i(void)			MCR_LD_AI
Z80FN _in_e_c(void)			MCR_Z80INF(R_Z80E)
Z80FN _out_c_e(void)		MCR_Z80OUT(R_Z80E)
Z80FN _adc_hl_de(void)		MCR_ADC_W(R_Z80DE)
Z80FN _ld_de_xword(void)	LDW_x(R_Z80DE)
Z80FN _im_2(void)			MCR_IM(2)
Z80FN _ld_a_r(void)			MCR_LD_AR

Z80FN _in_h_c(void)			MCR_Z80INF(R_Z80H)
Z80FN _out_c_h(void)		MCR_Z80OUT(R_Z80H)
Z80FN _sbc_hl_hl(void)		MCR_SBC_W(R_Z80HL)				/* !! */
Z80FN _ld_xword_hl(void)	LDx_W(R_Z80HL)
Z80FN _rrd(void)			MCR_RRD
Z80FN _in_l_c(void)			MCR_Z80INF(R_Z80L)
Z80FN _out_c_l(void)		MCR_Z80OUT(R_Z80L)
Z80FN _adc_hl_hl(void)		MCR_ADC_W(R_Z80HL)
Z80FN _ld_hl_xword(void)	LDW_x(R_Z80HL)
Z80FN _rld(void)			MCR_RLD

Z80FN _in_0_c(void)			MCR_Z80IN0
Z80FN _out_c_0(void)		MCR_Z80OUT(0)
Z80FN _sbc_hl_sp(void)		MCR_SBC_W(R_Z80SP)
Z80FN _ld_xword_sp(void)	LDx_W(R_Z80SP)
Z80FN _in_a_c(void)			MCR_Z80INF(R_Z80A)
Z80FN _out_c_a(void)		MCR_Z80OUT(R_Z80A)
Z80FN _adc_hl_sp(void)		MCR_ADC_W(R_Z80SP)
Z80FN _ld_sp_xword(void)	LDW_x(R_Z80SP)

Z80FN _ldi(void)			MCR_LDI
Z80FN _cpi(void)			MCR_CPI
Z80FN _ini(void)			MCR_INI
Z80FN _outi(void)			MCR_OUTI
Z80FN _ldd(void)			MCR_LDD
Z80FN _cpd(void)			MCR_CPD
Z80FN _ind(void)			MCR_IND
Z80FN _outd(void)			MCR_OUTD

Z80FN _ldir(void)			MCR_LDIR
Z80FN _cpir(void)			MCR_CPIR
Z80FN _inir(void)			MCR_INIR
Z80FN _otir(void)			MCR_OTIR
Z80FN _lddr(void)			MCR_LDDR
Z80FN _cpdr(void)			MCR_CPDR
Z80FN _indr(void)			MCR_INDR
Z80FN _otdr(void)			MCR_OTDR


static const Z80OP z80c_subp[256] = {
	_ld_nop,		_ld_nop,		_ld_nop,		_ld_nop,		/* 00 */
	_ld_nop,		_ld_nop,		_ld_nop,		_ld_nop,
	_ld_nop,		_ld_nop,		_ld_nop,		_ld_nop,
	_ld_nop,		_ld_nop,		_ld_nop,		_ld_nop,

	_ld_nop,		_ld_nop,		_ld_nop,		_ld_nop,		/* 10 */
	_ld_nop,		_ld_nop,		_ld_nop,		_ld_nop,
	_ld_nop,		_ld_nop,		_ld_nop,		_ld_nop,
	_ld_nop,		_ld_nop,		_ld_nop,		_ld_nop,

	_ld_nop,		_ld_nop,		_ld_nop,		_ld_nop,		/* 20 */
	_ld_nop,		_ld_nop,		_ld_nop,		_ld_nop,
	_ld_nop,		_ld_nop,		_ld_nop,		_ld_nop,
	_ld_nop,		_ld_nop,		_ld_nop,		_ld_nop,

	_ld_nop,		_ld_nop,		_ld_nop,		_ld_nop,		/* 30 */
	_ld_nop,		_ld_nop,		_ld_nop,		_ld_nop,
	_ld_nop,		_ld_nop,		_ld_nop,		_ld_nop,
	_ld_nop,		_ld_nop,		_ld_nop,		_ld_nop,

	_in_b_c,		_out_c_b,		_sbc_hl_bc,		_ld_xword_bc,	/* 40 */
	_neg,			_retn,			_im_0,			_ld_i_a,
	_in_c_c,		_out_c_c,		_adc_hl_bc,		_ld_bc_xword,
	_neg,			_reti,			_im_0,			_ld_r_a,

	_in_d_c,		_out_c_d,		_sbc_hl_de,		_ld_xword_de,	/* 50 */
	_neg,			_retn,			_im_1,			_ld_a_i,
	_in_e_c,		_out_c_e,		_adc_hl_de,		_ld_de_xword,
	_neg,			_reti,			_im_2,			_ld_a_r,

	_in_h_c,		_out_c_h,		_sbc_hl_hl,		_ld_xword_hl,	/* 60 */
	_neg,			_retn,			_im_0,			_rrd,
	_in_l_c,		_out_c_l,		_adc_hl_hl,		_ld_hl_xword,
	_neg,			_reti,			_im_0,			_rld,

	_in_0_c,		_out_c_0,		_sbc_hl_sp,		_ld_xword_sp,	/* 70 */
	_neg,			_retn,			_im_1,			_ld_nop,
	_in_a_c,		_out_c_a,		_adc_hl_sp,		_ld_sp_xword,
	_neg,			_reti,			_im_2,			_ld_nop,

	_ld_nop,		_ld_nop,		_ld_nop,		_ld_nop,		/* 80 */
	_ld_nop,		_ld_nop,		_ld_nop,		_ld_nop,
	_ld_nop,		_ld_nop,		_ld_nop,		_ld_nop,
	_ld_nop,		_ld_nop,		_ld_nop,		_ld_nop,

	_ld_nop,		_ld_nop,		_ld_nop,		_ld_nop,		/* 90 */
	_ld_nop,		_ld_nop,		_ld_nop,		_ld_nop,
	_ld_nop,		_ld_nop,		_ld_nop,		_ld_nop,
	_ld_nop,		_ld_nop,		_ld_nop,		_ld_nop,

	_ldi,			_cpi,			_ini,			_outi,			/* a0 */
	_ld_nop,		_ld_nop,		_ld_nop,		_ld_nop,
	_ldd,			_cpd,			_ind,			_outd,
	_ld_nop,		_ld_nop,		_ld_nop,		_ld_nop,

	_ldir,			_cpir,			_inir,			_otir,			/* b0 */
	_ld_nop,		_ld_nop,		_ld_nop,		_ld_nop,
	_lddr,			_cpdr,			_indr,			_otdr,
	_ld_nop,		_ld_nop,		_ld_nop,		_ld_nop,

	_ld_nop,		_ld_nop,		_ld_nop,		_ld_nop,		/* c0 */
	_ld_nop,		_ld_nop,		_ld_nop,		_ld_nop,
	_ld_nop,		_ld_nop,		_ld_nop,		_ld_nop,
	_ld_nop,		_ld_nop,		_ld_nop,		_ld_nop,

	_ld_nop,		_ld_nop,		_ld_nop,		_ld_nop,		/* d0 */
	_ld_nop,		_ld_nop,		_ld_nop,		_ld_nop,
	_ld_nop,		_ld_nop,		_ld_nop,		_ld_nop,
	_ld_nop,		_ld_nop,		_ld_nop,		_ld_nop,

	_ld_nop,		_ld_nop,		_ld_nop,		_ld_nop,		/* e0 */
	_ld_nop,		_ld_nop,		_ld_nop,		_ld_nop,
	_ld_nop,		_ld_nop,		_ld_nop,		_ld_nop,
	_ld_nop,		_ld_nop,		_ld_nop,		_ld_nop,

	_ld_nop,		_ld_nop,		_ld_nop,		_ld_nop,		/* f0 */
	_ld_nop,		_ld_nop,		_ld_nop,		_ld_nop,
	_ld_nop,		_ld_nop,		_ld_nop,		_ld_nop,
	_ld_nop,		_ld_nop,		_ld_nop,		_ld_nop
};


void z80c_sub(void) {

	UINT	tmp;

	R_Z80R++;
	GET_PC_BYTE(tmp);
	Z80_COUNT(cycles_ed[tmp]);
	z80c_subp[tmp]();
}

