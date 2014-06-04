
	INCLUDE	z80a.inc
	INCLUDE	z80a_mn.inc
	INCLUDE	z80a_i.inc

	EXPORT	z80a_ix
	EXPORT	z80a_iy

	IMPORT	z80a_ixcb
	IMPORT	z80a_iycb
	IMPORT	mem_read16
	IMPORT	mem_write16

	AREA	.text, CODE, READONLY


_no_op			sub		r9, r9, #&10000
_ld_nop			mov		pc, r11

_ld_ix_word		LD16w	CPU_IX
_ld_iy_word		LD16w	CPU_IY

_ld_xword_ix	LDx16	CPU_IX
_ld_xword_iy	LDx16	CPU_IY

_ld_ix_xword	LD16x	CPU_IX
_ld_iy_xword	LD16x	CPU_IY

_ld_xix_byte	LDxI_b	CPU_IX
_ld_xiy_byte	LDxI_b	CPU_IY

_ld_sp_ix		MLDSP	CPU_IX
_ld_sp_iy		MLDSP	CPU_IY

_add_ix_bc		MADD16	CPU_IX, CPU_BC
_add_ix_de		MADD16	CPU_IX, CPU_DE
_add_ix_ix		MADD16D	CPU_IX
_add_ix_sp		MADDSP	CPU_IX

_add_iy_bc		MADD16	CPU_IY, CPU_BC
_add_iy_de		MADD16	CPU_IY, CPU_DE
_add_iy_iy		MADD16D	CPU_IY
_add_iy_sp		MADDSP	CPU_IY

_inc_ix			MINC16	CPU_IX
_inc_iy			MINC16	CPU_IY

_inc_xix		MINCI8	CPU_IX
_inc_xiy		MINCI8	CPU_IY

_dec_ix			MDEC16	CPU_IX
_dec_iy			MDEC16	CPU_IY

_dec_xix		MDECI8	CPU_IX
_dec_xiy		MDECI8	CPU_IY

_push_ix		MPUSH	CPU_IX
_push_iy		MPUSH	CPU_IY

_pop_ix			MPOP	CPU_IX
_pop_iy			MPOP	CPU_IY

_ex_xsp_ix		MEXSP	CPU_IX
_ex_xsp_iy		MEXSP	CPU_IY

_jp_ix			MJPR16	CPU_IX
_jp_iy			MJPR16	CPU_IY



; ---- 8bit...

_inc_ixh		MINC8	CPU_IXH
_inc_ixl		MINC8	CPU_IXL
_inc_iyh		MINC8	CPU_IYH
_inc_iyl		MINC8	CPU_IYL

_dec_ixh		MDEC8	CPU_IXH
_dec_ixl		MDEC8	CPU_IXL
_dec_iyh		MDEC8	CPU_IYH
_dec_iyl		MDEC8	CPU_IYL

_ld_ixh_byte	LD8b	CPU_IXH
_ld_ixl_byte	LD8b	CPU_IXL
_ld_iyh_byte	LD8b	CPU_IYH
_ld_iyl_byte	LD8b	CPU_IYL

_ld_b_ixh		LD8		CPU_B, CPU_IXH
_ld_b_ixl		LD8		CPU_B, CPU_IXL
_ld_b_xix		LD8_xI	CPU_B, CPU_IX
_ld_b_iyh		LD8		CPU_B, CPU_IYH
_ld_b_iyl		LD8		CPU_B, CPU_IYL
_ld_b_xiy		LD8_xI	CPU_B, CPU_IY

_ld_c_ixh		LD8		CPU_C, CPU_IXH
_ld_c_ixl		LD8		CPU_C, CPU_IXL
_ld_c_xix		LD8_xI	CPU_C, CPU_IX
_ld_c_iyh		LD8		CPU_C, CPU_IYH
_ld_c_iyl		LD8		CPU_C, CPU_IYL
_ld_c_xiy		LD8_xI	CPU_C, CPU_IY

