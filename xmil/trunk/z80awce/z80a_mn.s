
	INCLUDE	z80a.inc
	INCLUDE	z80a_mn.inc

	EXPORT	z80a_nonmaskedinterrupt
	EXPORT	z80a_interrupt
	EXPORT	z80a_execute
	EXPORT	z80a_step

	IMPORT	z80core
	IMPORT	z80a_cb
	IMPORT	z80a_sb
	IMPORT	z80a_ix
	IMPORT	z80a_iy
	IMPORT	z80dmap
	IMPORT	mem_read16
	IMPORT	mem_write16
	IMPORT	iocore_out
	IMPORT	iocore_inp
	IMPORT	dma

	AREA	.text, CODE, READONLY


_ld_nop			mov		pc, r11

_ld_bc_word		LD16w	CPU_BC
_ld_de_word		LD16w	CPU_DE
_ld_hl_word		LD16w	CPU_HL

_ld_xbc_a		LDxA	CPU_BC
_ld_xde_a		LDxA	CPU_DE
_ld_xhl_a		LDxA	CPU_HL

_inc_bc			MINC16	CPU_BC
_inc_de			MINC16	CPU_DE
_inc_hl			MINC16	CPU_HL

_inc_b			MINC8	CPU_B
_inc_c			MINC8	CPU_C
_inc_d			MINC8	CPU_D
_inc_e			MINC8	CPU_E
_inc_h			MINC8	CPU_H
_inc_l			MINC8	CPU_L
_inc_xhl		MINCM8	CPU_HL

_dec_b			MDEC8	CPU_B
_dec_c			MDEC8	CPU_C
_dec_d			MDEC8	CPU_D
_dec_e			MDEC8	CPU_E
_dec_h			MDEC8	CPU_H
_dec_l			MDEC8	CPU_L
_dec_xhl		MDECM8	CPU_HL

_ld_b_byte		LD8b	CPU_B
_ld_c_byte		LD8b	CPU_C
_ld_d_byte		LD8b	CPU_D
_ld_e_byte		LD8b	CPU_E
_ld_h_byte		LD8b	CPU_H
_ld_l_byte		LD8b	CPU_L

_add_hl_bc		MADD16	CPU_HL, CPU_BC
_add_hl_de		MADD16	CPU_HL, CPU_DE
_add_hl_hl		MADD16D	CPU_HL
_add_hl_sp		MADDSP	CPU_HL

_dec_bc			MDEC16	CPU_BC
_dec_de			MDEC16	CPU_DE
_dec_hl			MDEC16	CPU_HL

_ld_xword_hl	LDx16	CPU_HL
_ld_hl_xword	LD16x	CPU_HL


_ld_b_c			LD8		CPU_B, CPU_C
_ld_b_d			LD8		CPU_B, CPU_D
_ld_b_e			LD8		CPU_B, CPU_E
_ld_b_h			LD8		CPU_B, CPU_H
_ld_b_l			LD8		CPU_B, CPU_L

_ld_c_b			LD8		CPU_C, CPU_B
_ld_c_d			LD8		CPU_C, CPU_D
_ld_c_e			LD8		CPU_C, CPU_E
_ld_c_h			LD8		CPU_C, CPU_H
_ld_c_l			LD8		CPU_C, CPU_L

_ld_d_b			LD8		CPU_D, CPU_B
_ld_d_c			LD8		CPU_D, CPU_C
_ld_d_e			LD8		CPU_D, CPU_E
_ld_d_h			LD8		CPU_D, CPU_H
_ld_d_l			LD8		CPU_D, CPU_L

_ld_e_b			LD8		CPU_E, CPU_B
_ld_e_c			LD8		CPU_E, CPU_C
_ld_e_d			LD8		CPU_E, CPU_D
_ld_e_h			LD8		CPU_E, CPU_H
_ld_e_l			LD8		CPU_E, CPU_L

_ld_h_b			LD8		CPU_H, CPU_B
_ld_h_c			LD8		CPU_H, CPU_C
_ld_h_d			LD8		CPU_H, CPU_D
_ld_h_e			LD8		CPU_H, CPU_E
_ld_h_l			LD8		CPU_H, CPU_L

