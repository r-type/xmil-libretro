
	.include	"../z80aadv/z80a.inc"
	.include	"../z80aadv/z80a_mn.inc"
	.include	"../z80aadv/z80a_s.inc"

	.global	z80a_sb

	.extern	_mem_read16
	.extern	_mem_write16_ret

	.extern	_outx
	.extern	_otxr
	.extern	_out_c_0
	.extern	_out_c_a
	.extern	_out_c_r
	.extern	_reti
	.extern	_retn
	.extern	_ld_xword_hl4
	.extern	_ld_xword_r3
	.extern	_ld_hl_xword4
	.extern	_ld_r3_xword

@	AREA	.text, CODE, READONLY

	.section	.iwram.text
	.code	32
	.align	0


_in_b_c:		Z80WORK	#8
				INPR8	CPU_B
_in_c_c:		Z80WORK	#8
				INPR8	CPU_C
_in_d_c:		Z80WORK	#8
				INPR8	CPU_D
_in_e_c:		Z80WORK	#8
				INPR8	CPU_E
_in_h_c:		Z80WORK	#8
				INPR8	CPU_H
_in_l_c:		Z80WORK	#8
				INPR8	CPU_L
_in_0_c:		Z80WORK	#8
				INP8
				mov		pc, r11
_in_a_c:		Z80WORK	#8
				ldrh	r0, [r4, #CPU_BC]
				Z80INP
				add		r1, r4, #CPU_SZPFLAG
				and		r2, rAF, #C_FLAG
				ldrb	r3, [r1, r0]
				orr		r12, r2, r0, lsl #8
				orr		rAF, r12, r3
				mov		pc, r11


_ini:			Z80WORK	#12
				ldrh	r0, [r4, #CPU_BC]
				Z80INP
				ldrh	r1, [r4, #CPU_HL]
				ldrb	r2, [r4, #CPU_B]
				MEMWR8	r1, r0
				add		r1, r1, #1
				subs	r2, r2, #1
				strh	r1, [r4, #CPU_HL]
				strb	r2, [r4, #CPU_B]
				and		rAF, rAF, #(0xff << 8)
				orr		rAF, rAF, #N_FLAG
				orreq	rAF, rAF, #Z_FLAG
				mov		pc, r11

_ind:			Z80WORK	#12
				ldrh	r0, [r4, #CPU_BC]
				Z80INP
				ldrh	r1, [r4, #CPU_HL]
				ldrb	r2, [r4, #CPU_B]
				MEMWR8	r1, r0
				sub		r1, r1, #1
				subs	r2, r2, #1
				strh	r1, [r4, #CPU_HL]
				strb	r2, [r4, #CPU_B]
				and		rAF, rAF, #(0xff << 8)
				orr		rAF, rAF, #N_FLAG
				orreq	rAF, rAF, #Z_FLAG
				mov		pc, r11

_inir:			Z80WORK	#12
				ldrh	r0, [r4, #CPU_BC]
				Z80INP
				ldrh	r1, [r4, #CPU_HL]
				ldrb	r2, [r4, #CPU_B]
				MEMWR8	r1, r0
				add		r1, r1, #1
				subs	r2, r2, #1
				strh	r1, [r4, #CPU_HL]
				strb	r2, [r4, #CPU_B]
				and		rAF, rAF, #(0xff << 8)
				orr		rAF, rAF, #N_FLAG
				orreq	rAF, rAF, #Z_FLAG
				subne	r8, r8, #(2 << 16)
				subne	rREMCLOCK, rREMCLOCK, #5
				mov		pc, r11

_indr:			Z80WORK	#12
				ldrh	r0, [r4, #CPU_BC]
				Z80INP
				ldrh	r1, [r4, #CPU_HL]
				ldrb	r2, [r4, #CPU_B]
				MEMWR8	r1, r0
				sub		r1, r1, #1
				subs	r2, r2, #1
				strh	r1, [r4, #CPU_HL]
				strb	r2, [r4, #CPU_B]
				and		rAF, rAF, #(0xff << 8)
				orr		rAF, rAF, #N_FLAG
				orreq	rAF, rAF, #Z_FLAG
				subne	r8, r8, #(2 << 16)
				subne	rREMCLOCK, rREMCLOCK, #5
				mov		pc, r11


_im_0:			Z80WORK	#4
				bic		r9, r9, #(0x03 << 16)
				mov		pc, r11
_im_1:			Z80WORK	#4
				bic		r9, r9, #(0x03 << 16)
				orr		r9, r9, #(0x01 << 16)
				mov		pc, r11
_im_2:			Z80WORK	#4
				bic		r9, r9, #(0x03 << 16)
				orr		r9, r9, #(0x02 << 16)
				mov		pc, r11


_ld_i_a:		Z80WORK	#5
				and		r0, rAF, #(0xff << 8)
				bic		r9, r9, #(0xff << 8)
				orr		r9, r9, r0
				mov		pc, r11

_ld_a_i:		Z80WORK	#5
				and		r0, r9, #(0xff << 8)
				and		r1, rAF, #C_FLAG
				orr		rAF, rAF, r0
				movs	r1, r0, lsl #16
				orreq	rAF, rAF, #Z_FLAG
				orrmi	rAF, rAF, #S_FLAG
				tst		r9, #(1 << IFF_IFLAG)
				orreq	rAF, rAF, #V_FLAG
				mov		pc, r11

_ld_r_a:		Z80WORK	#5
				mov		r0, rAF, lsr #8
				bic		r9, r9, #(0xff << 24)
				strb	r0, [r4, #CPU_R2]
				orr		r9, r9, r0, lsl #24
				mov		pc, r11

_ld_a_r:		Z80WORK	#5
				ldrb	r0, [r4, #CPU_R2]
				and		r1, r9, #(0x7f << 24)
				and		r2, rAF, #C_FLAG
				tst		r0, #0x80
				adds	r1, r1, #(0x80 << 24)
				orreq	r2, r2, #Z_FLAG
				orrmi	r2, r2, #S_FLAG
				tst		r9, #(1 << IFF_IFLAG)
				orreq	r2, r2, #V_FLAG
				orr		rAF, r2, r1, lsr #16
				mov		pc, r11


_ld_xword_bc:	Z80WORK	#16
				mov		r3, #CPU_BC
				b		_ld_xword_r3+4

_ld_xword_de:	Z80WORK	#16
				mov		r3, #CPU_DE
				b		_ld_xword_r3+4

_ld_xword_sp:	Z80WORK	#16
				GETPC16
				mov		r1, r8
				b		_mem_write16_ret

_ld_bc_xword:	Z80WORK	#16
				mov		r3, #CPU_BC
				b		_ld_r3_xword+4

_ld_de_xword:	Z80WORK	#16
				mov		r3, #CPU_DE
				b		_ld_r3_xword+4

_ld_sp_xword:	Z80WORK	#16
				GETPC16
				mov		r8, r8, lsr #16
				bl		_mem_read16
				orr		r8, r0, r8, lsl #16
				mov		pc, r11


_sbc_hl_16:		Z80WORK	#11
				mov		r1, #6
				and		r3, r1, r0, lsr #3
				MSBC16	r3

_sbc_hl_sp:		Z80WORK	#11
				MSBCSP
; @ _sbc_hl_hl	MSBCHL2


_adc_hl_16:		Z80WORK	#11
				mov		r1, #6
				and		r3, r1, r0, lsr #3
				MADC16	r3

_adc_hl_sp:		Z80WORK	#11
				MADCSP
; @ _adc_hl_hl	MADCHL2



_rrd:			Z80WORK	#14
				ldrh	r0, [r4, #CPU_HL]
				and		r1, rAF, #0x0f00
				MEMRD8	r12, r0
				mov		r1, r1, lsr #4
				and		r2, r12, #0x0f
				bic		rAF, rAF, #0x0f00
				orr		rAF, rAF, r2, lsl #8
				orr		r1, r1, r12, lsr #4
				add		r2, r4, #CPU_SZPFLAG
				bic		r12, rAF, #(0xff - C_FLAG)
				ldrb	rAF, [r2, rAF, lsr #8]
				MEMWR8	r0, r1
				orr		rAF, rAF, r12
				mov		pc, r11

_rld:			Z80WORK	#14
				ldrh	r0, [r4, #CPU_HL]
				and		r1, rAF, #0x0f00
				MEMRD8	r12, r0
				mov		r1, r1, lsr #8
				and		r2, r12, #0xf0
				bic		rAF, rAF, #0x0f00
				orr		rAF, rAF, r2, lsl #4
				orr		r1, r1, r12, lsl #4
				add		r2, r4, #CPU_SZPFLAG
				bic		r12, rAF, #(0xff - C_FLAG)
				ldrb	rAF, [r2, rAF, lsr #8]
				MEMWR8	r0, r1
				orr		rAF, rAF, r12
				mov		pc, r11

_neg:			Z80WORK	#4
				and		r0, rAF, #(0xff << 8)
				mov		r1, #0
				subs	r2, r1, r0, lsl #16
				orr		r3, r2, #(N_FLAG << 16)
				eor		r1, r2, r0, lsl #16
				orrcc	r3, r3, #(C_FLAG << 16)
				orrvs	r3, r3, #(V_FLAG << 16)
				orreq	r3, r3, #(Z_FLAG << 16)
				orrmi	r3, r3, #(S_FLAG << 16)
				tst		r1, #(H_FLAG << 24)
				mov		rAF, r3, lsr #16
				orrne	rAF, rAF, #H_FLAG
				mov		pc, r11


_ldi:			Z80WORK	#12
				ldrh	r0, [r4, #CPU_HL]
				ldrh	r1, [r4, #CPU_DE]
				ldrh	r2, [r4, #CPU_BC]
				MEMRD8	r3, r0
				MEMWR8	r1, r3
				add		r0, r0, #1
				add		r1, r1, #1
				subs	r2, r2, #1
				strh	r0, [r4, #CPU_HL]
				strh	r1, [r4, #CPU_DE]
				strh	r2, [r4, #CPU_BC]
				bic		rAF, rAF, #(0xff - 0xe9)
				orrne	rAF, rAF, #V_FLAG
				mov		pc, r11

_ldd:			Z80WORK	#12
				ldrh	r0, [r4, #CPU_HL]
				ldrh	r1, [r4, #CPU_DE]
				ldrh	r2, [r4, #CPU_BC]
				MEMRD8	r3, r0
				MEMWR8	r1, r3
				sub		r0, r0, #1
				sub		r1, r1, #1
				subs	r2, r2, #1
				strh	r0, [r4, #CPU_HL]
				strh	r1, [r4, #CPU_DE]
				strh	r2, [r4, #CPU_BC]
				bic		rAF, rAF, #(0xff - 0xe9)
				orrne	rAF, rAF, #V_FLAG
				mov		pc, r11

_ldir:			Z80WORK	#12
				ldrh	r0, [r4, #CPU_HL]
				ldrh	r1, [r4, #CPU_DE]
				ldrh	r2, [r4, #CPU_BC]
				MEMRD8	r3, r0
				MEMWR8	r1, r3
				add		r0, r0, #1
				add		r1, r1, #1
				subs	r2, r2, #1
				strh	r0, [r4, #CPU_HL]
				strh	r1, [r4, #CPU_DE]
				strh	r2, [r4, #CPU_BC]
				bic		rAF, rAF, #(0xff - 0xe9)
				orrne	rAF, rAF, #V_FLAG
				subne	r8, r8, #(2 << 16)
				subne	rREMCLOCK, rREMCLOCK, #5
				mov		pc, r11

_lddr:			Z80WORK	#12
				ldrh	r0, [r4, #CPU_HL]
				ldrh	r1, [r4, #CPU_DE]
				ldrh	r2, [r4, #CPU_BC]
				MEMRD8	r3, r0
				MEMWR8	r1, r3
				sub		r0, r0, #1
				sub		r1, r1, #1
				subs	r2, r2, #1
				strh	r0, [r4, #CPU_HL]
				strh	r1, [r4, #CPU_DE]
				strh	r2, [r4, #CPU_BC]
				bic		rAF, rAF, #(0xff - 0xe9)
				orrne	rAF, rAF, #V_FLAG
				subne	r8, r8, #(2 << 16)
				subne	rREMCLOCK, rREMCLOCK, #5
				mov		pc, r11


_cpi:			Z80WORK	#12
				ldrh	r0, [r4, #CPU_HL]
				MEMRD8	r1, r0					; @ T
				add		r0, r0, #1
				mov		r2, rAF, lsr #8			; @ A
				strh	r0, [r4, #CPU_HL]
				subs	r3, r2, r1
				bic		rAF, rAF, #(0xff - C_FLAG)
				eor		r0, r3, r2
				orreq	rAF, rAF, #Z_FLAG
				eor		r0, r0, r1
				and		r2, r3, #0x80
				and		r0, r0, #H_FLAG
				orr		rAF, rAF, r2
				ldrh	r1, [r4, #CPU_BC]
				orr		r0, r0, #N_FLAG
				subs	r1, r1, #1
				orr		rAF, r0, rAF
				strh	r1, [r4, #CPU_BC]
				orrne	rAF, rAF, #V_FLAG
				mov		pc, r11

_cpd:			Z80WORK	#12
				ldrh	r0, [r4, #CPU_HL]
				MEMRD8	r1, r0					; @ T
				sub		r0, r0, #1
				mov		r2, rAF, lsr #8			; @ A
				strh	r0, [r4, #CPU_HL]
				subs	r3, r2, r1
				bic		rAF, rAF, #(0xff - C_FLAG)
				eor		r0, r3, r2
				orreq	rAF, rAF, #Z_FLAG
				eor		r0, r0, r1
				and		r2, r3, #0x80
				and		r0, r0, #H_FLAG
				orr		rAF, rAF, r2
				ldrh	r1, [r4, #CPU_BC]
				orr		r0, r0, #N_FLAG
				subs	r1, r1, #1
				orr		rAF, r0, rAF
				strh	r1, [r4, #CPU_BC]
				orrne	rAF, rAF, #V_FLAG
				mov		pc, r11

_cpir:			Z80WORK	#12
				ldrh	r0, [r4, #CPU_HL]
				MEMRD8	r1, r0					; @ T
				add		r0, r0, #1
				mov		r2, rAF, lsr #8			; @ A
				strh	r0, [r4, #CPU_HL]
				subs	r3, r2, r1
				eor		r0, r3, r2
				bic		rAF, rAF, #(0xff - C_FLAG)
				eor		r0, r0, r1
				orr		rAF, rAF, #N_FLAG
				and		r0, r0, #H_FLAG
				ldrh	r1, [r4, #CPU_BC]
				orr		rAF, r0, rAF
				beq		1000f
				and		r2, r3, #0x80
				subs	r1, r1, #1
				orr		rAF, rAF, r2
				strh	r1, [r4, #CPU_BC]
				orrne	rAF, rAF, #V_FLAG
				subne	r8, r8, #(2 << 16)
				subne	rREMCLOCK, rREMCLOCK, #5
				mov		pc, r11
1000:			orr		rAF, rAF, #Z_FLAG
				subs	r1, r1, #1
				orrne	rAF, rAF, #V_FLAG
				strh	r1, [r4, #CPU_BC]
				mov		pc, r11

_cpdr:			Z80WORK	#12
				ldrh	r0, [r4, #CPU_HL]
				MEMRD8	r1, r0					; @ T
				sub		r0, r0, #1
				mov		r2, rAF, lsr #8			; @ A
				strh	r0, [r4, #CPU_HL]
				subs	r3, r2, r1
				eor		r0, r3, r2
				bic		rAF, rAF, #(0xff - C_FLAG)
				eor		r0, r0, r1
				orr		rAF, rAF, #N_FLAG
				and		r0, r0, #H_FLAG
				ldrh	r1, [r4, #CPU_BC]
				orr		rAF, r0, rAF
				beq		1100f
				and		r2, r3, #0x80
				subs	r1, r1, #1
				orr		rAF, rAF, r2
				strh	r1, [r4, #CPU_BC]
				orrne	rAF, rAF, #V_FLAG
				subne	r8, r8, #(2 << 16)
				subne	rREMCLOCK, rREMCLOCK, #5
				mov		pc, r11
1100:			orr		rAF, rAF, #Z_FLAG
				subs	r1, r1, #1
				orrne	rAF, rAF, #V_FLAG
				strh	r1, [r4, #CPU_BC]
				mov		pc, r11


; @ ----

z80a_sb:		GETPC8
				add		r9, r9, #(1 << 24)
				sub		r12, r0, #0x40
				cmp		r12, #0x80
				ldrcc	pc, [pc, r12, lsl #2]
_ld_nop:		mov		pc, r11

		.word	_in_b_c,		_out_c_r,		_sbc_hl_16,		_ld_xword_bc
		.word	_neg,			_retn,			_im_0,			_ld_i_a
		.word	_in_c_c,		_out_c_r,		_adc_hl_16,		_ld_bc_xword
		.word	_neg,			_reti,			_im_0,			_ld_r_a

		.word	_in_d_c,		_out_c_r,		_sbc_hl_16,		_ld_xword_de
		.word	_neg,			_retn,			_im_1,			_ld_a_i
		.word	_in_e_c,		_out_c_r,		_adc_hl_16,		_ld_de_xword
		.word	_neg,			_reti,			_im_2,			_ld_a_r

		.word	_in_h_c,		_out_c_r,		_sbc_hl_16,		_ld_xword_hl4
		.word	_neg,			_retn,			_im_0,			_rrd
		.word	_in_l_c,		_out_c_r,		_adc_hl_16,		_ld_hl_xword4
		.word	_neg,			_reti,			_im_0,			_rld

		.word	_in_0_c,		_out_c_0,		_sbc_hl_sp,		_ld_xword_sp
		.word	_neg,			_retn,			_im_1,			_ld_nop
		.word	_in_a_c,		_out_c_a,		_adc_hl_sp,		_ld_sp_xword
		.word	_neg,			_reti,			_im_2,			_ld_nop

		.word	_ld_nop,		_ld_nop,		_ld_nop,		_ld_nop
		.word	_ld_nop,		_ld_nop,		_ld_nop,		_ld_nop
		.word	_ld_nop,		_ld_nop,		_ld_nop,		_ld_nop
		.word	_ld_nop,		_ld_nop,		_ld_nop,		_ld_nop

		.word	_ld_nop,		_ld_nop,		_ld_nop,		_ld_nop
		.word	_ld_nop,		_ld_nop,		_ld_nop,		_ld_nop
		.word	_ld_nop,		_ld_nop,		_ld_nop,		_ld_nop
		.word	_ld_nop,		_ld_nop,		_ld_nop,		_ld_nop

		.word	_ldi,			_cpi,			_ini,			_outx
		.word	_ld_nop,		_ld_nop,		_ld_nop,		_ld_nop
		.word	_ldd,			_cpd,			_ind,			_outx
		.word	_ld_nop,		_ld_nop,		_ld_nop,		_ld_nop

		.word	_ldir,			_cpir,			_inir,			_otxr
		.word	_ld_nop,		_ld_nop,		_ld_nop,		_ld_nop
		.word	_lddr,			_cpdr,			_indr,			_otxr
		.word	_ld_nop,		_ld_nop,		_ld_nop,		_ld_nop

	.end

