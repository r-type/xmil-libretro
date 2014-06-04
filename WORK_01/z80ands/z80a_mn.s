
	.include	"./nds/ramptr.inc"
	.include	"../z80ands/z80a.inc"
	.include	"../z80ands/z80a_mn.inc"
	.include	"../z80ands/z80dma.inc"

	.global	z80a_nonmaskedinterrupt
	.global	z80a_interrupt
	.global	z80a_execute
	.global	z80a_step

	.extern	z80core
	.extern	z80a_cb
	.extern	z80a_sb
	.extern	z80a_ix
	.extern	z80a_iy
	.extern	z80dmap2
	.extern	_mem_read16
	.extern	_mr16_oddr12
	.extern _mem_write16_ret
	.extern _mem_write16
	.extern	_mw16_odd
	.extern	dma
	.extern	ievent_eoi

	.extern	_out_byte_a
	.extern	_in_a_byte

	.global	_reti
	.global	_retn
	.global	_add_r3_16
	.global	_add_r3_r3
	.global	_add_r3_sp
	.global	_ld_xword_hl4
	.global	_ld_xword_r3
	.global	_ld_hl_xword4
	.global	_ld_r3_xword
	.global	_ld_xr2_byte
	.global	_add_a_xr2
	.global	_adc_a_xr2
	.global	_sub_xr2
	.global	_sbc_a_xr2
	.global	_and_xr2
	.global	_xor_xr2
	.global	_or_xr2
	.global	_cp_xr2
	.global	_inc_xr2
	.global	_dec_xr2
	.global	_push_r3
	.global	_pop_r3


; @ ---- iwram thumb

	.section	.itcm
	.code	16
	.align	1

_ld_b_c:		sLD8	#CPU_B, #CPU_C
_ld_b_d:		sLD8	#CPU_B, #CPU_D
_ld_b_e:		sLD8	#CPU_B, #CPU_E
_ld_b_h:		sLD8	#CPU_B, #CPU_H
_ld_b_l:		sLD8	#CPU_B, #CPU_L

_ld_c_b:		sLD8	#CPU_C, #CPU_B
_ld_c_d:		sLD8	#CPU_C, #CPU_D
_ld_c_e:		sLD8	#CPU_C, #CPU_E
_ld_c_h:		sLD8	#CPU_C, #CPU_H
_ld_c_l:		sLD8	#CPU_C, #CPU_L

_ld_d_b:		sLD8	#CPU_D, #CPU_B
_ld_d_c:		sLD8	#CPU_D, #CPU_C
_ld_d_e:		sLD8	#CPU_D, #CPU_E
_ld_d_h:		sLD8	#CPU_D, #CPU_H
_ld_d_l:		sLD8	#CPU_D, #CPU_L

_ld_e_b:		sLD8	#CPU_E, #CPU_B
_ld_e_c:		sLD8	#CPU_E, #CPU_C
_ld_e_d:		sLD8	#CPU_E, #CPU_D
_ld_e_h:		sLD8	#CPU_E, #CPU_H
_ld_e_l:		sLD8	#CPU_E, #CPU_L

_ld_h_b:		sLD8	#CPU_H, #CPU_B
_ld_h_c:		sLD8	#CPU_H, #CPU_C
_ld_h_d:		sLD8	#CPU_H, #CPU_D
_ld_h_e:		sLD8	#CPU_H, #CPU_E
_ld_h_l:		sLD8	#CPU_H, #CPU_L

_ld_l_b:		sLD8	#CPU_L, #CPU_B
_ld_l_c:		sLD8	#CPU_L, #CPU_C
_ld_l_d:		sLD8	#CPU_L, #CPU_D
_ld_l_e:		sLD8	#CPU_L, #CPU_E
_ld_l_h:		sLD8	#CPU_L, #CPU_H

_ld_b_a:		sLD8A	#CPU_B
_ld_c_a:		sLD8A	#CPU_C
_ld_d_a:		sLD8A	#CPU_D
_ld_e_a:		sLD8A	#CPU_E
_ld_h_a:		sLD8A	#CPU_H
_ld_l_a:		sLD8A	#CPU_L

_ld_xbc_a:		Z80WORK	#3
				sLDxA	#CPU_BC
_ld_xde_a:		Z80WORK	#3
				sLDxA	#CPU_DE
_ld_xhl_a:		Z80WORK	#3
				sLDxA	#CPU_HL

_ld_xhl_b:		Z80WORK	#3
				sLDx8	#CPU_HL, #CPU_B
_ld_xhl_c:		Z80WORK	#3
				sLDx8	#CPU_HL, #CPU_C
_ld_xhl_d:		Z80WORK	#3
				sLDx8	#CPU_HL, #CPU_D
_ld_xhl_e:		Z80WORK	#3
				sLDx8	#CPU_HL, #CPU_E
_ld_xhl_h:		Z80WORK	#3
				sLDx8	#CPU_HL, #CPU_H
