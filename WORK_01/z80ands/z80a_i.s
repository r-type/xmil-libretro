
XI		=		0x80000000

	.include	"../z80ands/z80a.inc"
	.include	"../z80ands/z80a_mn.inc"

	.global	z80a_ix
	.global	z80a_iy

	.extern	z80a_icb
	.extern	_inc_r
	.extern	_dec_r
	.extern	_add_a_r
	.extern	_adc_a_r
	.extern	_sub_r
	.extern	_sbc_a_r
	.extern	_and_r
	.extern	_xor_r
	.extern	_or_r
	.extern	_cp_r

	.extern	_add_r3_16
	.extern	_add_r3_r3
	.extern	_add_r3_sp
	.extern	_ld_xword_r3
	.extern	_ld_r3_xword
	.extern	_ld_xr2_byte
	.extern	_add_a_xr2
	.extern	_adc_a_xr2
	.extern	_sub_xr2
	.extern	_sbc_a_xr2
	.extern	_and_xr2
	.extern	_xor_xr2
	.extern	_or_xr2
	.extern	_cp_xr2
	.extern	_inc_xr2
	.extern	_dec_xr2
	.extern	_push_r3
	.extern	_pop_r3

	.section	.itcm
	.code	16
	.align	1


_ld_ih_b:		add		r3, #1
_ld_il_b:		sLD8	r3, #CPU_B

_ld_ih_c:		add		r3, #1
_ld_il_c:		sLD8	r3, #CPU_C

_ld_ih_d:		add		r3, #1
_ld_il_d:		sLD8	r3, #CPU_D

_ld_ih_e:		add		r3, #1
_ld_il_e:		sLD8	r3, #CPU_E

_ld_ih_a:		add		r3, #1
_ld_il_a:		sLD8A	r3


_ld_il_ih:		add		r0, r3, #1
				ldrb	r1, [r0, r4]
				strb	r1, [r3, r4]
				bx		r11

_ld_ih_il:		add		r0, r3, #1
				ldrb	r1, [r3, r4]
				strb	r1, [r0, r4]
				bx		r11


_ld_xi_a:		lsr		r0, rAF, #8
				MEMWR8	r2, r0
				bx		r11


_inc_i:			Z80WORK	#2
				sMINC16	r3

_dec_i:			Z80WORK	#2
				sMDEC16	r3


	.code	32
	.align	2


__inc_xr2:		Z80WORK	#4
				b		_inc_xr2
__dec_xr2:		Z80WORK	#4
				b		_dec_xr2


_no_op:			sub		r8, r8, #(1 << 16)
				add		rREMCLOCK, rREMCLOCK, #4
_ld_nop:		mov		pc, r11

_ld_i_word:		Z80WORK	#6
				LD16w	r3

_ld_sp_i:		Z80WORK	#2
				MLDSP	r3

_ex_xsp_i:		Z80WORK	#15
				MEXSP	r3

_jp_i:			MJPR16	r3



; @ ---- 8bit...

_ld_ih_byte:	add		r3, r3, #1
_ld_il_byte:	Z80WORK	#3
				LD8b	r3

