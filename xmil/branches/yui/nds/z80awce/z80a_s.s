
	INCLUDE	z80a.inc
	INCLUDE	z80a_mn.inc
	INCLUDE	z80a_s.inc

	EXPORT	z80a_sb

	IMPORT	iocore_out
	IMPORT	iocore_inp
	IMPORT	mem_read16
	IMPORT	mem_write16
	IMPORT	ievent_eoi

	AREA	.text, CODE, READONLY


_ld_nop			mov		pc, r11


_im_0			bic		r6, r6, #(&ff << 16)
				mov		pc, r11
_im_1			bic		r6, r6, #(&ff << 16)
				orr		r6, r6, #(&01 << 16)
				mov		pc, r11
_im_2			bic		r6, r6, #(&ff << 16)
				orr		r6, r6, #(&02 << 16)
				mov		pc, r11


_out_c_b		OUTR8	CPU_B
_out_c_c		OUTR8	CPU_C
_out_c_d		OUTR8	CPU_D
_out_c_e		OUTR8	CPU_E
_out_c_h		OUTR8	CPU_H
_out_c_l		OUTR8	CPU_L
_out_c_0		mov		r1, #0
				OUT8
_out_c_a		mov		r1, r8, lsr #8
				OUT8

_in_b_c			INPR8	CPU_B
_in_c_c			INPR8	CPU_C
_in_d_c			INPR8	CPU_D
_in_e_c			INPR8	CPU_E
_in_h_c			INPR8	CPU_H
_in_l_c			INPR8	CPU_L
_in_0_c			INP8
				mov		pc, r11
