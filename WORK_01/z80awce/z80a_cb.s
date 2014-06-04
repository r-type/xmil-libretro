
	INCLUDE	z80a.inc

	EXPORT	z80a_cb
	EXPORT	z80a_ixcb
	EXPORT	z80a_iycb

	AREA	.text, CODE, READONLY

	MACRO
$label	BITA	$b, $s
$label		tst		r8, #($b << 8)
			bic		r8, r8, #(&ff - C_FLAG)
			orreq	r8, r8, #(H_FLAG + Z_FLAG)
			orrne	r8, r8, #(H_FLAG + $s)
			mov		pc, r11
	MEND

	MACRO
$label	BRA		$b
$label		bic		r8, r8, #($b << 8)
			mov		pc, r11
	MEND

	MACRO
$label	BSA		$b
$label		orr		r8, r8, #($b << 8)
			mov		pc, r11
	MEND

rlc_a			and		r0, r8, #(&7f << 8)
				cmp		r8, #&8000
				orrcs	r0, r0, #&80
				adc		r1, r0, r0
				ldrb	r2, [r7, r0, lsr #7]
				add		r8, r2, r1
				mov		pc, r11

rrc_a			movs	r0, r8, lsr #9
				orrcs	r0, r0, #&80
				ldrb	r1, [r0, r7]
				adc		r8, r1, r0, lsl #8
				mov		pc, r11

rl_a			and		r0, r8, #(&7f << 8)
				tst		r8, #C_FLAG
				orrne	r0, r0, #&80
				cmp		r8, #&8000
				ldrb	r1, [r7, r0, lsr #7]
				adc		r8, r1, r0 lsl #1
				mov		pc, r11

rr_a			mov		r0, r8, lsr #9
				tst		r8, #C_FLAG
				orrne	r0, r0, #&80
				ldrb	r1, [r0, r7]
				movs	r2, r8, lsr #9
				adc		r8, r1, r0, lsl #8
				mov		pc, r11

sla_a			and		r0, r8, #(&7f << 8)
				movs	r2, r8, lsr #16
				ldrb	r1, [r7, r0, lsr #7]
				adc		r8, r1, r0, lsl #1
				mov		pc, r11

sra_a			tst		r8, #&8000
				orrne	r8, r8, #&10000
srl_a			movs	r0, r8, lsr #9
				ldrb	r1, [r0, r7]
				adc		r8, r1, r0, lsl #8
				mov		pc, r11

sll_a			and		r0, r8, #(&7f << 8)
				orr		r0, r0, #&80
				movs	r2, r8, lsr #16
				ldrb	r1, [r7, r0, lsr #7]
				adc		r8, r1, r0, lsl #1
				mov		pc, r11


bit0_a		BITA	&01, 0
bit1_a		BITA	&02, 0
bit2_a		BITA	&04, 0
bit3_a		BITA	&08, 0
bit4_a		BITA	&10, 0
bit5_a		BITA	&20, 0
bit6_a		BITA	&40, 0
bit7_a		BITA	&80, S_FLAG

br0_a		BRA		&01
br1_a		BRA		&02
br2_a		BRA		&04
br3_a		BRA		&08
br4_a		BRA		&10
br5_a		BRA		&20
br6_a		BRA		&40
br7_a		BRA		&80

bs0_a		BSA		&01
bs1_a		BSA		&02
bs2_a		BSA		&04
bs3_a		BSA		&08
bs4_a		BSA		&10
bs5_a		BSA		&20
bs6_a		BSA		&40
bs7_a		BSA		&80



	MACRO
$label	BITR4	$b, $s
$label		bic		r8, r8, #(&ff - C_FLAG)
			tst		r0, #$b
			orreq	r8, r8, #(H_FLAG + Z_FLAG)
			orrne	r8, r8, #(H_FLAG + $s)
			mov		pc, r11
	MEND

	MACRO
$label	BRR4	$b
$label		bic		r0, r0, #$b
			strb	r0, [r4]
			mov		pc, r11
	MEND

	MACRO
$label	BSR4	$b
$label		orr		r0, r0, #$b
			strb	r0, [r4]
			mov		pc, r11
	MEND


rlc_m			Z80WORK	#23
rlc_r			and		r1, r0, #&7f
				cmp		r0, #&80
				adc		r2, r1, r1
				and		r8, r8, #(&ff << 8)
				ldrb	r3, [r2, r7]
				strb	r2, [r4]
				adc		r8, r3, r8
				mov		pc, r11

rrc_m			Z80WORK	#23
rrc_r			movs	r0, r0, lsr #1
				and		r8, r8, #(&ff << 8)
				orrcs	r0, r0, #&80
				ldrb	r1, [r0, r7]
				strb	r0, [r4]
				adc		r8, r1, r8
				mov		pc, r11

rl_m			Z80WORK	#23
rl_r			and		r1, r8, #C_FLAG
				movs	r2, r0, lsl #25
				orr		r3, r2, r1, lsl #24
				and		r8, r8, #(&ff << 8)
				mov		r1, r3, lsr #24
				ldrb	r2, [r7, r3, lsr #24]
				strb	r1, [r4]
				adc		r8, r2, r8
				mov		pc, r11

rr_m			Z80WORK	#23
rr_r			and		r1, r8, #C_FLAG
				movs	r2, r0, lsr #1
				orr		r3, r2, r1, lsl #7
				and		r8, r8, #(&ff << 8)
				ldrb	r1, [r3, r7]
				strb	r3, [r4]
				adc		r8, r1, r8
				mov		pc, r11

sla_m			Z80WORK	#23
sla_r			movs	r1, r0, lsl #25
				and		r8, r8, #(&ff << 8)
				mov		r2, r1, lsr #24
				ldrb	r3, [r7, r1, lsr #24]
				strb	r2, [r4]
				adc		r8, r3, r8
				mov		pc, r11

sra_m			Z80WORK	#23
sra_r			tst		r0, #&80
				orrne	r0, r0, #&100
				movs	r1, r0, lsr #1
				and		r8, r8, #(&ff << 8)
				ldrb	r2, [r1, r7]
				strb	r1, [r4]
				adc		r8, r2, r8
				mov		pc, r11

sll_m			Z80WORK	#23
sll_r			movs	r1, r0, lsl #25
				add		r1, r1, #(1 << 24)
				and		r8, r8, #(&ff << 8)
				mov		r2, r1, lsr #24
				ldrb	r3, [r7, r1, lsr #24]
				strb	r2, [r4]
				adc		r8, r3, r8
				mov		pc, r11

srl_m			Z80WORK	#23
srl_r			movs	r1, r0, lsr #1
				and		r8, r8, #(&ff << 8)
				ldrb	r2, [r1, r7]
				strb	r1, [r4]
				adc		r8, r2, r8
				mov		pc, r11


bit0_m		Z80WORK	#20
bit0_r		BITR4	&01, 0
bit1_m		Z80WORK	#20
bit1_r		BITR4	&02, 0
bit2_m		Z80WORK	#20
bit2_r		BITR4	&04, 0
bit3_m		Z80WORK	#20
bit3_r		BITR4	&08, 0
bit4_m		Z80WORK	#20
bit4_r		BITR4	&10, 0
bit5_m		Z80WORK	#20
bit5_r		BITR4	&20, 0
bit6_m		Z80WORK	#20
bit6_r		BITR4	&40, 0
bit7_m		Z80WORK	#20
bit7_r		BITR4	&80, S_FLAG

br0_m		Z80WORK	#23
br0_r		BRR4	&01
br1_m		Z80WORK	#23
br1_r		BRR4	&02
br2_m		Z80WORK	#23
br2_r		BRR4	&04
br3_m		Z80WORK	#23
br3_r		BRR4	&08
br4_m		Z80WORK	#23
br4_r		BRR4	&10
br5_m		Z80WORK	#23
br5_r		BRR4	&20
br6_m		Z80WORK	#23
br6_r		BRR4	&40
br7_m		Z80WORK	#23
br7_r		BRR4	&80

bs0_m		Z80WORK	#23
bs0_r		BSR4	&01
bs1_m		Z80WORK	#23
bs1_r		BSR4	&02
bs2_m		Z80WORK	#23
bs2_r		BSR4	&04
bs3_m		Z80WORK	#23
bs3_r		BSR4	&08
bs4_m		Z80WORK	#23
bs4_r		BSR4	&10
bs5_m		Z80WORK	#23
bs5_r		BSR4	&20
bs6_m		Z80WORK	#23
bs6_r		BSR4	&40
bs7_m		Z80WORK	#23
bs7_r		BSR4	&80


z80a_cb		GETPC8
			add		r6, r6, #(1 << 24)
			and		r1, r0, #7				; reg
			and		r2, r0, #&f8			; ctrl
			cmp		r1, #6
			bcc		cb_r
			beq		cb_xhl

cb_a		adr		r12, cba_r
			ldr		r2, [r12, r2, lsr #1]
			Z80WORK	#8
			mov		pc, r2
cba_r		dcd		rlc_a
			dcd		rrc_a
			dcd		rl_a
			dcd		rr_a
			dcd		sla_a
			dcd		sra_a
			dcd		sll_a
			dcd		srl_a
			dcd		bit0_a
			dcd		bit1_a
			dcd		bit2_a
			dcd		bit3_a
			dcd		bit4_a
			dcd		bit5_a
			dcd		bit6_a
			dcd		bit7_a
			dcd		br0_a
			dcd		br1_a
			dcd		br2_a
			dcd		br3_a
			dcd		br4_a
			dcd		br5_a
			dcd		br6_a
			dcd		br7_a
			dcd		bs0_a
			dcd		bs1_a
			dcd		bs2_a
			dcd		bs3_a
			dcd		bs4_a
			dcd		bs5_a
			dcd		bs6_a
			dcd		bs7_a

cb_r		adr		r12, cbr4_r
			add		r3, r7, #CPU_C
			eor		r1, r1, #1
			ldr		r2, [r12, r2, lsr #1]
			Z80WORK	#8
			ldrb	r0, [r1, r3]
			add		r4, r1, r3
			mov		pc, r2
cbr4_r		dcd		rlc_r
			dcd		rrc_r
			dcd		rl_r
			dcd		rr_r
			dcd		sla_r
			dcd		sra_r
			dcd		sll_r
			dcd		srl_r
			dcd		bit0_r
			dcd		bit1_r
			dcd		bit2_r
			dcd		bit3_r
			dcd		bit4_r
			dcd		bit5_r
			dcd		bit6_r
			dcd		bit7_r
			dcd		br0_r
			dcd		br1_r
			dcd		br2_r
			dcd		br3_r
			dcd		br4_r
			dcd		br5_r
			dcd		br6_r
			dcd		br7_r
			dcd		bs0_r
			dcd		bs1_r
			dcd		bs2_r
			dcd		bs3_r
			dcd		bs4_r
			dcd		bs5_r
			dcd		bs6_r
			dcd		bs7_r

cb_xhl		ldrh	r4, [r7, #CPU_HL]
			MEMLEA8	r4, r0, r1
			adr		r12, cbr4_m
			ldrb	r0, [r0, r4]
			ldr		r2, [r12, r2, lsr #1]
			add		r4, r4, r1
			mov		pc, r2
cbr4_m		dcd		rlc_m
			dcd		rrc_m
			dcd		rl_m
			dcd		rr_m
			dcd		sla_m
			dcd		sra_m
			dcd		sll_m
			dcd		srl_m
			dcd		bit0_m
			dcd		bit1_m
			dcd		bit2_m
			dcd		bit3_m
			dcd		bit4_m
			dcd		bit5_m
			dcd		bit6_m
			dcd		bit7_m
			dcd		br0_m
			dcd		br1_m
			dcd		br2_m
			dcd		br3_m
			dcd		br4_m
			dcd		br5_m
			dcd		br6_m
			dcd		br7_m
			dcd		bs0_m
			dcd		bs1_m
			dcd		bs2_m
			dcd		bs3_m
			dcd		bs4_m
			dcd		bs5_m
			dcd		bs6_m
			dcd		bs7_m


z80a_ixcb	GETIPC	r1, CPU_IX
			add		r6, r6, #(1 << 24)
			GETPC8
			MEMLEA8	r1, r2, r3
			adr		r12, cbr4_m
			and		r0, r0, #&f8
			ldr		r12, [r12, r0, lsr #1]
			ldrb	r0, [r1, r2]
			add		r4, r1, r3
			mov		pc, r12

z80a_iycb	GETIPC	r1, CPU_IY
			add		r6, r6, #(1 << 24)
			GETPC8
			MEMLEA8	r1, r2, r3
			adr		r12, cbr4_m
			and		r0, r0, #&f8
			ldr		r12, [r12, r0, lsr #1]
			ldrb	r0, [r1, r2]
			add		r4, r1, r3
			mov		pc, r12


	END