_ld_l_b			LD8		CPU_L, CPU_B
_ld_l_c			LD8		CPU_L, CPU_C
_ld_l_d			LD8		CPU_L, CPU_D
_ld_l_e			LD8		CPU_L, CPU_E
_ld_l_h			LD8		CPU_L, CPU_H

_ld_b_xhl		LD8x	CPU_B, CPU_HL
_ld_c_xhl		LD8x	CPU_C, CPU_HL
_ld_d_xhl		LD8x	CPU_D, CPU_HL
_ld_e_xhl		LD8x	CPU_E, CPU_HL
_ld_h_xhl		LD8x	CPU_H, CPU_HL
_ld_l_xhl		LD8x	CPU_L, CPU_HL

_ld_xhl_b		LDx8	CPU_HL, CPU_B
_ld_xhl_c		LDx8	CPU_HL, CPU_C
_ld_xhl_d		LDx8	CPU_HL, CPU_D
_ld_xhl_e		LDx8	CPU_HL, CPU_E
_ld_xhl_h		LDx8	CPU_HL, CPU_H
_ld_xhl_l		LDx8	CPU_HL, CPU_L

_ld_b_a			LD8A	CPU_B
_ld_c_a			LD8A	CPU_C
_ld_d_a			LD8A	CPU_D
_ld_e_a			LD8A	CPU_E
_ld_h_a			LD8A	CPU_H
_ld_l_a			LD8A	CPU_L

_ld_a_b			LDA8	CPU_B
_ld_a_c			LDA8	CPU_C
_ld_a_d			LDA8	CPU_D
_ld_a_e			LDA8	CPU_E
_ld_a_h			LDA8	CPU_H
_ld_a_l			LDA8	CPU_L

_ld_a_xbc		LDAx	CPU_BC
_ld_a_xde		LDAx	CPU_DE
_ld_a_xhl		LDAx	CPU_HL


_add_a_b		MADDR8	CPU_B
_add_a_c		MADDR8	CPU_C
_add_a_d		MADDR8	CPU_D
_add_a_e		MADDR8	CPU_E
_add_a_h		MADDR8	CPU_H
_add_a_l		MADDR8	CPU_L
_add_a_xhl		MADDM8	CPU_HL

_adc_a_b		MADCR8	CPU_B
_adc_a_c		MADCR8	CPU_C
_adc_a_d		MADCR8	CPU_D
_adc_a_e		MADCR8	CPU_E
_adc_a_h		MADCR8	CPU_H
_adc_a_l		MADCR8	CPU_L
_adc_a_xhl		MADCM8	CPU_HL

_sub_b			MSUBR8	CPU_B
_sub_c			MSUBR8	CPU_C
_sub_d			MSUBR8	CPU_D
_sub_e			MSUBR8	CPU_E
_sub_h			MSUBR8	CPU_H
_sub_l			MSUBR8	CPU_L
_sub_xhl		MSUBM8	CPU_HL
_sbc_a_b		MSBCR8	CPU_B
_sbc_a_c		MSBCR8	CPU_C
_sbc_a_d		MSBCR8	CPU_D
_sbc_a_e		MSBCR8	CPU_E
_sbc_a_h		MSBCR8	CPU_H
_sbc_a_l		MSBCR8	CPU_L
_sbc_a_xhl		MSBCM8	CPU_HL

_and_b			MANDR8	CPU_B
_and_c			MANDR8	CPU_C
_and_d			MANDR8	CPU_D
_and_e			MANDR8	CPU_E
_and_h			MANDR8	CPU_H
_and_l			MANDR8	CPU_L
_and_xhl		MANDM8	CPU_HL
_xor_b			MXORR8	CPU_B
_xor_c			MXORR8	CPU_C
_xor_d			MXORR8	CPU_D
_xor_e			MXORR8	CPU_E
_xor_h			MXORR8	CPU_H
_xor_l			MXORR8	CPU_L
_xor_xhl		MXORM8	CPU_HL

_or_b			MORR8	CPU_B
_or_c			MORR8	CPU_C
_or_d			MORR8	CPU_D
_or_e			MORR8	CPU_E
_or_h			MORR8	CPU_H
_or_l			MORR8	CPU_L
_or_xhl			MORM8	CPU_HL
_cp_b			MCPR8	CPU_B
_cp_c			MCPR8	CPU_C
_cp_d			MCPR8	CPU_D
_cp_e			MCPR8	CPU_E
_cp_h			MCPR8	CPU_H
_cp_l			MCPR8	CPU_L
_cp_xhl			MCPM8	CPU_HL