_ld_d_ixh		LD8		CPU_D, CPU_IXH
_ld_d_ixl		LD8		CPU_D, CPU_IXL
_ld_d_xix		LD8_xI	CPU_D, CPU_IX
_ld_d_iyh		LD8		CPU_D, CPU_IYH
_ld_d_iyl		LD8		CPU_D, CPU_IYL
_ld_d_xiy		LD8_xI	CPU_D, CPU_IY

_ld_e_ixh		LD8		CPU_E, CPU_IXH
_ld_e_ixl		LD8		CPU_E, CPU_IXL
_ld_e_xix		LD8_xI	CPU_E, CPU_IX
_ld_e_iyh		LD8		CPU_E, CPU_IYH
_ld_e_iyl		LD8		CPU_E, CPU_IYL
_ld_e_xiy		LD8_xI	CPU_E, CPU_IY

_ld_ixh_b		LD8		CPU_IXH, CPU_B
_ld_ixh_c		LD8		CPU_IXH, CPU_C
_ld_ixh_d		LD8		CPU_IXH, CPU_D
_ld_ixh_e		LD8		CPU_IXH, CPU_E
_ld_ixh_ixl		LD8		CPU_IXH, CPU_IXL
_ld_h_xix		LD8_xI	CPU_H, CPU_IX
_ld_ixh_a		LD8A	CPU_IXH
_ld_iyh_b		LD8		CPU_IYH, CPU_B
_ld_iyh_c		LD8		CPU_IYH, CPU_C
_ld_iyh_d		LD8		CPU_IYH, CPU_D
_ld_iyh_e		LD8		CPU_IYH, CPU_E
_ld_iyh_iyl		LD8		CPU_IYH, CPU_IYL
_ld_h_xiy		LD8_xI	CPU_H, CPU_IY
_ld_iyh_a		LD8A	CPU_IYH

_ld_ixl_b		LD8		CPU_IXL, CPU_B
_ld_ixl_c		LD8		CPU_IXL, CPU_C
_ld_ixl_d		LD8		CPU_IXL, CPU_D
_ld_ixl_e		LD8		CPU_IXL, CPU_E
_ld_ixl_ixh		LD8		CPU_IXL, CPU_IXH
_ld_l_xix		LD8_xI	CPU_L, CPU_IX
_ld_ixl_a		LD8A	CPU_IXL
_ld_iyl_b		LD8		CPU_IYL, CPU_B
_ld_iyl_c		LD8		CPU_IYL, CPU_C
_ld_iyl_d		LD8		CPU_IYL, CPU_D
_ld_iyl_e		LD8		CPU_IYL, CPU_E
_ld_iyl_iyh		LD8		CPU_IYL, CPU_IYH
_ld_l_xiy		LD8_xI	CPU_L, CPU_IY
_ld_iyl_a		LD8A	CPU_IYL

_ld_xix_b		LDxI_8	CPU_IX, CPU_B
_ld_xix_c		LDxI_8	CPU_IX, CPU_C
_ld_xix_d		LDxI_8	CPU_IX, CPU_D
_ld_xix_e		LDxI_8	CPU_IX, CPU_E
_ld_xix_h		LDxI_8	CPU_IX, CPU_H
_ld_xix_l		LDxI_8	CPU_IX, CPU_L
_ld_xix_a		LDxI_A	CPU_IX
_ld_xiy_b		LDxI_8	CPU_IY, CPU_B
_ld_xiy_c		LDxI_8	CPU_IY, CPU_C
_ld_xiy_d		LDxI_8	CPU_IY, CPU_D
_ld_xiy_e		LDxI_8	CPU_IY, CPU_E
_ld_xiy_h		LDxI_8	CPU_IY, CPU_H
_ld_xiy_l		LDxI_8	CPU_IY, CPU_L
_ld_xiy_a		LDxI_A	CPU_IY

_ld_a_ixh		LDA8	CPU_IXH
_ld_a_ixl		LDA8	CPU_IXL
_ld_a_xix		LDA_xI	CPU_IX
_ld_a_iyh		LDA8	CPU_IYH
_ld_a_iyl		LDA8	CPU_IYL
_ld_a_xiy		LDA_xI	CPU_IY