_ld_r_ih:		add		r3, r3, #1
_ld_r_il:		eor		r2, r0, #0x4c			; @ 010??10?
				ldrb	r0, [r3, r4]
				strb	r0, [r4, r2, lsr #3]
				mov		pc, r11

_ld_r_xi:		eor		r0, r0, #0x4c			; @ 010??10?
				MEMRD8	r1, r2
				strb	r1, [r4, r0, lsr #3]
				mov		pc, r11

_ld_xi_r:		eor		r1, r0, #0x71			; @ 01110???
				ldrb	r0, [r1, r4]
				MEMWR8	r2, r0
				mov		pc, r11

_ld_a_ih:		add		r3, r3, #1
_ld_a_il:		LDA8	r3

_ld_a_xi:		MEMRD8	r0, r2
				and		r2, rAF, #0xff
				orr		rAF, r2, r0, lsl #8
				mov		pc, r11


z80a_ix:		adr		r3, optbl
				GETPC8
				ldr		r12, [r3, r0, lsl #2]
				Z80WORK	#4
				add		r9, r9, #(1 << 24)
				mov		r3, #CPU_IX
				tst		r12, #XI
				bxeq	r12
				adds	r8, r8, #(1 << 16)
				ldrmib	r2, [r5, r8, lsr #16]
				ldrplb	r2, [r10, r8, lsr #16]
				ldrh	r1, [r3, r4]
				tst		r2, #0x80
				addne	r2, r2, #0xff00
				add		r2, r2, r1
				bic		r2, r2, #(1 << 16)
				bic		r12, r12, #XI
				Z80WORK	#11
				bx		r12


z80a_iy:		adr		r3, optbl
				GETPC8
				ldr		r12, [r3, r0, lsl #2]
				Z80WORK	#4
				add		r9, r9, #(1 << 24)
				mov		r3, #CPU_IY
				tst		r12, #XI
				bxeq	r12
				adds	r8, r8, #(1 << 16)
				ldrmib	r2, [r5, r8, lsr #16]
				ldrplb	r2, [r10, r8, lsr #16]
				ldrh	r1, [r3, r4]
				tst		r2, #0x80
				addne	r2, r2, #0xff00
				add		r2, r2, r1
				bic		r2, r2, #(1 << 16)
				bic		r12, r12, #XI
				Z80WORK	#11
				bx		r12


optbl:	.word	_no_op,			_no_op,			_no_op,			_no_op
		.word	_no_op,			_no_op,			_no_op,			_no_op
		.word	_no_op,			_add_r3_16,		_no_op,			_no_op
		.word	_no_op,			_no_op,			_no_op,			_no_op

		.word	_no_op,			_no_op,			_no_op,			_no_op
		.word	_no_op,			_no_op,			_no_op,			_no_op
		.word	_no_op,			_add_r3_16,		_no_op,			_no_op
		.word	_no_op,			_no_op,			_no_op,			_no_op

		.word	_no_op,			_ld_i_word,		_ld_xword_r3,	_inc_i+1
		.word	_inc_r+4,		_dec_r+4,		_ld_ih_byte,	_no_op
		.word	_no_op,			_add_r3_r3,		_ld_r3_xword,	_dec_i+1
		.word	_inc_r+8,		_dec_r+8,		_ld_il_byte,	_no_op

		.word	_no_op,			_no_op,			_no_op,			_no_op
		.word	XI+__inc_xr2,	XI+__dec_xr2,	XI+_ld_xr2_byte,_no_op
		.word	_no_op,			_add_r3_sp,		_no_op,			_no_op
		.word	_no_op,			_no_op,			_no_op,			_no_op

		.word	_no_op,			_no_op,			_no_op,			_no_op
		.word	_ld_r_ih,		_ld_r_il,		XI+_ld_r_xi,	_no_op
		.word	_no_op,			_no_op,			_no_op,			_no_op
		.word	_ld_r_ih,		_ld_r_il,		XI+_ld_r_xi,	_no_op

		.word	_no_op,			_no_op,			_no_op,			_no_op
		.word	_ld_r_ih,		_ld_r_il,		XI+_ld_r_xi,	_no_op
		.word	_no_op,			_no_op,			_no_op,			_no_op
		.word	_ld_r_ih,		_ld_r_il,		XI+_ld_r_xi,	_no_op

		.word	_ld_ih_b+1,		_ld_ih_c+1,		_ld_ih_d+1,		_ld_ih_e+1
		.word	_ld_nop,		_ld_ih_il+1,	XI+_ld_r_xi,	_ld_ih_a+1
		.word	_ld_il_b+1,		_ld_il_c+1,		_ld_il_d+1,		_ld_il_e+1
		.word	_ld_il_ih+1,	_ld_nop,		XI+_ld_r_xi,	_ld_il_a+1

		.word	XI+_ld_xi_r,	XI+_ld_xi_r,	XI+_ld_xi_r,	XI+_ld_xi_r
		.word	XI+_ld_xi_r,	XI+_ld_xi_r,	_no_op,			XI+_ld_xi_a+1
		.word	_no_op,			_no_op,			_no_op,			_no_op
		.word	_ld_a_ih,		_ld_a_il,		XI+_ld_a_xi,	_no_op

		.word	_no_op,			_no_op,			_no_op,			_no_op
		.word	_add_a_r+4,		_add_a_r+8,		XI+_add_a_xr2,	_no_op
		.word	_no_op,			_no_op,			_no_op,			_no_op
		.word	_adc_a_r+4,		_adc_a_r+8,		XI+_adc_a_xr2,	_no_op

		.word	_no_op,			_no_op,			_no_op,			_no_op
		.word	_sub_r+4,		_sub_r+8,		XI+_sub_xr2,	_no_op
		.word	_no_op,			_no_op,			_no_op,			_no_op
		.word	_sbc_a_r+4,		_sbc_a_r+8,		XI+_sbc_a_xr2,	_no_op

		.word	_no_op,			_no_op,			_no_op,			_no_op
		.word	_and_r+4,		_and_r+8,		XI+_and_xr2,	_no_op
		.word	_no_op,			_no_op,			_no_op,			_no_op
		.word	_xor_r+4,		_xor_r+8,		XI+_xor_xr2,	_no_op

		.word	_no_op,			_no_op,			_no_op,			_no_op
		.word	_or_r+4,		_or_r+8,		XI+_or_xr2,		_no_op
		.word	_no_op,			_no_op,			_no_op,			_no_op
		.word	_cp_r+4,		_cp_r+8,		XI+_cp_xr2,		_no_op

		.word	_no_op,			_no_op,			_no_op,			_no_op
		.word	_no_op,			_no_op,			_no_op,			_no_op
		.word	_no_op,			_no_op,			_no_op,			XI+z80a_icb
		.word	_no_op,			_no_op,			_no_op,			_no_op

		.word	_no_op,			_no_op,			_no_op,			_no_op
		.word	_no_op,			_no_op,			_no_op,			_no_op
		.word	_no_op,			_no_op,			_no_op,			_no_op
		.word	_no_op,			_no_op,			_no_op,			_no_op

		.word	_no_op,			_pop_r3,		_no_op,			_ex_xsp_i
		.word	_no_op,			_push_r3,		_no_op,			_no_op
		.word	_no_op,			_jp_i,			_no_op,			_no_op
		.word	_no_op,			_no_op,			_no_op,			_no_op

		.word	_no_op,			_no_op,			_no_op,			_no_op
		.word	_no_op,			_no_op,			_no_op,			_no_op
		.word	_no_op,			_ld_sp_i,		_no_op,			_no_op
		.word	_no_op,			_no_op,			_no_op,			_no_op

	.end