_in_a_c			ldrh	r0, [r7, #CPU_BC]
				str		r10, [r7, #CPU_REMCLOCK]
				bl		iocore_inp
				and		r2, r8, #C_FLAG
				ldrb	r1, [r0, r7]
				orr		r3, r2, r0, lsl #8
				ldr		r10, [r7, #CPU_REMCLOCK]
				orr		r8, r3, r1
				mov		pc, r11

_ld_i_a			and		r0, r8, #&ff00
				bic		r6, r6, #&ff00
				orr		r6, r6, r0
				mov		pc, r11

_ld_a_i			and		r0, r6, #&ff00
				and		r1, r8, #C_FLAG
				orr		r8, r8, r0
				movs	r1, r0, lsl #16
				orreq	r8, r8, #Z_FLAG
				orrmi	r8, r8, #S_FLAG
				tst		r6, #(1 << IFF_IFLAG)
				orreq	r8, r8, #V_FLAG
				mov		pc, r11

_ld_r_a			mov		r0, r8, lsr #8
				bic		r6, r6, #&ff000000
				strb	r0, [r7, #CPU_R2]
				orr		r6, r6, r0, lsl #24
				mov		pc, r11

_ld_a_r			ldrb	r0, [r7, #CPU_R2]
				and		r1, r6, #&7f000000
				and		r2, r8, #C_FLAG
				tst		r0, #&80
				adds	r1, r1, #&80000000
				orreq	r2, r2, #Z_FLAG
				orrmi	r2, r2, #S_FLAG
				tst		r6, #(1 << IFF_IFLAG)
				orreq	r2, r2, #V_FLAG
				orr		r8, r2, r1, lsr #16
				mov		pc, r11


_ld_xword_bc	LDx16	CPU_BC
_ld_xword_de	LDx16	CPU_DE
_ld_xword_hl	LDx16	CPU_HL

_ld_bc_xword	LD16x	CPU_BC
_ld_de_xword	LD16x	CPU_DE
_ld_hl_xword	LD16x	CPU_HL

_adc_hl_bc		MADC16	CPU_BC
_adc_hl_de		MADC16	CPU_DE
_adc_hl_hl		MADCHL2
_adc_hl_sp		MADCSP

_sbc_hl_bc		MSBC16	CPU_BC
_sbc_hl_de		MSBC16	CPU_DE
_sbc_hl_hl		MSBCHL2
_sbc_hl_sp		MSBCSP


_ld_xword_sp	GETPC16
				mov		r1, r9
				mov		lr, r11
				b		mem_write16

_ld_sp_xword	GETPC16
				mov		r9, r9, lsr #16
				bl		mem_read16
				orr		r9, r0, r9, lsl #16
				mov		pc, r11


_rrd			ldrh	r0, [r7, #CPU_HL]
				and		r1, r8, #&0f00
				MEMLEA8	r0, r2, r3
				ldrb	r12, [r2, r0]
				mov		r1, r1, lsr #4
				and		r2, r12, #&0f
				bic		r8, r8, #&0f00
				orr		r8, r8, r2, lsl #8
				orr		r1, r1, r12, lsr #4
				bic		r12, r8, #(&ff - C_FLAG)
				ldrb	r8, [r7, r8 lsr #8]
				strb	r1, [r3, r0]
				orr		r8, r8, r12
				mov		pc, r11

_rld			ldrh	r0, [r7, #CPU_HL]
				and		r1, r8, #&0f00
				MEMLEA8	r0, r2, r3
				ldrb	r12, [r2, r0]
				mov		r1, r1, lsr #8
				and		r2, r12, #&f0
				bic		r8, r8, #&0f00
				orr		r8, r8, r2, lsl #4
				orr		r1, r1, r12, lsl #4
				bic		r12, r8, #(&ff - C_FLAG)
				ldrb	r8, [r7, r8 lsr #8]
				strb	r1, [r3, r0]
				orr		r8, r8, r12
				mov		pc, r11

_neg			and		r0, r8, #&ff00
				mov		r1, #0
				subs	r2, r1, r0, lsl #16
				orr		r3, r2, #(N_FLAG << 16)
				eor		r1, r2, r0, lsl #16
				orrcc	r3, r3, #(C_FLAG << 16)
				orrvs	r3, r3, #(V_FLAG << 16)
				orreq	r3, r3, #(Z_FLAG << 16)
				orrmi	r3, r3, #(S_FLAG << 16)
				tst		r1, #(H_FLAG << 24)
				mov		r8, r3, lsr #16
				orrne	r8, r8, #H_FLAG
				mov		pc, r11

_retn			bic		r8, r8, #(1 << IFF_NMI)
				MRET

_reti			str		r10, [r7, #CPU_REMCLOCK]
				bl		ievent_eoi
				ldr		r10, [r7, #CPU_REMCLOCK]
				MRET


_ldi			ldrh	r0, [r7, #CPU_HL]
				ldrh	r1, [r7, #CPU_DE]
				ldrh	r2, [r7, #CPU_BC]
				MEMRD8	r3, r0
				MEMWR8	r1, r3
				add		r0, r0, #1
				add		r1, r1, #1
				subs	r2, r2, #1
				strh	r0, [r7, #CPU_HL]
				strh	r1, [r7, #CPU_DE]
				strh	r2, [r7, #CPU_BC]
				bic		r8, r8, #(&ff - &e9)
				orrne	r8, r8, #V_FLAG
				mov		pc, r11

_ldd			ldrh	r0, [r7, #CPU_HL]
				ldrh	r1, [r7, #CPU_DE]
				ldrh	r2, [r7, #CPU_BC]
				MEMRD8	r3, r0
				MEMWR8	r1, r3
				sub		r0, r0, #1
				sub		r1, r1, #1
				subs	r2, r2, #1
				strh	r0, [r7, #CPU_HL]
				strh	r1, [r7, #CPU_DE]
				strh	r2, [r7, #CPU_BC]
				bic		r8, r8, #(&ff - &e9)
				orrne	r8, r8, #V_FLAG
				mov		pc, r11

_ldir			ldrh	r0, [r7, #CPU_HL]
				ldrh	r1, [r7, #CPU_DE]
				ldrh	r2, [r7, #CPU_BC]
				MEMRD8	r3, r0
				MEMWR8	r1, r3
				add		r0, r0, #1
				add		r1, r1, #1
				subs	r2, r2, #1
				strh	r0, [r7, #CPU_HL]
				strh	r1, [r7, #CPU_DE]
				strh	r2, [r7, #CPU_BC]
				bic		r8, r8, #(&ff - &e9)
				orrne	r8, r8, #V_FLAG
				subne	r9, r9, #&20000
				subne	r10, r10, #5
				mov		pc, r11

_lddr			ldrh	r0, [r7, #CPU_HL]
				ldrh	r1, [r7, #CPU_DE]
				ldrh	r2, [r7, #CPU_BC]
				MEMRD8	r3, r0
				MEMWR8	r1, r3
				sub		r0, r0, #1
				sub		r1, r1, #1
				subs	r2, r2, #1
				strh	r0, [r7, #CPU_HL]
				strh	r1, [r7, #CPU_DE]
				strh	r2, [r7, #CPU_BC]
				bic		r8, r8, #(&ff - &e9)
				orrne	r8, r8, #V_FLAG
				subne	r9, r9, #&20000
				subne	r10, r10, #5
				mov		pc, r11


_outi			ldrh	r2, [r7, #CPU_HL]
				ldrh	r0, [r7, #CPU_BC]
				MEMRD8	r1, r2
				add		r2, r2, #1
				subs	r0, r0, #&100
				addcc	r0, r0, #&10000
				strh	r2, [r7, #CPU_HL]
				strh	r0, [r7, #CPU_BC]
				and		r8, r8, #&ff00
				tst		r0, #&ff00
				orr		r8, r8, #N_FLAG
				orreq	r8, r8, #Z_FLAG
				str		r10, [r7, #CPU_REMCLOCK]
				bl		iocore_out
				ldr		r10, [r7, #CPU_REMCLOCK]
				mov		pc, r11

_outd			ldrh	r2, [r7, #CPU_HL]
				ldrh	r0, [r7, #CPU_BC]
				MEMRD8	r1, r2
				sub		r2, r2, #1
				subs	r0, r0, #&100
				addcc	r0, r0, #&10000
				strh	r2, [r7, #CPU_HL]
				strh	r0, [r7, #CPU_BC]
				and		r8, r8, #&ff00
				tst		r0, #&ff00
				orr		r8, r8, #N_FLAG
				orreq	r8, r8, #Z_FLAG
				str		r10, [r7, #CPU_REMCLOCK]
				bl		iocore_out
				ldr		r10, [r7, #CPU_REMCLOCK]
				mov		pc, r11

_otir			ldrh	r2, [r7, #CPU_HL]
				ldrh	r0, [r7, #CPU_BC]
				MEMRD8	r1, r2
				add		r2, r2, #1
				subs	r0, r0, #&100
				addcc	r0, r0, #&10000
				strh	r2, [r7, #CPU_HL]
				strh	r0, [r7, #CPU_BC]
				and		r8, r8, #&ff00
				tst		r0, #&ff00
				orr		r8, r8, #N_FLAG
				subeq	r10, r10, #5
				orreq	r8, r8, #Z_FLAG
				subeq	r9, r9, #&20000
				str		r10, [r7, #CPU_REMCLOCK]
				bl		iocore_out
				ldr		r10, [r7, #CPU_REMCLOCK]
				mov		pc, r11

_otdr			ldrh	r2, [r7, #CPU_HL]
				ldrh	r0, [r7, #CPU_BC]
				MEMRD8	r1, r2
				sub		r2, r2, #1
				subs	r0, r0, #&100
				addcc	r0, r0, #&10000
				strh	r2, [r7, #CPU_HL]
				strh	r0, [r7, #CPU_BC]
				and		r8, r8, #&ff00
				tst		r0, #&ff00
				orr		r8, r8, #N_FLAG
				subeq	r10, r10, #5
				orreq	r8, r8, #Z_FLAG
				subeq	r9, r9, #&20000
				str		r10, [r7, #CPU_REMCLOCK]
				bl		iocore_out
				ldr		r10, [r7, #CPU_REMCLOCK]
				mov		pc, r11


_ini			ldrh	r0, [r7, #CPU_BC]
				bl		iocore_inp
				ldrh	r1, [r7, #CPU_HL]
				ldrb	r2, [r7, #CPU_B]
				MEMWR8	r1, r0
				add		r1, r1, #1
				subs	r2, r2, #1
				strh	r1, [r7, #CPU_HL]
				strb	r2, [r7, #CPU_B]
				and		r8, r8, #&ff00
				orr		r8, r8, #N_FLAG
				orreq	r8, r8, #Z_FLAG
				mov		pc, r11

_ind			ldrh	r0, [r7, #CPU_BC]
				bl		iocore_inp
				ldrh	r1, [r7, #CPU_HL]
				ldrb	r2, [r7, #CPU_B]
				MEMWR8	r1, r0
				sub		r1, r1, #1
				subs	r2, r2, #1
				strh	r1, [r7, #CPU_HL]
				strb	r2, [r7, #CPU_B]
				and		r8, r8, #&ff00
				orr		r8, r8, #N_FLAG
				orreq	r8, r8, #Z_FLAG
				mov		pc, r11

_inir			ldrh	r0, [r7, #CPU_BC]
				bl		iocore_inp
				ldrh	r1, [r7, #CPU_HL]
				ldrb	r2, [r7, #CPU_B]
				MEMWR8	r1, r0
				add		r1, r1, #1
				subs	r2, r2, #1
				strh	r1, [r7, #CPU_HL]
				strb	r2, [r7, #CPU_B]
				and		r8, r8, #&ff00
				orr		r8, r8, #N_FLAG
				orreq	r8, r8, #Z_FLAG
				subeq	r9, r9, #&20000
				subeq	r10, r10, #5
				mov		pc, r11

_indr			ldrh	r0, [r7, #CPU_BC]
				bl		iocore_inp
				ldrh	r1, [r7, #CPU_HL]
				ldrb	r2, [r7, #CPU_B]
				MEMWR8	r1, r0
				sub		r1, r1, #1
				subs	r2, r2, #1
				strh	r1, [r7, #CPU_HL]
				strb	r2, [r7, #CPU_B]
				and		r8, r8, #&ff00
				orr		r8, r8, #N_FLAG
				orreq	r8, r8, #Z_FLAG
				subeq	r9, r9, #&20000
				subeq	r10, r10, #5
				mov		pc, r11

_cpi			ldrh	r0, [r7, #CPU_HL]
				MEMRD8	r1, r0				; T
				add		r0, r0, #1
				mov		r2, r8, lsr #8		; A
				strh	r0, [r7, #CPU_HL]
				subs	r3, r2, r1
				bic		r8, r8, #(&ff - C_FLAG)
				eor		r0, r3, r2
				orreq	r8, r8, #Z_FLAG
				eor		r0, r0, r1
				and		r2, r3, #&80
				and		r0, r0, #H_FLAG
				orr		r8, r8, r2
				ldrh	r1, [r7, #CPU_BC]
				orr		r0, r0, #N_FLAG
				subs	r1, r1, #1
				orr		r8, r0, r8
				strh	r1, [r7, #CPU_BC]
				orrne	r8, r8, #V_FLAG
				mov		pc, r11

_cpd			ldrh	r0, [r7, #CPU_HL]
				MEMRD8	r1, r0				; T
				sub		r0, r0, #1
				mov		r2, r8, lsr #8		; A
				strh	r0, [r7, #CPU_HL]
				subs	r3, r2, r1
				bic		r8, r8, #(&ff - C_FLAG)
				eor		r0, r3, r2
				orreq	r8, r8, #Z_FLAG
				eor		r0, r0, r1
				and		r2, r3, #&80
				and		r0, r0, #H_FLAG
				orr		r8, r8, r2
				ldrh	r1, [r7, #CPU_BC]
				orr		r0, r0, #N_FLAG
				subs	r1, r1, #1
				orr		r8, r0, r8
				strh	r1, [r7, #CPU_BC]
				orrne	r8, r8, #V_FLAG
				mov		pc, r11

_cpir			ldrh	r0, [r7, #CPU_HL]
				MEMRD8	r1, r0				; T
				add		r0, r0, #1
				mov		r2, r8, lsr #8		; A
				strh	r0, [r7, #CPU_HL]
				subs	r3, r2, r1
				eor		r0, r3, r2
				bic		r8, r8, #(&ff - C_FLAG)
				eor		r0, r0, r1
				orr		r8, r8, #N_FLAG
				and		r0, r0, #H_FLAG
				ldrh	r1, [r7, #CPU_BC]
				orr		r8, r0, r8
				beq		__cpir0
				and		r2, r3, #&80
				subs	r1, r1, #1
				orr		r8, r8, r2
				strh	r1, [r7, #CPU_BC]
				orrne	r8, r8, #V_FLAG
				mov		pc, r11
__cpir0			orr		r8, r8, #Z_FLAG
				subs	r1, r1, #1
				orrne	r8, r8, #V_FLAG
				strh	r1, [r7, #CPU_BC]
				mov		pc, r11

_cpdr			ldrh	r0, [r7, #CPU_HL]
				MEMRD8	r1, r0				; T
				sub		r0, r0, #1
				mov		r2, r8, lsr #8		; A
				strh	r0, [r7, #CPU_HL]
				subs	r3, r2, r1
				eor		r0, r3, r2
				bic		r8, r8, #(&ff - C_FLAG)
				eor		r0, r0, r1
				orr		r8, r8, #N_FLAG
				and		r0, r0, #H_FLAG
				ldrh	r1, [r7, #CPU_BC]
				orr		r8, r0, r8
				beq		__cpdr0
				and		r2, r3, #&80
				subs	r1, r1, #1
				orr		r8, r8, r2
				strh	r1, [r7, #CPU_BC]
				orrne	r8, r8, #V_FLAG
				mov		pc, r11
__cpdr0			orr		r8, r8, #Z_FLAG
				subs	r1, r1, #1
				orrne	r8, r8, #V_FLAG
				strh	r1, [r7, #CPU_BC]
				mov		pc, r11


; ----

z80a_sb			GETPC8
				adr		r1, clktbl
				adr		r2, optbl
				ldrb	r3, [r1, r0]
				ldr		r1, [r2, r0, lsl #2]
				rsb		r10, r3, r10
				add		r6, r6, #(1 << 24)
				mov		pc, r1

clktbl			dcb	 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
				dcb	 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
				dcb	 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
				dcb	 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
				dcb	12,12,15,20, 8, 8, 8, 9,12,12,15,20, 8, 8, 8, 9
				dcb	12,12,15,20, 8, 8, 8, 9,12,12,15,20, 8, 8, 8, 9
				dcb	12,12,15,20, 8, 8, 8,18,12,12,15,20, 8, 8, 8,18
				dcb	12,12,15,20, 8, 8, 8, 0,12,12,15,20, 8, 8, 8, 0
				dcb	 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
				dcb	 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
				dcb	16,16,16,16, 0, 0, 0, 0,16,16,16,16, 0, 0, 0, 0
				dcb	16,16,16,16, 0, 0, 0, 0,16,16,16,16, 0, 0, 0, 0
				dcb	 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
				dcb	 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
				dcb	 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
				dcb	 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0

optbl	dcd		_ld_nop,		_ld_nop,		_ld_nop,		_ld_nop
		dcd		_ld_nop,		_ld_nop,		_ld_nop,		_ld_nop
		dcd		_ld_nop,		_ld_nop,		_ld_nop,		_ld_nop
		dcd		_ld_nop,		_ld_nop,		_ld_nop,		_ld_nop

		dcd		_ld_nop,		_ld_nop,		_ld_nop,		_ld_nop
		dcd		_ld_nop,		_ld_nop,		_ld_nop,		_ld_nop
		dcd		_ld_nop,		_ld_nop,		_ld_nop,		_ld_nop
		dcd		_ld_nop,		_ld_nop,		_ld_nop,		_ld_nop

		dcd		_ld_nop,		_ld_nop,		_ld_nop,		_ld_nop
		dcd		_ld_nop,		_ld_nop,		_ld_nop,		_ld_nop
		dcd		_ld_nop,		_ld_nop,		_ld_nop,		_ld_nop
		dcd		_ld_nop,		_ld_nop,		_ld_nop,		_ld_nop

		dcd		_ld_nop,		_ld_nop,		_ld_nop,		_ld_nop
		dcd		_ld_nop,		_ld_nop,		_ld_nop,		_ld_nop
		dcd		_ld_nop,		_ld_nop,		_ld_nop,		_ld_nop
		dcd		_ld_nop,		_ld_nop,		_ld_nop,		_ld_nop

		dcd		_in_b_c,		_out_c_b,		_sbc_hl_bc,		_ld_xword_bc
		dcd		_neg,			_retn,			_im_0,			_ld_i_a
		dcd		_in_c_c,		_out_c_c,		_adc_hl_bc,		_ld_bc_xword
		dcd		_neg,			_reti,			_im_0,			_ld_r_a

		dcd		_in_d_c,		_out_c_d,		_sbc_hl_de,		_ld_xword_de
		dcd		_neg,			_retn,			_im_1,			_ld_a_i
		dcd		_in_e_c,		_out_c_e,		_adc_hl_de,		_ld_de_xword
		dcd		_neg,			_reti,			_im_2,			_ld_a_r

		dcd		_in_h_c,		_out_c_h,		_sbc_hl_hl,		_ld_xword_hl
		dcd		_neg,			_retn,			_im_0,			_rrd
		dcd		_in_l_c,		_out_c_l,		_adc_hl_hl,		_ld_hl_xword
		dcd		_neg,			_reti,			_im_0,			_rld

		dcd		_in_0_c,		_out_c_0,		_sbc_hl_sp,		_ld_xword_sp
		dcd		_neg,			_retn,			_im_1,			_ld_nop
		dcd		_in_a_c,		_out_c_a,		_adc_hl_sp,		_ld_sp_xword
		dcd		_neg,			_reti,			_im_2,			_ld_nop

		dcd		_ld_nop,		_ld_nop,		_ld_nop,		_ld_nop
		dcd		_ld_nop,		_ld_nop,		_ld_nop,		_ld_nop
		dcd		_ld_nop,		_ld_nop,		_ld_nop,		_ld_nop
		dcd		_ld_nop,		_ld_nop,		_ld_nop,		_ld_nop

		dcd		_ld_nop,		_ld_nop,		_ld_nop,		_ld_nop
		dcd		_ld_nop,		_ld_nop,		_ld_nop,		_ld_nop
		dcd		_ld_nop,		_ld_nop,		_ld_nop,		_ld_nop
		dcd		_ld_nop,		_ld_nop,		_ld_nop,		_ld_nop

		dcd		_ldi,			_cpi,			_ini,			_outi
		dcd		_ld_nop,		_ld_nop,		_ld_nop,		_ld_nop
		dcd		_ldd,			_cpd,			_ind,			_outd
		dcd		_ld_nop,		_ld_nop,		_ld_nop,		_ld_nop

		dcd		_ldir,			_cpir,			_inir,			_otir
		dcd		_ld_nop,		_ld_nop,		_ld_nop,		_ld_nop
		dcd		_lddr,			_cpdr,			_indr,			_otdr
		dcd		_ld_nop,		_ld_nop,		_ld_nop,		_ld_nop

		dcd		_ld_nop,		_ld_nop,		_ld_nop,		_ld_nop
		dcd		_ld_nop,		_ld_nop,		_ld_nop,		_ld_nop
		dcd		_ld_nop,		_ld_nop,		_ld_nop,		_ld_nop
		dcd		_ld_nop,		_ld_nop,		_ld_nop,		_ld_nop

		dcd		_ld_nop,		_ld_nop,		_ld_nop,		_ld_nop
		dcd		_ld_nop,		_ld_nop,		_ld_nop,		_ld_nop
		dcd		_ld_nop,		_ld_nop,		_ld_nop,		_ld_nop
		dcd		_ld_nop,		_ld_nop,		_ld_nop,		_ld_nop

		dcd		_ld_nop,		_ld_nop,		_ld_nop,		_ld_nop
		dcd		_ld_nop,		_ld_nop,		_ld_nop,		_ld_nop
		dcd		_ld_nop,		_ld_nop,		_ld_nop,		_ld_nop
		dcd		_ld_nop,		_ld_nop,		_ld_nop,		_ld_nop

		dcd		_ld_nop,		_ld_nop,		_ld_nop,		_ld_nop
		dcd		_ld_nop,		_ld_nop,		_ld_nop,		_ld_nop
		dcd		_ld_nop,		_ld_nop,		_ld_nop,		_ld_nop
		dcd		_ld_nop,		_ld_nop,		_ld_nop,		_ld_nop

	END