_add_a_ixh		MADDR8	CPU_IXH
_add_a_ixl		MADDR8	CPU_IXL
_add_a_xix		MADDI8	CPU_IX
_add_a_iyh		MADDR8	CPU_IYH
_add_a_iyl		MADDR8	CPU_IYL
_add_a_xiy		MADDI8	CPU_IY

_adc_a_ixh		MADCR8	CPU_IXH
_adc_a_ixl		MADCR8	CPU_IXL
_adc_a_xix		MADCI8	CPU_IX
_adc_a_iyh		MADCR8	CPU_IYH
_adc_a_iyl		MADCR8	CPU_IYL
_adc_a_xiy		MADCI8	CPU_IY

_sub_ixh		MSUBR8	CPU_IXH
_sub_ixl		MSUBR8	CPU_IXL
_sub_xix		MSUBI8	CPU_IX
_sub_iyh		MSUBR8	CPU_IYH
_sub_iyl		MSUBR8	CPU_IYL
_sub_xiy		MSUBI8	CPU_IY

_sbc_a_ixh		MSBCR8	CPU_IXH
_sbc_a_ixl		MSBCR8	CPU_IXL
_sbc_a_xix		MSBCI8	CPU_IX
_sbc_a_iyh		MSBCR8	CPU_IYH
_sbc_a_iyl		MSBCR8	CPU_IYL
_sbc_a_xiy		MSBCI8	CPU_IY

_and_ixh		MANDR8	CPU_IXH
_and_ixl		MANDR8	CPU_IXL
_and_xix		MANDI8	CPU_IX
_and_iyh		MANDR8	CPU_IYH
_and_iyl		MANDR8	CPU_IYL
_and_xiy		MANDI8	CPU_IY

_xor_ixh		MXORR8	CPU_IXH
_xor_ixl		MXORR8	CPU_IXL
_xor_xix		MXORI8	CPU_IX
_xor_iyh		MXORR8	CPU_IYH
_xor_iyl		MXORR8	CPU_IYL
_xor_xiy		MXORI8	CPU_IY

_or_ixh			MORR8	CPU_IXH
_or_ixl			MORR8	CPU_IXL
_or_xix			MORI8	CPU_IX
_or_iyh			MORR8	CPU_IYH
_or_iyl			MORR8	CPU_IYL
_or_xiy			MORI8	CPU_IY

_cp_ixh			MCPR8	CPU_IXH
_cp_ixl			MCPR8	CPU_IXL
_cp_xix			MCPI8	CPU_IX
_cp_iyh			MCPR8	CPU_IYH
_cp_iyl			MCPR8	CPU_IYL
_cp_xiy			MCPI8	CPU_IY