_push_af		mov		r1, r8
				MPUSHr1
_push_bc		MPUSH	CPU_BC
_push_de		MPUSH	CPU_DE
_push_hl		MPUSH	CPU_HL

_pop_af			MPOPr0
				mov		r8, r0
				mov		pc, r11
_pop_bc			MPOP	CPU_BC
_pop_de			MPOP	CPU_DE
_pop_hl			MPOP	CPU_HL


; --- PCレジスタ

_jr				MJR
_jr_nz			MJRNFLG	Z_FLAG
_jr_nc			MJRNFLG	C_FLAG
_jr_z			MJRFLG	Z_FLAG
_jr_c			MJRFLG	C_FLAG

				; ループする事が真と考える事！
_djnz			ldrb	r0, [r7, #CPU_B]
				;
				subs	r0, r0, #1
				;
				strb	r0, [r7, #CPU_B]
				MJRNE

_jp				MJP
_jp_hl			MJPR16	CPU_HL
_jp_nz			MJPNFLG	Z_FLAG
_jp_nc			MJPNFLG	C_FLAG
_jp_po			MJPNFLG	V_FLAG
_jp_p			MJPNFLG	S_FLAG
_jp_z			MJPFLG	Z_FLAG
_jp_c			MJPFLG	C_FLAG
_jp_pe			MJPFLG	V_FLAG
_jp_m			MJPFLG	S_FLAG

_call			MCALL
_call_nz		MCALLNF	Z_FLAG
_call_nc		MCALLNF	C_FLAG
_call_po		MCALLNF	V_FLAG
_call_p			MCALLNF	S_FLAG
_call_z			MCALLF	Z_FLAG
_call_c			MCALLF	C_FLAG
_call_pe		MCALLF	V_FLAG
_call_m			MCALLF	S_FLAG

_ret			MRET
_ret_nz			MRETNF	Z_FLAG
_ret_nc			MRETNF	C_FLAG
_ret_po			MRETNF	V_FLAG
_ret_p			MRETNF	S_FLAG
_ret_z			MRETF	Z_FLAG
_ret_c			MRETF	C_FLAG
_ret_pe			MRETF	V_FLAG
_ret_m			MRETF	S_FLAG

_rst_00			MRST	&00
_rst_08			MRST	&08
_rst_10			MRST	&10
_rst_18			MRST	&18
_rst_20			MRST	&20
_rst_28			MRST	&28
_rst_30			MRST	&30
_rst_38			MRST	&38




; --- HLレジスタ

_ld_xhl_byte	ldrh	r4, [r7, #CPU_HL]
				GETPC8
				MEMWR8	r4, r0
				mov		pc, r11


; --- Aレジスタ

_ld_xbyte_a		GETPC16
				mov		r1, r8, lsr #8
				MEMWR8	r0, r1
				mov		pc, r11

_ld_a_xbyte		GETPC16
				MEMRD8	r1, r0
				and		r2, r8, #&ff
				orr		r8, r2, r1, lsl #8
				mov		pc, r11

_inc_a			bic		r1, r8, #(&ff - C_FLAG)
				add		r0, r7, #CPU_INCFLAG
				add		r1, r1, #(1 << 8)
				ldrb	r2, [r0, r8, lsr #8]
				bic		r1, r1, #(1 << 16)
				orr		r8, r1, r2
				mov		pc, r11

_dec_a			bic		r1, r8, #(&ff - C_FLAG)
				add		r0, r7, #CPU_DECFLAG
				subs	r1, r1, #(1 << 8)
				ldrb	r2, [r0, r8, lsr #8]
				addmi	r1, r1, #(1 << 16)
				orr		r8, r1, r2
				mov		pc, r11

_ld_a_byte		GETPC8
				and		r8, r8, #&ff
				orr		r8, r8, r0, lsl #8
				mov		pc, r11


_add_a_a		and		r0, r8, #(&ff << 8)
				and		r1, r8, #(H_FLAG << 7)
				mov		r2, r0, lsl #16
				adds	r3, r2, r0, lsl #16
				orrcs	r3, r3, #(C_FLAG << 16)
				orrvs	r3, r3, #(V_FLAG << 16)
				orreq	r3, r3, #(Z_FLAG << 16)
				orrmi	r3, r3, #(S_FLAG << 16)
				mov		r8, r3, lsr #16
				orr		r8, r8, r1, lsr #7
				mov		pc, r11

_adc_a_a		tst		r8, #C_FLAG
				and		r0, r8, #(&ff << 8)
				orrne	r0, r0, #&80
				and		r1, r8, #(H_FLAG << 7)
				mov		r2, r0, lsl #16
				adds	r3, r2, r0, lsl #16
				orrcs	r3, r3, #(C_FLAG << 16)
				orrvs	r3, r3, #(V_FLAG << 16)
				orreq	r3, r3, #(Z_FLAG << 16)
				orrmi	r3, r3, #(S_FLAG << 16)
				mov		r8, r3, lsr #16
				orr		r8, r8, r1, lsr #7
				mov		pc, r11

_sub_a			mov		r8, #(Z_FLAG + N_FLAG)
				mov		pc, r11

_sbc_a_a		tst		r8, #C_FLAG
				moveq	r8, #(Z_FLAG + N_FLAG)
				movne	r8, #(&ff << 8)
				orrne	r8, r8, #(S_FLAG + H_FLAG + N_FLAG + C_FLAG)
				mov		pc, r11

_and_a			;
_or_a			ldrb	r0, [r7, r8, lsr #8]
				and		r1, r8, #(&ff << 8)
				orr		r8, r1, r0
				mov		pc, r11

_xor_a			mov		r8, #(Z_FLAG + V_FLAG)
				mov		pc, r11

_cp_a			and		r0, r8, #(&ff << 8)
				orr		r8, r0, #(Z_FLAG + N_FLAG)
				mov		pc, r11

_add_a_byte		GETPC8
				MADD8	r0
_adc_a_byte		GETPC8
				MADC8	r0
_sub_byte		GETPC8
				MSUB8	r0
_sbc_a_byte		GETPC8
				MSBC8	r0
_and_byte		GETPC8
				MAND8	r0
_xor_byte		GETPC8
				MXOR8	r0
_or_byte		GETPC8
				MOR8	r0
_cp_byte		GETPC8
				MCP8	r0


; --- SPレジスタ

_ex_xsp_hl		MEXSP	CPU_HL

_ld_sp_hl		MLDSP	CPU_HL

_ld_sp_word		GETPC16
				mov		r1, r9, lsr #16
				orr		r9, r0, r1, lsl #16
				mov		pc, r11

_inc_sp			add		r9, r9, #1
				movs	r0, r9, lsl #16
				subeq	r9, r9, #(1 << 16)
				mov		pc, r11

_dec_sp			movs	r0, r9, lsl #16
				sub		r9, r9, #1
				addeq	r9, r9, #(1 << 16)
				mov		pc, r11


; ---- AFレジスタ

_ex_af_af		ldrh	r0, [r7, #CPU_AF2]
				strh	r8, [r7, #CPU_AF2]
				mov		r8, r0
				mov		pc, r11

_rlca			and		r0, r8, #(&7f << 8)
				and		r1, r8, #&ec
				tst		r8, #(&80 << 8)
				add		r8, r1, r0, lsl #1
				addne	r8, r8, #C_FLAG
				addne	r8, r8, #(1 << 8)
				mov		pc, r11

_rrca			movs	r0, r8, lsr #9			; a
				and		r1, r8, #&ec			; f
				add		r8, r1, r0, lsl #8
				addcs	r8, r8, #C_FLAG
				addcs	r8, r8, #(1 << 15)
				mov		pc, r11

_rla			and		r0, r8, #(&7f << 8)
				and		r1, r8, #&ec
				tst		r8, #C_FLAG
				addne	r1, r1, #(1 << 8)
				tst		r8, #(&80 << 8)
				addne	r1, r1, #C_FLAG
				add		r8, r1, r0, lsl #1
				mov		pc, r11

_rra			and		r0, r8, #&ec
				movs	r1, r8, lsr #9
				addcs	r0, r0, #C_FLAG
				tst		r8, #C_FLAG
				addne	r0, r0, #(&80 << 8)
				add		r8, r0, r1, lsl #8
				mov		pc, r11


_daa			and		r0, r8, #(&ff << 8)				; dst
				and		r1, r8, #(&0f << 8)				; alow
				tst		r8, #N_FLAG
				bne		_daa_n
				and		r2, r8, #(N_FLAG + C_FLAG)
				cmp		r1, #(&09 << 8)
				orrhi	r2, r2, #H_FLAG			; -> ne..
				tstls	r8, #H_FLAG
				addne	r0, r0, #(&06 << 8)
				cmp		r0, #(&a0 << 8)
				orrcs	r2, r2, #C_FLAG
				tst		r2, #C_FLAG
				addne	r0, r0, #(&60 << 8)
				b		daa_setr8
_daa_n			and		r2, r8, #(N_FLAG + C_FLAG)
				cmp		r0, #(&9a << 8)
				orrcs	r2, r2, #C_FLAG
				tst		r2, #C_FLAG
				subne	r0, r0, #(&60 << 8)
				tst		r8, #H_FLAG
				bne		_daa_nh
				cmp		r1, #(&0a << 8)
				bcc		daa_setr8
				subs	r0, r0, #(&06 << 8)
				orrmi	r2, r2, #C_FLAG
				b		daa_setr8
_daa_nh			cmp		r1, #(&06 << 8)
				orrcc	r2, r2, #H_FLAG
				sub		r0, r0, #(&06 << 8)
daa_setr8		and		r1, r0, #(&ff << 8)
				add		r8, r1, r2
				ldrb	r0, [r7, r1, lsr #8]			; CPU_SZPFLAG
				add		r8, r0, r8
				mov		pc, r11

_cpl			eor		r8, r8, #(&ff << 8)
				orr		r8, r8, #(H_FLAG + N_FLAG)
				mov		pc, r11

_scf			bic		r8, r8, #(H_FLAG + N_FLAG)
				orr		r8, r8, #C_FLAG
				mov		pc, r11

_ccf			bic		r1, r8, #(H_FLAG + N_FLAG)
				tst		r8, #C_FLAG
				orrne	r1, r1, #H_FLAG
				eor		r8, r1, #C_FLAG
				mov		pc, r11


; ---- システム

_di				orr		r6, r6, #(1 << IFF_IFLAG)
				mov		pc, r11

_ei				tst		r6, #(1 << IFF_IFLAG)
				moveq	pc, r11
				bic		r6, r6, #(1 << IFF_IFLAG)
				subs	r1, r10, #1
				movle	pc, r11
				tst		r6, #(1 << IFF_NMI)
				ldr		r2, [r7, #CPU_REQIRQ]
				movne	pc, r11
				cmp		r2, #0
				moveq	pc, r11
				ldr		r0, [r7, #CPU_BASECLOCK]
				mov		r10, #1
				sub		r0, r0, r1
				str		r0, [r7, #CPU_BASECLOCK]
				mov		pc, r11

_halt			sub		r9, r9, #(1 << 16)
				orr		r6, r6, #(1 << IFF_HALT)
				mov		r10, #0
				mov		pc, r11

_ex_de_hl		ldrh	r0, [r7, #CPU_DE]
				ldrh	r1, [r7, #CPU_HL]
				strh	r0, [r7, #CPU_HL]
				strh	r1, [r7, #CPU_DE]
				mov		pc, r11

_exx			ldrh	r0, [r7, #CPU_BC]
				ldrh	r1, [r7, #CPU_DE]
				ldrh	r2, [r7, #CPU_HL]
				ldrh	r3, [r7, #CPU_BC2]
				ldrh	r4, [r7, #CPU_DE2]
				ldrh	r12, [r7, #CPU_HL2]
				strh	r3, [r7, #CPU_BC]
				strh	r4, [r7, #CPU_DE]
				strh	r12, [r7, #CPU_HL]
				strh	r0, [r7, #CPU_BC2]
				strh	r1, [r7, #CPU_DE2]
				strh	r2, [r7, #CPU_HL2]
				mov		pc, r11

_out_byte_a		GETPC8
				and		r2, r8, #(&ff << 8)
				mov		r1, r0
				orr		r0, r2, r0
				str		r10, [r7, #CPU_REMCLOCK]
				bl		iocore_out
				ldr		r10, [r7, #CPU_REMCLOCK]
				mov		pc, r11

_in_a_byte		GETPC8
				and		r1, r8, #(&ff << 8)
				str		r10, [r7, #CPU_REMCLOCK]
				orr		r0, r1, r0
				bl		iocore_inp
				and		r1, r8, #&ff
				ldr		r10, [r7, #CPU_REMCLOCK]
				orr		r8, r1, r0, lsl #8
				mov		pc, r11



; ----

z80a_nonmaskedinterrupt
				stmdb	sp!, {r4 - r11, lr}
				ldr		r7, z8a_r7
				ldr		r6, [r7, #_CPU_IFF]
			;	ldrh	r8, [r7, #_CPU_AF]
				ldr		r9, [r7, #_CPU_SP]
			;	ldr		r10, [r7, #CPU_REMCLOCK]
				tst		r6, #(1 << IFF_NMI)
				bne		__nmiend
				mov		r0, r9, lsl #16
				tst		r6, #(1 << IFF_HALT)
				addne	r9, r9, #&10000
				bicne	r6, r6, #(1 << IFF_HALT)
				sub		r2, r0, #&20000				; stack!
				orr		r6, r6, #(1 << IFF_NMI)
				mov		r0, r2, lsr #16
				mov		r1, r9, lsr #16
				orr		r9, r0, #&660000
				bl		mem_write16
			;	strh	r8, [r7, #_CPU_AF]
				str		r9, [r7, #_CPU_SP]
			;	str		r10, [r7, #CPU_REMCLOCK]
				str		r6, [r7, #_CPU_IFF]
__nmiend		ldmia	sp!, {r4 - r11, pc}


z80a_interrupt
				stmdb	sp!, {r4 - r11, lr}

				ldr		r7, z8a_r7
				ldr		r6, [r7, #_CPU_IFF]
				ldrh	r8, [r7, #_CPU_AF]
				ldr		r9, [r7, #_CPU_SP]
				ldr		r10, [r7, #CPU_REMCLOCK]

				tst		r6, #(1 << IFF_HALT)
				orr		r6, r6, #(1 << IFF_IFLAG)
				bicne	r6, r6, #(1 << IFF_HALT)
				addne	r9, r9, #&10000
				tst		r6, #&30000
				beq		__intrim0

				mov		r2, r9, lsl #16		; stack
				mov		r1, r9, lsr #16		; pc
				sub		r2, r2, #&20000
				tst		r6, #&20000
				bne		__intrim2

__intrim1		mov		r0, r2, lsr #16
				Z80WORK	#11
				orr		r9, r0, #&380000
				adr		lr, __intrend
				b		mem_write16

__intrim2		and		r3, r6, #&ff00
				mov		r9, r2, lsr #16
				orr		r4, r3, r0
				mov		r0, r2, lsr #16
				bl		mem_write16
				mov		r0, r4
				bl		mem_read16
				orr		r9, r9, r0, lsl #16

__intrend		strh	r8, [r7, #_CPU_AF]
				str		r9, [r7, #_CPU_SP]
				str		r10, [r7, #CPU_REMCLOCK]
				str		r6, [r7, #_CPU_IFF]
				ldmia	sp!, {r4 - r11, pc}

__intrim0		adr		r1, clktbl
				adr		r2, optbl
				ldrb	r3, [r1, r0]
				ldr		r1, [r2, r0, lsl #2]
				rsb		r10, r3, r10
				adr		r11, __intrend
				mov		pc, r1


z80a_execute	stmdb	sp!, {r4 - r11, lr}
				ldr		r7, z8a_r7
				ldr		r0, s_dma
				ldrh	r8, [r7, #_CPU_AF]
				ldr		r9, [r7, #_CPU_SP]
				ldrb	r1, [r0]
				ldr		r10, [r7, #CPU_REMCLOCK]
				ldr		r6, [r7, #_CPU_IFF]
				cmp		r1, #0
				bne		__z80awdma

__z80alp		cmp		r10, #0
				ble		__z80aed
				GETPC8
				add		r6, r6, #(1 << 24)
				adr		r1, clktbl
				adr		r2, optbl
				ldrb	r3, [r1, r0]
				ldr		r1, [r2, r0, lsl #2]
				rsb		r10, r3, r10
				adr		r11, __z80alp
				mov		pc, r1

__z80awdma		GETPC8
				add		r6, r6, #(1 << 24)
				adr		r1, clktbl
				adr		r2, optbl
				ldrb	r3, [r1, r0]
				ldr		r1, [r2, r0, lsl #2]
				rsb		r10, r3, r10
				mov		r11, pc
				mov		pc, r1
				bl		z80dmap
				cmp		r10, #0
				bgt		__z80awdma

__z80aed		strh	r8, [r7, #_CPU_AF]
				str		r9, [r7, #_CPU_SP]
				str		r10, [r7, #CPU_REMCLOCK]
				str		r6, [r7, #_CPU_IFF]
				ldmia	sp!, {r4 - r11, pc}


z8a_r7			dcd		z80core + CPU_SIZE
s_dma			dcd		dma


z80a_step		stmdb	sp!, {r4 - r11, lr}
				ldr		r7, z8a_r7
				ldrh	r8, [r7, #_CPU_AF]
				ldr		r9, [r7, #_CPU_SP]
				ldr		r10, [r7, #CPU_REMCLOCK]
				ldr		r6, [r7, #_CPU_IFF]

				GETPC8
				add		r6, r6, #(1 << 24)
				adr		r1, clktbl
				adr		r2, optbl
				ldrb	r3, [r1, r0]
				ldr		r1, [r2, r0, lsl #2]
				rsb		r10, r3, r10
				mov		r11, pc
				mov		pc, r1

				strh	r8, [r7, #_CPU_AF]
				str		r9, [r7, #_CPU_SP]
				str		r10, [r7, #CPU_REMCLOCK]
				str		r6, [r7, #_CPU_IFF]
				ldmia	sp!, {r4 - r11, lr}
				b		z80dmap



clktbl			dcb	 4,10, 7, 6, 4, 4, 7, 4, 4,11, 7, 6, 4, 4, 7, 4
				dcb	 8,10, 7, 6, 4, 4, 7, 4, 7,11, 7, 6, 4, 4, 7, 4
				dcb	 7,10,16, 6, 4, 4, 7, 4, 7,11,16, 6, 4, 4, 7, 4
				dcb	 7,10,13, 6,11,11,10, 4, 7,11,13, 6, 4, 4, 7, 4
				dcb	 4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4
				dcb	 4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4
				dcb	 4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4
				dcb	 7, 7, 7, 7, 7, 7, 4, 7, 4, 4, 4, 4, 4, 4, 7, 4
				dcb	 4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4
				dcb	 4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4
				dcb	 4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4
				dcb	 4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4
				dcb	 5,10,10,10,10,11, 7,11, 5, 4,10, 0,10,10, 7,11
				dcb	 5,10,10,11,10,11, 7,11, 5, 4,10,11,10, 0, 7,11
				dcb	 5,10,10,19,10,11, 7,11, 5, 4,10, 4,10, 0, 7,11
				dcb	 5,10,10, 4,10,11, 7,11, 5, 6,10, 4,10, 0, 7,11

optbl	dcd		_ld_nop,		_ld_bc_word,	_ld_xbc_a,		_inc_bc
		dcd		_inc_b,			_dec_b,			_ld_b_byte,		_rlca
		dcd		_ex_af_af,		_add_hl_bc,		_ld_a_xbc,		_dec_bc
		dcd		_inc_c,			_dec_c,			_ld_c_byte,		_rrca

		dcd		_djnz,			_ld_de_word,	_ld_xde_a,		_inc_de
		dcd		_inc_d,			_dec_d,			_ld_d_byte,		_rla
		dcd		_jr,			_add_hl_de,		_ld_a_xde,		_dec_de
		dcd		_inc_e,			_dec_e,			_ld_e_byte,		_rra

		dcd		_jr_nz,			_ld_hl_word,	_ld_xword_hl,	_inc_hl
		dcd		_inc_h,			_dec_h,			_ld_h_byte,		_daa
		dcd		_jr_z,			_add_hl_hl,		_ld_hl_xword,	_dec_hl
		dcd		_inc_l,			_dec_l,			_ld_l_byte,		_cpl

		dcd		_jr_nc,			_ld_sp_word,	_ld_xbyte_a,	_inc_sp
		dcd		_inc_xhl,		_dec_xhl,		_ld_xhl_byte,	_scf
		dcd		_jr_c,			_add_hl_sp,		_ld_a_xbyte,	_dec_sp
		dcd		_inc_a,			_dec_a,			_ld_a_byte,		_ccf

		dcd		_ld_nop,		_ld_b_c,		_ld_b_d,		_ld_b_e
		dcd		_ld_b_h,		_ld_b_l,		_ld_b_xhl,		_ld_b_a
		dcd		_ld_c_b,		_ld_nop,		_ld_c_d,		_ld_c_e
		dcd		_ld_c_h,		_ld_c_l,		_ld_c_xhl,		_ld_c_a

		dcd		_ld_d_b,		_ld_d_c,		_ld_nop,		_ld_d_e
		dcd		_ld_d_h,		_ld_d_l,		_ld_d_xhl,		_ld_d_a
		dcd		_ld_e_b,		_ld_e_c,		_ld_e_d,		_ld_nop
		dcd		_ld_e_h,		_ld_e_l,		_ld_e_xhl,		_ld_e_a

		dcd		_ld_h_b,		_ld_h_c,		_ld_h_d,		_ld_h_e
		dcd		_ld_nop,		_ld_h_l,		_ld_h_xhl,		_ld_h_a
		dcd		_ld_l_b,		_ld_l_c,		_ld_l_d,		_ld_l_e
		dcd		_ld_l_h,		_ld_nop,		_ld_l_xhl,		_ld_l_a

		dcd		_ld_xhl_b,		_ld_xhl_c,		_ld_xhl_d,		_ld_xhl_e
		dcd		_ld_xhl_h,		_ld_xhl_l,		_halt,			_ld_xhl_a
		dcd		_ld_a_b,		_ld_a_c,		_ld_a_d,		_ld_a_e
		dcd		_ld_a_h,		_ld_a_l,		_ld_a_xhl,		_ld_nop

		dcd		_add_a_b,		_add_a_c,		_add_a_d,		_add_a_e
		dcd		_add_a_h,		_add_a_l,		_add_a_xhl,		_add_a_a
		dcd		_adc_a_b,		_adc_a_c,		_adc_a_d,		_adc_a_e
		dcd		_adc_a_h,		_adc_a_l,		_adc_a_xhl,		_adc_a_a

		dcd		_sub_b,			_sub_c,			_sub_d,			_sub_e
		dcd		_sub_h,			_sub_l,			_sub_xhl,		_sub_a
		dcd		_sbc_a_b,		_sbc_a_c,		_sbc_a_d,		_sbc_a_e
		dcd		_sbc_a_h,		_sbc_a_l,		_sbc_a_xhl,		_sbc_a_a

		dcd		_and_b,			_and_c,			_and_d,			_and_e
		dcd		_and_h,			_and_l,			_and_xhl,		_and_a
		dcd		_xor_b,			_xor_c,			_xor_d,			_xor_e
		dcd		_xor_h,			_xor_l,			_xor_xhl,		_xor_a

		dcd		_or_b,			_or_c,			_or_d,			_or_e
		dcd		_or_h,			_or_l,			_or_xhl,		_or_a
		dcd		_cp_b,			_cp_c,			_cp_d,			_cp_e
		dcd		_cp_h,			_cp_l,			_cp_xhl,		_cp_a

		dcd		_ret_nz,		_pop_bc,		_jp_nz,			_jp
		dcd		_call_nz,		_push_bc,		_add_a_byte,	_rst_00
		dcd		_ret_z,			_ret,			_jp_z,			z80a_cb
		dcd		_call_z,		_call,			_adc_a_byte,	_rst_08

		dcd		_ret_nc,		_pop_de,		_jp_nc,			_out_byte_a
		dcd		_call_nc,		_push_de,		_sub_byte,		_rst_10
		dcd		_ret_c,			_exx,			_jp_c,			_in_a_byte
		dcd		_call_c,		z80a_ix,		_sbc_a_byte,	_rst_18

		dcd		_ret_po,		_pop_hl,		_jp_po,			_ex_xsp_hl
		dcd		_call_po,		_push_hl,		_and_byte,		_rst_20
		dcd		_ret_pe,		_jp_hl,			_jp_pe,			_ex_de_hl
		dcd		_call_pe,		z80a_sb,		_xor_byte,		_rst_28

		dcd		_ret_p,			_pop_af,		_jp_p,			_di
		dcd		_call_p,		_push_af,		_or_byte,		_rst_30
		dcd		_ret_m,			_ld_sp_hl,		_jp_m,			_ei
		dcd		_call_m,		z80a_iy,		_cp_byte,		_rst_38

	END