_ld_xhl_l:		Z80WORK	#3
				sLDx8	#CPU_HL, #CPU_L


_inc_bc:		Z80WORK	#2
				sMINC16	#CPU_BC
_inc_de:		Z80WORK	#2
				sMINC16	#CPU_DE
_inc_hl:		Z80WORK	#2
				sMINC16	#CPU_HL

_dec_bc:		Z80WORK	#2
				sMDEC16	#CPU_BC
_dec_de:		Z80WORK	#2
				sMDEC16	#CPU_DE
_dec_hl:		Z80WORK	#2
				sMDEC16	#CPU_HL


_ex_af_af:		ldrh	r0, [r4, #CPU_AF2]
				strh	rAF, [r4, #CPU_AF2]
				mov		rAF, r0
				bx		r11

_ex_de_hl:		ldrh	r0, [r4, #CPU_DE]
				ldrh	r1, [r4, #CPU_HL]
				strh	r0, [r4, #CPU_HL]
				strh	r1, [r4, #CPU_DE]
				bx		r11

_exx:			ldr		r0, [r4, #CPU_BC]
				ldrh	r1, [r4, #CPU_HL]
				ldr		r2, [r4, #CPU_BC2]
				ldrh	r3, [r4, #CPU_HL2]
				str		r2, [r4, #CPU_BC]
				strh	r3, [r4, #CPU_HL]
				str		r0, [r4, #CPU_BC2]
				strh	r1, [r4, #CPU_HL2]
				bx		r11


; @ ---- iwram arm

	.code	32
	.align	2

_jr_nz:			tst		rAF, #Z_FLAG			; @ ふつーループする
				bne		__jrf_skip
_jrf:
_jr0:			Z80WORK	#8
				GETPC8S
				add		r8, r8, r0, lsl #16
				mov		pc, r11
_jr_z:			tst		rAF, #Z_FLAG
				bne		_jrf
__jrf_skip:		Z80WORK	#3
				add		r8, r8, #(1 << 16)
				mov		pc, r11

_jr_nc:			tst		rAF, #C_FLAG			; @ キャリーはどっち？
				beq		_jrf
				Z80WORK	#3
				add		r8, r8, #(1 << 16)
				mov		pc, r11

_jr_c:			tst		rAF, #C_FLAG
				bne		_jrf
				Z80WORK	#3
				add		r8, r8, #(1 << 16)
_ld_nop:		mov		pc, r11

_djnz:			ldrb	r0, [r4, #CPU_B]
				Z80WORK	#1
				subs	r0, r0, #1
				strb	r0, [r4, #CPU_B]
				bne		_jrf
				Z80WORK	#3
				add		r8, r8, #(1 << 16)
				mov		pc, r11


_reti:			Z80WORK	#10
				str		rREMCLOCK, [r4, #_CPU_REMCLOCK]
				mov		lr, pc
				ldr		pc, 800f
				ldr		rREMCLOCK, [r4, #_CPU_REMCLOCK]
				b		_ret0
800:			.word	ievent_eoi

_retn:			Z80WORK	#10
				bic		rAF, rAF, #(1 << IFF_NMI)
				b		_ret0

_ret_nz:		MRETNF	#Z_FLAG
_ret_nc:		MRETNF	#C_FLAG
_ret_po:		MRETNF	#V_FLAG
_ret_p:			MRETNF	#S_FLAG
_ret_z:			MRETF	#Z_FLAG
_ret_c:			MRETF	#C_FLAG
_ret_pe:		MRETF	#V_FLAG
_ret_m:			MRETF	#S_FLAG

			; @ spはほぼ偶数の筈
_ret:			Z80WORK	#6						; @ これ 12?
_ret0:			movs	r3, r8, lsl #16			; @ sp
				movmi	r12, r5
				movpl	r12, r10
				mov		r0, r3, lsr #16
				tst		r8, #1
				ldreqh	r0, [r0, r12]
				blne	_mr16_oddr12
				add		r3, r3, #(2 << 16)
				mov		r0, r0, lsl #16
				orr		r8, r0, r3, lsr #16
				sub		r8, r8, #(1 << 16)
				mov		pc, r11


; @ ---- alu

	.global	_inc_r
	.global	_dec_r
	.global	_add_a_r
	.global	_adc_a_r
	.global	_sub_r
	.global	_sbc_a_r
	.global	_and_r
	.global	_xor_r
	.global	_or_r
	.global	_cp_r

_inc_r:			mov		r3, r0, lsr #3
				eor		r3, r3, #1
				MINC8	r3

_dec_r:			mov		r3, r0, lsr #3
				eor		r3, r3, #1
				MDEC8	r3


_add_a_r:		and		r3, r0, #7
				eor		r3, r3, #1
				ldrb	r12, [r3, r4]
				MADD8	r12

_adc_a_r:		and		r3, r0, #7
				eor		r3, r3, #1
				ldrb	r12, [r3, r4]
				MADC8	r12

_sub_r:			and		r3, r0, #7
				eor		r3, r3, #1
				ldrb	r12, [r3, r4]
				MSUB8	r12

_sbc_a_r:		and		r3, r0, #7
				eor		r3, r3, #1
				ldrb	r12, [r3, r4]
				MSBC8	r12

_and_r:			and		r3, r0, #7
				eor		r3, r3, #1
				ldrb	r12, [r3, r4]
				MAND8	r12

_xor_r:			and		r3, r0, #7
				eor		r3, r3, #1
				ldrb	r12, [r3, r4]
				MXOR8	r12

_or_r:			and		r3, r0, #7
				eor		r3, r3, #1
				ldrb	r12, [r3, r4]
				MOR8	r12

_cp_r:			and		r3, r0, #7
				eor		r3, r3, #1
				ldrb	r12, [r3, r4]
				MCP8	r12


; @ ----

_rst:			Z80WORK	#7
				add		r8, r8, #(1 << 16)
				sub		r2, r0, #0xc8
				mov		r0, r8, lsl #16
				mov		r1, r8, lsr #16
				sub		r0, r0, #(2 << 16)
				mov		r0, r0, lsr #16
				orr		r8, r0, r2, lsl #16
				b		_mem_write16_ret


; @ ----


_ld_bc_word:	Z80WORK	#6
				LD16w	#CPU_BC
_ld_de_word:	Z80WORK	#6
				LD16w	#CPU_DE
_ld_hl_word:	Z80WORK	#6
				LD16w	#CPU_HL

_inc_xhl:		Z80WORK	#7
				ldrh	r2, [r4, #CPU_HL]
_inc_xr2:		MINCX8	r2

_dec_xhl:		Z80WORK	#7
				ldrh	r2, [r4, #CPU_HL]
_dec_xr2:		MDECX8	r2

_ld_b_byte:		Z80WORK	#3
				LD8b	#CPU_B
_ld_c_byte:		Z80WORK	#3
				LD8b	#CPU_C
_ld_d_byte:		Z80WORK	#3
				LD8b	#CPU_D
_ld_e_byte:		Z80WORK	#3
				LD8b	#CPU_E
_ld_h_byte:		Z80WORK	#3
				LD8b	#CPU_H
_ld_l_byte:		Z80WORK	#3
				LD8b	#CPU_L


_add_hl_16:		mov		r3, #CPU_HL
_add_r3_16:		Z80WORK	#7
				mov		r2, r0, lsr #3			; @ 00??1001
				and		r1, r2, #6
				MADD16	r3, r1

_add_hl_hl:		mov		r3, #CPU_HL
_add_r3_r3:		Z80WORK	#7
				MADD16D	r3

_add_hl_sp:		mov		r3, #CPU_HL
_add_r3_sp:		Z80WORK	#7
				MADDSP	r3


_ld_xword_hl4:	Z80WORK	#4
_ld_xword_hl:	mov		r3, #CPU_HL
_ld_xword_r3:	Z80WORK	#12
				LDx16	r3

_ld_hl_xword4:	Z80WORK	#4
_ld_hl_xword:	mov		r3, #CPU_HL
_ld_r3_xword:	Z80WORK	#12
				LD16x	r3


_ld_b_xhl:		Z80WORK	#3
				LD8x	#CPU_B, #CPU_HL
_ld_c_xhl:		Z80WORK	#3
				LD8x	#CPU_C, #CPU_HL
_ld_d_xhl:		Z80WORK	#3
				LD8x	#CPU_D, #CPU_HL
_ld_e_xhl:		Z80WORK	#3
				LD8x	#CPU_E, #CPU_HL
_ld_h_xhl:		Z80WORK	#3
				LD8x	#CPU_H, #CPU_HL
_ld_l_xhl:		Z80WORK	#3
				LD8x	#CPU_L, #CPU_HL

_ld_a_b:		LDA8	#CPU_B
_ld_a_c:		LDA8	#CPU_C
_ld_a_d:		LDA8	#CPU_D
_ld_a_e:		LDA8	#CPU_E
_ld_a_h:		LDA8	#CPU_H
_ld_a_l:		LDA8	#CPU_L

_ld_a_xbc:		Z80WORK	#3
				LDAx	#CPU_BC
_ld_a_xde:		Z80WORK	#3
				LDAx	#CPU_DE
_ld_a_xhl:		Z80WORK	#3
				LDAx	#CPU_HL


_add_a_xhl:		Z80WORK	#3
				ldrh	r2, [r4, #CPU_HL]
_add_a_xr2:		MEMRD8	r12, r2
				MADD8	r12

_adc_a_xhl:		Z80WORK	#3
				ldrh	r2, [r4, #CPU_HL]
_adc_a_xr2:		MEMRD8	r12, r2
				MADC8	r12

_sub_xhl:		Z80WORK	#3
				ldrh	r2, [r4, #CPU_HL]
_sub_xr2:		MEMRD8	r12, r2
				MSUB8	r12

_sbc_a_xhl:		Z80WORK	#3
				ldrh	r2, [r4, #CPU_HL]
_sbc_a_xr2:		MEMRD8	r12, r2
				MSBC8	r12

_and_xhl:		Z80WORK	#3
				ldrh	r2, [r4, #CPU_HL]
_and_xr2:		MEMRD8	r12, r2
				MAND8	r12

_xor_xhl:		Z80WORK	#3
				ldrh	r2, [r4, #CPU_HL]
_xor_xr2:		MEMRD8	r12, r2
				MXOR8	r12

_or_xhl:		Z80WORK	#3
				ldrh	r2, [r4, #CPU_HL]
_or_xr2:		MEMRD8	r12, r2
				MOR8	r12

_cp_xhl:		Z80WORK	#3
				ldrh	r2, [r4, #CPU_HL]
_cp_xr2:		MEMRD8	r12, r2
				MCP8	r12


_push_af:		Z80WORK	#7
				mov		r1, rAF
				MPUSHr1

_push_16:		mov		r3, r0, lsr #3			; @ 11??0101
				eor		r3, r3, #0x18			; @ 11??0
_push_r3:		Z80WORK	#7
				ldrh	r1, [r3, r4]
				MPUSHr1


_pop_af:		Z80WORK	#6
				MPOPr0
				mov		rAF, r0
				mov		pc, r11

_pop_16:		mov		r3, r0, lsr #3			; @ 11??0001
				eor		r3, r3, #0x18			; @ 11??0
_pop_r3:		Z80WORK	#6
				MPOPr0
				strh	r0, [r3, r4]
				mov		pc, r11


; @ --- PCレジスタ

_jp_hl:			MJPR16	#CPU_HL
_jp_nz:			MJPNFLG	#Z_FLAG
_jp_nc:			MJPNFLG	#C_FLAG
_jp_po:			MJPNFLG	#V_FLAG
_jp_p:			MJPNFLG	#S_FLAG
_jp_z:			MJPFLG	#Z_FLAG
_jp_c:			MJPFLG	#C_FLAG
_jp_pe:			MJPFLG	#V_FLAG
_jp_m:			MJPFLG	#S_FLAG
_jp0:			Z80WORK	#6
_jp:			MJP

_call_nz:		MCALLNF	#Z_FLAG
_call_nc:		MCALLNF	#C_FLAG
_call_po:		MCALLNF	#V_FLAG
_call_p:		MCALLNF	#S_FLAG
_call_z:		MCALLF	#Z_FLAG
_call_c:		MCALLF	#C_FLAG
_call_pe:		MCALLF	#V_FLAG
_call_m:		MCALLF	#S_FLAG
_call:			Z80WORK	#13
				MCALL


; @ --- HLレジスタ

_ld_xhl_byte:	Z80WORK	#6
				ldrh	r2, [r4, #CPU_HL]
_ld_xr2_byte:	GETPC8
				MEMWR8	r2, r0
				mov		pc, r11


; @ --- Aレジスタ

_ld_xbyte_a:	Z80WORK	#9
				GETPC16
				mov		r1, rAF, lsr #8
				MEMWR8	r0, r1
				mov		pc, r11

_ld_a_xbyte:	Z80WORK	#9
				GETPC16
				MEMRD8	r1, r0
				and		r2, rAF, #0xff
				orr		rAF, r2, r1, lsl #8
				mov		pc, r11

_inc_a:			bic		r1, rAF, #(0xff - C_FLAG)
				add		r0, r4, #CPU_INCFLAG
				add		r1, r1, #(1 << 8)
				ldrb	r2, [r0, rAF, lsr #8]
				bic		r1, r1, #(1 << 16)
				orr		rAF, r1, r2
				mov		pc, r11

_dec_a:			bic		r1, rAF, #(0xff - C_FLAG)
				add		r0, r4, #CPU_DECFLAG
				subs	r1, r1, #(1 << 8)
				ldrb	r2, [r0, rAF, lsr #8]
				addmi	r1, r1, #(1 << 16)
				orr		rAF, r1, r2
				mov		pc, r11

_ld_a_byte:		Z80WORK	#3
				GETPC8
				and		rAF, rAF, #0xff
				orr		rAF, rAF, r0, lsl #8
				mov		pc, r11


_add_a_a:		and		r0, rAF, #(0xff << 8)
				and		r1, rAF, #(H_FLAG << 7)
				mov		r2, r0, lsl #16
				adds	r3, r2, r0, lsl #16
				orrcs	r3, r3, #(C_FLAG << 16)
				orrvs	r3, r3, #(V_FLAG << 16)
				orreq	r3, r3, #(Z_FLAG << 16)
				orrmi	r3, r3, #(S_FLAG << 16)
				mov		rAF, r3, lsr #16
				orr		rAF, rAF, r1, lsr #7
				mov		pc, r11

_adc_a_a:		tst		rAF, #C_FLAG
				and		r0, rAF, #(0xff << 8)
				orrne	r0, r0, #0x80
				and		r1, rAF, #(H_FLAG << 7)
				mov		r2, r0, lsl #16
				adds	r3, r2, r0, lsl #16
				orrcs	r3, r3, #(C_FLAG << 16)
				orrvs	r3, r3, #(V_FLAG << 16)
				orreq	r3, r3, #(Z_FLAG << 16)
				orrmi	r3, r3, #(S_FLAG << 16)
				mov		rAF, r3, lsr #16
				orr		rAF, rAF, r1, lsr #7
				mov		pc, r11

_sub_a:			mov		rAF, #(Z_FLAG + N_FLAG)
				mov		pc, r11

_sbc_a_a:		tst		rAF, #C_FLAG
				moveq	rAF, #(Z_FLAG + N_FLAG)
				movne	rAF, #(0xff << 8)
				orrne	rAF, rAF, #(S_FLAG + H_FLAG + N_FLAG + C_FLAG)
				mov		pc, r11

_and_a:			;
_or_a:			add		r1, r4, #CPU_SZPFLAG
				and		r2, rAF, #(0xff << 8)
				ldrb	r0, [r1, rAF, lsr #8]
				orr		rAF, r0, r2
				mov		pc, r11

_xor_a:			mov		rAF, #(Z_FLAG + V_FLAG)
				mov		pc, r11

_cp_a:			and		r0, rAF, #(0xff << 8)
				orr		rAF, r0, #(Z_FLAG + N_FLAG)
				mov		pc, r11

_add_a_byte:	Z80WORK	#3
				GETPC8
				MADD8	r0
_adc_a_byte:	Z80WORK	#3
				GETPC8
				MADC8	r0
_sub_byte:		Z80WORK	#3
				GETPC8
				MSUB8	r0
_sbc_a_byte:	Z80WORK	#3
				GETPC8
				MSBC8	r0
_and_byte:		Z80WORK	#3
				GETPC8
				MAND8	r0
_xor_byte:		Z80WORK	#3
				GETPC8
				MXOR8	r0
_or_byte:		Z80WORK	#3
				GETPC8
				MOR8	r0
_cp_byte:		Z80WORK	#3
				GETPC8
				MCP8	r0


; @ --- SPレジスタ

_ex_xsp_hl:		Z80WORK	#15
				MEXSP	#CPU_HL

_ld_sp_hl:		Z80WORK	#2
				MLDSP	#CPU_HL

_ld_sp_word:	Z80WORK	#6
				GETPC16
				mov		r1, r8, lsr #16
				orr		r8, r0, r1, lsl #16
				mov		pc, r11

_inc_sp:		Z80WORK	#2
				add		r8, r8, #1
				movs	r0, r8, lsl #16
				subeq	r8, r8, #(1 << 16)
				mov		pc, r11

_dec_sp:		Z80WORK	#2
				movs	r0, r8, lsl #16
				sub		r8, r8, #1
				addeq	r8, r8, #(1 << 16)
				mov		pc, r11


; @ ---- AFレジスタ

_rlca:			and		r0, rAF, #(0x7f << 8)
				and		r1, rAF, #0xec
				tst		rAF, #(0x80 << 8)
				add		rAF, r1, r0, lsl #1
				addne	rAF, rAF, #C_FLAG
				addne	rAF, rAF, #(1 << 8)
				mov		pc, r11

_rrca:			movs	r0, rAF, lsr #9			; @ a
				and		r1, rAF, #0xec			; @ f
				add		rAF, r1, r0, lsl #8
				addcs	rAF, rAF, #C_FLAG
				addcs	rAF, rAF, #(1 << 15)
				mov		pc, r11

_rla:			and		r0, rAF, #(0x7f << 8)
				and		r1, rAF, #0xec
				tst		rAF, #C_FLAG
				addne	r1, r1, #(1 << 8)
				tst		rAF, #(0x80 << 8)
				addne	r1, r1, #C_FLAG
				add		rAF, r1, r0, lsl #1
				mov		pc, r11

_rra:			and		r0, rAF, #0xec
				movs	r1, rAF, lsr #9
				addcs	r0, r0, #C_FLAG
				tst		rAF, #C_FLAG
				addne	r0, r0, #(0x80 << 8)
				add		rAF, r0, r1, lsl #8
				mov		pc, r11


_daa:			and		r0, rAF, #(0xff << 8)	; @ dst
				and		r1, rAF, #(0x0f << 8)	; @ alow
				tst		rAF, #N_FLAG
				bne		1000f
				and		r2, rAF, #(N_FLAG + C_FLAG)
				cmp		r1, #(0x09 << 8)
				orrhi	r2, r2, #H_FLAG			; @ -> ne..
				tstls	rAF, #H_FLAG
				addne	r0, r0, #(0x06 << 8)
				cmp		r0, #(0xa0 << 8)
				orrcs	r2, r2, #C_FLAG
				tst		r2, #C_FLAG
				addne	r0, r0, #(0x60 << 8)
				b		1002f
1000:			and		r2, rAF, #(N_FLAG + C_FLAG)
				cmp		r0, #(0x9a << 8)
				orrcs	r2, r2, #C_FLAG
				tst		r2, #C_FLAG
				subne	r0, r0, #(0x60 << 8)
				tst		rAF, #H_FLAG
				bne		1001f
				cmp		r1, #(0x0a << 8)
				bcc		1002f
				subs	r0, r0, #(0x06 << 8)
				orrmi	r2, r2, #C_FLAG
				b		1002f
1001:			cmp		r1, #(0x06 << 8)
				orrcc	r2, r2, #H_FLAG
				sub		r0, r0, #(0x06 << 8)
1002:			and		r1, r0, #(0xff << 8)
				add		r3, r4, #CPU_SZPFLAG
				add		rAF, r1, r2
				ldrb	r0, [r3, r1, lsr #8]
				add		rAF, r0, rAF
				mov		pc, r11

_cpl:			eor		rAF, rAF, #(0xff << 8)
				orr		rAF, rAF, #(H_FLAG + N_FLAG)
				mov		pc, r11

_scf:			bic		rAF, rAF, #(H_FLAG + N_FLAG)
				orr		rAF, rAF, #C_FLAG
				mov		pc, r11

_ccf:			bic		r1, rAF, #(H_FLAG + N_FLAG)
				tst		rAF, #C_FLAG
				orrne	r1, r1, #H_FLAG
				eor		rAF, r1, #C_FLAG
				mov		pc, r11


; @ ---- システム

_di:			tst		r9, #(1 << IFF_IFLAG)
				orreq	r9, r9, #(1 << IFF_IFLAG)
				streqb	r9, [r4, #_CPU_IFF]
				mov		pc, r11

_ei:			tst		r9, #(1 << IFF_IFLAG)
				moveq	pc, r11
				bic		r9, r9, #(1 << IFF_IFLAG)
				subs	r1, rREMCLOCK, #1
				strb	r9, [r4, #_CPU_IFF]
				movle	pc, r11
				tst		r9, #(1 << IFF_NMI)
				ldr		r2, [r4, #CPU_REQIRQ]
				movne	pc, r11
				cmp		r2, #0
				moveq	pc, r11
				ldr		r0, [r4, #CPU_BASECLOCK]
				mov		rREMCLOCK, #1
				sub		r0, r0, r1
				str		r0, [r4, #CPU_BASECLOCK]
				mov		pc, r11

_halt:			sub		r8, r8, #(1 << 16)
				orr		r9, r9, #(1 << IFF_HALT)
				mov		rREMCLOCK, #0
				mov		pc, r11

	.pool

; @ ----

_MAINMEM:	.word	mainmem

z80a_nonmaskedinterrupt:
				stmdb	sp!, {r4 - r11, lr}
				mov		r4, #Z80CORE
			@	ldr		rREMCLOCK, [r4, #_CPU_REMCLOCK]
			@	ldrh	rAF, [r4, #_CPU_AF]
				ldr		r8, [r4, #_CPU_SP]
				ldr		r9, [r4, #_CPU_IFF]
				ldr		r10, [r4, #_CPU_MEMREAD]
				ldr		r5, _MAINMEM
				tst		r9, #(1 << IFF_NMI)
				bne		100f
				mov		r0, r8, lsl #16
				tst		r9, #(1 << IFF_HALT)
				addne	r8, r8, #0x10000
				bicne	r9, r9, #(1 << IFF_HALT)
				sub		r2, r0, #0x20000			@ stack!
				orr		r9, r9, #(1 << IFF_NMI)
				mov		r0, r2, lsr #16
				mov		r1, r8, lsr #16
				orr		r8, r0, #0x660000
				bl		_mem_write16
			@	str		rREMCLOCK, [r4, #_CPU_REMCLOCK]
			@	strh	rAF, [r4, #_CPU_AF]
				str		r8, [r4, #_CPU_SP]
				str		r9, [r4, #_CPU_IFF]
100:
.if INTERWORK
				ldmia	sp!, {r4 - r11, lr}
				bx		lr
.else
				ldmia	sp!, {r4 - r11, pc}
.endif

	.pool

z80a_interrupt:
				stmdb	sp!, {r4 - r11, lr}
				mov		r4, #Z80CORE
				add		r2, r4, #_CPU_REMCLOCK
				ldmia	r2, {r6 - r10}
				ldr		r5, _MAINMEM

				tst		r9, #(1 << IFF_HALT)
				orr		r9, r9, #(1 << IFF_IFLAG)
				bicne	r9, r9, #(1 << IFF_HALT)
				addne	r8, r8, #0x10000
				tst		r9, #0x30000
				beq		200f

				mov		r2, r8, lsl #16		@ stack
				mov		r1, r8, lsr #16		@ pc
				sub		r2, r2, #0x20000
				tst		r9, #0x20000
				bne		202f

201:			mov		r0, r2, lsr #16
				Z80WORK	#(11 - 4)
				orr		r8, r0, #0x380000
				adr		lr, 203f
				b		_mem_write16

202:			and		r3, r9, #0xff00
				mov		r8, r2, lsr #16
				orr		r3, r3, r0
				mov		r0, r2, lsr #16
				bl		_mem_write16
				mov		r0, r3
				bl		_mem_read16
				orr		r8, r8, r0, lsl #16

203:			add		r2, r4, #_CPU_REMCLOCK
				stmia	r2, {r6 - r9}
.if INTERWORK
				ldmia	sp!, {r4 - r11, lr}
				bx		lr
.else
				ldmia	sp!, {r4 - r11, pc}
.endif

200:			adr		r1, 400f
				adr		r11, 203b
				ldr		r2, [r1, r0, lsl #2]
				bx		r2

	.pool

301:			sub		rREMCLOCK, rREMCLOCK, #4
				GETPC8
				adr		r1, 400f
				add		r9, r9, #(1 << 24)
				ldr		r2, [r1, r0, lsl #2]
				mov		r11, pc
				bx		r2
				bl		z80dmap2
				cmp		rREMCLOCK, #0
				bgt		301b

302:			add		r8, r8, #(1 << 16)
				add		r2, r4, #_CPU_REMCLOCK
				stmia	r2, {r6 - r9}
.if INTERWORK
				ldmia	sp!, {r4 - r11, lr}
				bx		lr
.else
				ldmia	sp!, {r4 - r11, pc}
.endif

	.pool

z80a_execute:	stmdb	sp!, {r4 - r11, lr}
				mov		r4, #Z80CORE
				add		r2, r4, #_CPU_REMCLOCK
				ldr		r1, [r4, #(Z80CORE2DMA + DMA_FLAG)]
				ldmia	r2, {r6 - r10}
				sub		r8, r8, #(1 << 16)
				ldr		r5, _MAINMEM
				tst		r1, #DMAF_WORKING
				bne		301b

				add		rREMCLOCK, rREMCLOCK, #4
				adr		r11, 300f
300:			subs	rREMCLOCK, rREMCLOCK, #4	@ スタークルーザーの
				ble		302b						@ タイミングがちと狂う…
				GETPC8
				add		r9, r9, #(1 << 24)
				ldr		r12, [pc, r0, lsl #2]
				bx		r12

400:	.word	_ld_nop,		_ld_bc_word,	_ld_xbc_a+1,	_inc_bc+1
		.word	_inc_r,			_dec_r,			_ld_b_byte,		_rlca
		.word	_ex_af_af+1,	_add_hl_16,		_ld_a_xbc,		_dec_bc+1
		.word	_inc_r,			_dec_r,			_ld_c_byte,		_rrca

		.word	_djnz,			_ld_de_word,	_ld_xde_a+1,	_inc_de+1
		.word	_inc_r,			_dec_r,			_ld_d_byte,		_rla
		.word	_jr0,			_add_hl_16,		_ld_a_xde,		_dec_de+1
		.word	_inc_r,			_dec_r,			_ld_e_byte,		_rra

		.word	_jr_nz,			_ld_hl_word,	_ld_xword_hl,	_inc_hl+1
		.word	_inc_r,			_dec_r,			_ld_h_byte,		_daa
		.word	_jr_z,			_add_hl_hl,		_ld_hl_xword,	_dec_hl+1
		.word	_inc_r,			_dec_r,			_ld_l_byte,		_cpl

		.word	_jr_nc,			_ld_sp_word,	_ld_xbyte_a,	_inc_sp
		.word	_inc_xhl,		_dec_xhl,		_ld_xhl_byte,	_scf
		.word	_jr_c,			_add_hl_sp,		_ld_a_xbyte,	_dec_sp
		.word	_inc_a,			_dec_a,			_ld_a_byte,		_ccf

		.word	_ld_nop,		_ld_b_c+1,		_ld_b_d+1,		_ld_b_e+1
		.word	_ld_b_h+1,		_ld_b_l+1,		_ld_b_xhl,		_ld_b_a+1
		.word	_ld_c_b+1,		_ld_nop,		_ld_c_d+1,		_ld_c_e+1
		.word	_ld_c_h+1,		_ld_c_l+1,		_ld_c_xhl,		_ld_c_a+1

		.word	_ld_d_b+1,		_ld_d_c+1,		_ld_nop,		_ld_d_e+1
		.word	_ld_d_h+1,		_ld_d_l+1,		_ld_d_xhl,		_ld_d_a+1
		.word	_ld_e_b+1,		_ld_e_c+1,		_ld_e_d+1,		_ld_nop
		.word	_ld_e_h+1,		_ld_e_l+1,		_ld_e_xhl,		_ld_e_a+1

		.word	_ld_h_b+1,		_ld_h_c+1,		_ld_h_d+1,		_ld_h_e+1
		.word	_ld_nop,		_ld_h_l+1,		_ld_h_xhl,		_ld_h_a+1
		.word	_ld_l_b+1,		_ld_l_c+1,		_ld_l_d+1,		_ld_l_e+1
		.word	_ld_l_h+1,		_ld_nop,		_ld_l_xhl,		_ld_l_a+1

		.word	_ld_xhl_b+1,	_ld_xhl_c+1,	_ld_xhl_d+1,	_ld_xhl_e+1
		.word	_ld_xhl_h+1,	_ld_xhl_l+1,	_halt,			_ld_xhl_a+1
		.word	_ld_a_b,		_ld_a_c,		_ld_a_d,		_ld_a_e
		.word	_ld_a_h,		_ld_a_l,		_ld_a_xhl,		_ld_nop

		.word	_add_a_r,		_add_a_r,		_add_a_r,		_add_a_r
		.word	_add_a_r,		_add_a_r,		_add_a_xhl,		_add_a_a
		.word	_adc_a_r,		_adc_a_r,		_adc_a_r,		_adc_a_r
		.word	_adc_a_r,		_adc_a_r,		_adc_a_xhl,		_adc_a_a

		.word	_sub_r,			_sub_r,			_sub_r,			_sub_r
		.word	_sub_r,			_sub_r,			_sub_xhl,		_sub_a
		.word	_sbc_a_r,		_sbc_a_r,		_sbc_a_r,		_sbc_a_r
		.word	_sbc_a_r,		_sbc_a_r,		_sbc_a_xhl,		_sbc_a_a

		.word	_and_r,			_and_r,			_and_r,			_and_r
		.word	_and_r,			_and_r,			_and_xhl,		_and_a
		.word	_xor_r,			_xor_r,			_xor_r,			_xor_r
		.word	_xor_r,			_xor_r,			_xor_xhl,		_xor_a

		.word	_or_r,			_or_r,			_or_r,			_or_r
		.word	_or_r,			_or_r,			_or_xhl,		_or_a
		.word	_cp_r,			_cp_r,			_cp_r,			_cp_r
		.word	_cp_r,			_cp_r,			_cp_xhl,		_cp_a

		.word	_ret_nz,		_pop_16,		_jp_nz,			_jp0
		.word	_call_nz,		_push_16,		_add_a_byte,	_rst
		.word	_ret_z,			_ret,			_jp_z,			z80a_cb
		.word	_call_z,		_call,			_adc_a_byte,	_rst

		.word	_ret_nc,		_pop_16,		_jp_nc,			_out_byte_a
		.word	_call_nc,		_push_16,		_sub_byte,		_rst
		.word	_ret_c,			_exx+1,			_jp_c,			_in_a_byte
		.word	_call_c,		z80a_ix,		_sbc_a_byte,	_rst

		.word	_ret_po,		_pop_16,		_jp_po,			_ex_xsp_hl
		.word	_call_po,		_push_16,		_and_byte,		_rst
		.word	_ret_pe,		_jp_hl,			_jp_pe,			_ex_de_hl+1
		.word	_call_pe,		z80a_sb,		_xor_byte,		_rst

		.word	_ret_p,			_pop_af,		_jp_p,			_di
		.word	_call_p,		_push_af,		_or_byte,		_rst
		.word	_ret_m,			_ld_sp_hl,		_jp_m,			_ei
		.word	_call_m,		z80a_iy,		_cp_byte,		_rst


@ --- debug

	.section	.text
	.code		32
	.align		2

z80a_step:		stmdb	sp!, {r4 - r11, lr}
				mov		r4, #Z80CORE
				add		r2, r4, #_CPU_REMCLOCK
				ldmia	r2, {r6 - r10}
				sub		r8, r8, #(1 << 16)
				ldr		r5, _mainmem_2
				sub		rREMCLOCK, rREMCLOCK, #4
				GETPC8
				ldr		r11, _opcodes_2
				add		r9, r9, #(1 << 24)
				ldr		r12, [r11, r0, lsl #2]
				mov		r11, pc
				blx		r12
				add		r8, r8, #(1 << 16)
				add		r2, r4, #_CPU_REMCLOCK
				stmia	r2, {r6 - r9}
.if INTERWORK
				ldmia	sp!, {r4 - r11, lr}
				bx		lr
.else
				ldmia	sp!, {r4 - r11, pc}
.endif

_mainmem_2:		.word	mainmem
_opcodes_2:		.word	400b

	.end