z80a_ix			GETPC8
				adr		r1, clktbl
				add		r6, r6, #(1 << 24)
				add		r2, r1, #&100
				ldrb	r3, [r1, r0]
				ldr		r1, [r2, r0, lsl #2]
				rsb		r10, r3, r10
				mov		pc, r1

z80a_iy			GETPC8
				adr		r1, clktbl
				add		r6, r6, #(1 << 24)
				add		r2, r1, #&500
				ldrb	r3, [r1, r0]
				ldr		r1, [r2, r0, lsl #2]
				rsb		r10, r3, r10
				mov		pc, r1


clktbl			dcb	 0, 0, 0, 0, 0, 0, 0, 0, 0,15, 0, 0, 0, 0, 0, 0
				dcb	 0, 0, 0, 0, 0, 0, 0, 0, 0,15, 0, 0, 0, 0, 0, 0
				dcb	 0,14,20,10, 9, 9, 9, 0, 0,15,20,10, 9, 9, 9, 0
				dcb	 0, 0, 0, 0,23,23,19, 0, 0,15, 0, 0, 0, 0, 0, 0
				dcb	 0, 0, 0, 0, 9, 9,19, 0, 0, 0, 0, 0, 9, 9,19, 0
				dcb	 0, 0, 0, 0, 9, 9,19, 0, 0, 0, 0, 0, 9, 9,19, 0
				dcb	 9, 9, 9, 9, 9, 9,19, 9, 9, 9, 9, 9, 9, 9,19, 9
				dcb	19,19,19,19,19,19,19,19, 0, 0, 0, 0, 9, 9,19, 0
				dcb	 0, 0, 0, 0, 9, 9,19, 0, 0, 0, 0, 0, 9, 9,19, 0
				dcb	 0, 0, 0, 0, 9, 9,19, 0, 0, 0, 0, 0, 9, 9,19, 0
				dcb	 0, 0, 0, 0, 9, 9,19, 0, 0, 0, 0, 0, 9, 9,19, 0
				dcb	 0, 0, 0, 0, 9, 9,19, 0, 0, 0, 0, 0, 9, 9,19, 0
				dcb	 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
				dcb	 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
				dcb	 0,14, 0,23, 0,15, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0
				dcb	 0, 0, 0, 0, 0, 0, 0, 0, 0,10, 0, 0, 0, 0, 0, 0

ixtbl	dcd		_no_op,			_no_op,			_no_op,			_no_op
		dcd		_no_op,			_no_op,			_no_op,			_no_op
		dcd		_no_op,			_add_ix_bc,		_no_op,			_no_op
		dcd		_no_op,			_no_op,			_no_op,			_no_op

		dcd		_no_op,			_no_op,			_no_op,			_no_op
		dcd		_no_op,			_no_op,			_no_op,			_no_op
		dcd		_no_op,			_add_ix_de,		_no_op,			_no_op
		dcd		_no_op,			_no_op,			_no_op,			_no_op

		dcd		_no_op,			_ld_ix_word,	_ld_xword_ix,	_inc_ix
		dcd		_inc_ixh,		_dec_ixh,		_ld_ixh_byte,	_no_op
		dcd		_no_op,			_add_ix_ix,		_ld_ix_xword,	_dec_ix
		dcd		_inc_ixl,		_dec_ixl,		_ld_ixl_byte,	_no_op

		dcd		_no_op,			_no_op,			_no_op,			_no_op
		dcd		_inc_xix,		_dec_xix,		_ld_xix_byte,	_no_op
		dcd		_no_op,			_add_ix_sp,		_no_op,			_no_op
		dcd		_no_op,			_no_op,			_no_op,			_no_op

		dcd		_no_op,			_no_op,			_no_op,			_no_op
		dcd		_ld_b_ixh,		_ld_b_ixl,		_ld_b_xix,		_no_op
		dcd		_no_op,			_no_op,			_no_op,			_no_op
		dcd		_ld_c_ixh,		_ld_c_ixl,		_ld_c_xix,		_no_op

		dcd		_no_op,			_no_op,			_no_op,			_no_op
		dcd		_ld_d_ixh,		_ld_d_ixl,		_ld_d_xix,		_no_op
		dcd		_no_op,			_no_op,			_no_op,			_no_op
		dcd		_ld_e_ixh,		_ld_e_ixl,		_ld_e_xix,		_no_op

		dcd		_ld_ixh_b,		_ld_ixh_c,		_ld_ixh_d,		_ld_ixh_e
		dcd		_ld_nop,		_ld_ixh_ixl,	_ld_h_xix,		_ld_ixh_a
		dcd		_ld_ixl_b,		_ld_ixl_c,		_ld_ixl_d,		_ld_ixl_e
		dcd		_ld_ixl_ixh,	_ld_nop,		_ld_l_xix,		_ld_ixl_a

		dcd		_ld_xix_b,		_ld_xix_c,		_ld_xix_d,		_ld_xix_e
		dcd		_ld_xix_h,		_ld_xix_l,		_no_op,			_ld_xix_a
		dcd		_no_op,			_no_op,			_no_op,			_no_op
		dcd		_ld_a_ixh,		_ld_a_ixl,		_ld_a_xix,		_no_op

		dcd		_no_op,			_no_op,			_no_op,			_no_op
		dcd		_add_a_ixh,		_add_a_ixl,		_add_a_xix,		_no_op
		dcd		_no_op,			_no_op,			_no_op,			_no_op
		dcd		_adc_a_ixh,		_adc_a_ixl,		_adc_a_xix,		_no_op

		dcd		_no_op,			_no_op,			_no_op,			_no_op
		dcd		_sub_ixh,		_sub_ixl,		_sub_xix,		_no_op
		dcd		_no_op,			_no_op,			_no_op,			_no_op
		dcd		_sbc_a_ixh,		_sbc_a_ixl,		_sbc_a_xix,		_no_op

		dcd		_no_op,			_no_op,			_no_op,			_no_op
		dcd		_and_ixh,		_and_ixl,		_and_xix,		_no_op
		dcd		_no_op,			_no_op,			_no_op,			_no_op
		dcd		_xor_ixh,		_xor_ixl,		_xor_xix,		_no_op

		dcd		_no_op,			_no_op,			_no_op,			_no_op
		dcd		_or_ixh,		_or_ixl,		_or_xix,		_no_op
		dcd		_no_op,			_no_op,			_no_op,			_no_op
		dcd		_cp_ixh,		_cp_ixl,		_cp_xix,		_no_op

		dcd		_no_op,			_no_op,			_no_op,			_no_op
		dcd		_no_op,			_no_op,			_no_op,			_no_op
		dcd		_no_op,			_no_op,			_no_op,			z80a_ixcb
		dcd		_no_op,			_no_op,			_no_op,			_no_op

		dcd		_no_op,			_no_op,			_no_op,			_no_op
		dcd		_no_op,			_no_op,			_no_op,			_no_op
		dcd		_no_op,			_no_op,			_no_op,			_no_op
		dcd		_no_op,			_no_op,			_no_op,			_no_op

		dcd		_no_op,			_pop_ix,		_no_op,			_ex_xsp_ix
		dcd		_no_op,			_push_ix,		_no_op,			_no_op
		dcd		_no_op,			_jp_ix,			_no_op,			_no_op
		dcd		_no_op,			_no_op,			_no_op,			_no_op

		dcd		_no_op,			_no_op,			_no_op,			_no_op
		dcd		_no_op,			_no_op,			_no_op,			_no_op
		dcd		_no_op,			_ld_sp_ix,		_no_op,			_no_op
		dcd		_no_op,			_no_op,			_no_op,			_no_op


iytbl	dcd		_no_op,			_no_op,			_no_op,			_no_op
		dcd		_no_op,			_no_op,			_no_op,			_no_op
		dcd		_no_op,			_add_iy_bc,		_no_op,			_no_op
		dcd		_no_op,			_no_op,			_no_op,			_no_op

		dcd		_no_op,			_no_op,			_no_op,			_no_op
		dcd		_no_op,			_no_op,			_no_op,			_no_op
		dcd		_no_op,			_add_iy_de,		_no_op,			_no_op
		dcd		_no_op,			_no_op,			_no_op,			_no_op

		dcd		_no_op,			_ld_iy_word,	_ld_xword_iy,	_inc_iy
		dcd		_inc_iyh,		_dec_iyh,		_ld_iyh_byte,	_no_op
		dcd		_no_op,			_add_iy_iy,		_ld_iy_xword,	_dec_iy
		dcd		_inc_iyl,		_dec_iyl,		_ld_iyl_byte,	_no_op

		dcd		_no_op,			_no_op,			_no_op,			_no_op
		dcd		_inc_xiy,		_dec_xiy,		_ld_xiy_byte,	_no_op
		dcd		_no_op,			_add_iy_sp,		_no_op,			_no_op
		dcd		_no_op,			_no_op,			_no_op,			_no_op

		dcd		_no_op,			_no_op,			_no_op,			_no_op
		dcd		_ld_b_iyh,		_ld_b_iyl,		_ld_b_xiy,		_no_op
		dcd		_no_op,			_no_op,			_no_op,			_no_op
		dcd		_ld_c_iyh,		_ld_c_iyl,		_ld_c_xiy,		_no_op

		dcd		_no_op,			_no_op,			_no_op,			_no_op
		dcd		_ld_d_iyh,		_ld_d_iyl,		_ld_d_xiy,		_no_op
		dcd		_no_op,			_no_op,			_no_op,			_no_op
		dcd		_ld_e_iyh,		_ld_e_iyl,		_ld_e_xiy,		_no_op

		dcd		_ld_iyh_b,		_ld_iyh_c,		_ld_iyh_d,		_ld_iyh_e
		dcd		_ld_nop,		_ld_iyh_iyl,	_ld_h_xiy,		_ld_iyh_a
		dcd		_ld_iyl_b,		_ld_iyl_c,		_ld_iyl_d,		_ld_iyl_e
		dcd		_ld_iyl_iyh,	_ld_nop,		_ld_l_xiy,		_ld_iyl_a

		dcd		_ld_xiy_b,		_ld_xiy_c,		_ld_xiy_d,		_ld_xiy_e
		dcd		_ld_xiy_h,		_ld_xiy_l,		_no_op,			_ld_xiy_a
		dcd		_no_op,			_no_op,			_no_op,			_no_op
		dcd		_ld_a_iyh,		_ld_a_iyl,		_ld_a_xiy,		_no_op

		dcd		_no_op,			_no_op,			_no_op,			_no_op
		dcd		_add_a_iyh,		_add_a_iyl,		_add_a_xiy,		_no_op
		dcd		_no_op,			_no_op,			_no_op,			_no_op
		dcd		_adc_a_iyh,		_adc_a_iyl,		_adc_a_xiy,		_no_op

		dcd		_no_op,			_no_op,			_no_op,			_no_op
		dcd		_sub_iyh,		_sub_iyl,		_sub_xiy,		_no_op
		dcd		_no_op,			_no_op,			_no_op,			_no_op
		dcd		_sbc_a_iyh,		_sbc_a_iyl,		_sbc_a_xiy,		_no_op

		dcd		_no_op,			_no_op,			_no_op,			_no_op
		dcd		_and_iyh,		_and_iyl,		_and_xiy,		_no_op
		dcd		_no_op,			_no_op,			_no_op,			_no_op
		dcd		_xor_iyh,		_xor_iyl,		_xor_xiy,		_no_op

		dcd		_no_op,			_no_op,			_no_op,			_no_op
		dcd		_or_iyh,		_or_iyl,		_or_xiy,		_no_op
		dcd		_no_op,			_no_op,			_no_op,			_no_op
		dcd		_cp_iyh,		_cp_iyl,		_cp_xiy,		_no_op

		dcd		_no_op,			_no_op,			_no_op,			_no_op
		dcd		_no_op,			_no_op,			_no_op,			_no_op
		dcd		_no_op,			_no_op,			_no_op,			z80a_iycb
		dcd		_no_op,			_no_op,			_no_op,			_no_op

		dcd		_no_op,			_no_op,			_no_op,			_no_op
		dcd		_no_op,			_no_op,			_no_op,			_no_op
		dcd		_no_op,			_no_op,			_no_op,			_no_op
		dcd		_no_op,			_no_op,			_no_op,			_no_op

		dcd		_no_op,			_pop_iy,		_no_op,			_ex_xsp_iy
		dcd		_no_op,			_push_iy,		_no_op,			_no_op
		dcd		_no_op,			_jp_iy,			_no_op,			_no_op
		dcd		_no_op,			_no_op,			_no_op,			_no_op

		dcd		_no_op,			_no_op,			_no_op,			_no_op
		dcd		_no_op,			_no_op,			_no_op,			_no_op
		dcd		_no_op,			_ld_sp_iy,		_no_op,			_no_op
		dcd		_no_op,			_no_op,			_no_op,			_no_op

	END

