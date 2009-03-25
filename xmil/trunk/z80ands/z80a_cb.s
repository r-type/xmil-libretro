
	.include	"../z80ands/z80a.inc"

	.global	z80a_cb
	.global	z80a_icb

	.section	.itcm
	.code	32
	.align	2


.macro	BITA	b, s
			tst		rAF, #(\b << 8)
			bic		rAF, rAF, #(0xff - C_FLAG)
			orreq	rAF, rAF, #(H_FLAG + Z_FLAG)
			orrne	rAF, rAF, #(H_FLAG + \s)
			mov		pc, r11
.endm

.macro	BRA		b
			bic		rAF, rAF, #(\b << 8)
			mov		pc, r11
.endm

.macro	BSA		b
			orr		rAF, rAF, #(\b << 8)
			mov		pc, r11
.endm


rlc_a:			and		r0, rAF, #(0x7f << 8)
				cmp		rAF, #0x8000
				orrcs	r0, r0, #0x80
				add		r1, r4, #CPU_SZPFLAG
				adc		r2, r0, r0
				ldrb	r3, [r1, r0, lsr #7]
				add		rAF, r3, r2
				mov		pc, r11

rrc_a:			movs	r0, rAF, lsr #9
				orrcs	r0, r0, #0x80
				add		r1, r4, #CPU_SZPFLAG
				ldrb	r2, [r1, r0]
				adc		rAF, r2, r0, lsl #8
				mov		pc, r11

rl_a:			and		r0, rAF, #(0x7f << 8)
				tst		rAF, #C_FLAG
				orrne	r0, r0, #0x80
				add		r1, r4, #CPU_SZPFLAG
				cmp		rAF, #0x8000
				ldrb	r2, [r1, r0, lsr #7]
				adc		rAF, r2, r0, lsl #1
				mov		pc, r11

rr_a:			mov		r0, rAF, lsr #9
				tst		rAF, #C_FLAG
				orrne	r0, r0, #0x80
				add		r1, r4, #CPU_SZPFLAG
				ldrb	r2, [r1, r0]
				movs	r3, rAF, lsr #9
				adc		rAF, r2, r0, lsl #8
				mov		pc, r11

sla_a:			and		r0, rAF, #(0x7f << 8)
				add		r1, r4, #CPU_SZPFLAG
				movs	r2, rAF, lsr #16
				ldrb	r3, [r1, r0, lsr #7]
				adc		rAF, r3, r0, lsl #1
				mov		pc, r11

sra_a:			tst		rAF, #0x8000
				orrne	rAF, rAF, #0x10000
srl_a:			movs	r0, rAF, lsr #9
				add		r1, r4, #CPU_SZPFLAG
				ldrb	r2, [r1, r0]
				adc		rAF, r2, r0, lsl #8
				mov		pc, r11

sll_a:			and		r0, rAF, #(0x7f << 8)
				orr		r0, r0, #0x80
				add		r1, r4, #CPU_SZPFLAG
				movs	r2, rAF, lsr #16
				ldrb	r2, [r1, r0, lsr #7]
				adc		rAF, r2, r0, lsl #1
				mov		pc, r11


bit0_a:		BITA	0x01, 0
bit1_a:		BITA	0x02, 0
bit2_a:		BITA	0x04, 0
bit3_a:		BITA	0x08, 0
bit4_a:		BITA	0x10, 0
bit5_a:		BITA	0x20, 0
bit6_a:		BITA	0x40, 0
bit7_a:		BITA	0x80, S_FLAG

br0_a:		BRA		0x01
br1_a:		BRA		0x02
br2_a:		BRA		0x04
br3_a:		BRA		0x08
br4_a:		BRA		0x10
br5_a:		BRA		0x20
br6_a:		BRA		0x40
br7_a:		BRA		0x80

bs0_a:		BSA		0x01
bs1_a:		BSA		0x02
bs2_a:		BSA		0x04
bs3_a:		BSA		0x08
bs4_a:		BSA		0x10
bs5_a:		BSA		0x20
bs6_a:		BSA		0x40
bs7_a:		BSA		0x80


.macro	BITR3	b, s
			bic		rAF, rAF, #(0xff - C_FLAG)
			tst		r0, #\b
			orreq	rAF, rAF, #(H_FLAG + Z_FLAG)
			orrne	rAF, rAF, #(H_FLAG + \s)
			mov		pc, r11
.endm

.macro	BRR3	b
			mov		r1, #\b
			bic		r0, r1
			strb	r0, [r3]
			bx		r11
.endm

.macro	BSR3	b
			mov		r1, #\b
			orr		r0, r1
			strb	r0, [r3]
			bx		r11
.endm


rlc_m:			Z80WORK	#3
rlc_r:			and		r1, r0, #0x7f
				cmp		r0, #0x80
				adc		r2, r1, r1
				add		r1, r4, #CPU_SZPFLAG
				and		rAF, rAF, #(0xff << 8)
				ldrb	r0, [r1, r2]
				strb	r2, [r3]
				adc		rAF, r0, rAF
				mov		pc, r11

rrc_m:			Z80WORK	#3
rrc_r:			movs	r0, r0, lsr #1
				orrcs	r0, r0, #0x80
				add		r1, r4, #CPU_SZPFLAG
				and		rAF, rAF, #(0xff << 8)
				ldrb	r2, [r1, r0]
				strb	r0, [r3]
				adc		rAF, r2, rAF
				mov		pc, r11

rl_m:			Z80WORK	#3
rl_r:			and		r1, rAF, #C_FLAG
				movs	r2, r0, lsl #25
				orr		r12, r2, r1, lsl #24
				add		r0, r4, #CPU_SZPFLAG
				mov		r1, r12, lsr #24
				and		rAF, rAF, #(0xff << 8)
				ldrb	r2, [r1, r0]
				strb	r1, [r3]
				adc		rAF, r2, rAF
				mov		pc, r11

rr_m:			Z80WORK	#3
rr_r:			and		r1, rAF, #C_FLAG
				movs	r2, r0, lsr #1
				orr		r0, r2, r1, lsl #7
				add		r1, r4, #CPU_SZPFLAG
				and		rAF, rAF, #(0xff << 8)
				ldrb	r2, [r1, r0]
				strb	r0, [r3]
				adc		rAF, r2, rAF
				mov		pc, r11

sla_m:			Z80WORK	#3
sla_r:			movs	r1, r0, lsl #25
				and		rAF, rAF, #(0xff << 8)
				add		r2, r4, #CPU_SZPFLAG
				mov		r12, r1, lsr #24
				ldrb	r0, [r2, r1, lsr #24]
				strb	r12, [r3]
				adc		rAF, r0, rAF
				mov		pc, r11

sra_m:			Z80WORK	#3
sra_r:			tst		r0, #0x80
				orrne	r0, r0, #0x100
				movs	r1, r0, lsr #1
				add		r2, r4, #CPU_SZPFLAG
				and		rAF, rAF, #(0xff << 8)
				ldrb	r12, [r2, r1]
				strb	r1, [r3]
				adc		rAF, r12, rAF
				mov		pc, r11

sll_m:			Z80WORK	#3
sll_r:			movs	r1, r0, lsl #25
				add		r1, r1, #(1 << 24)
				add		r2, r4, #CPU_SZPFLAG
				and		rAF, rAF, #(0xff << 8)
				mov		r12, r1, lsr #24
				ldrb	r0, [r2, r1, lsr #24]
				strb	r12, [r3]
				adc		rAF, r0, rAF
				mov		pc, r11

srl_m:			Z80WORK	#3
srl_r:			movs	r1, r0, lsr #1
				add		r2, r4, #CPU_SZPFLAG
				and		rAF, rAF, #(0xff << 8)
				ldrb	r0, [r2, r1]
				strb	r1, [r3]
				adc		rAF, r0, rAF
				mov		pc, r11


bit0_r:		BITR3	0x01, 0
bit1_r:		BITR3	0x02, 0
bit2_r:		BITR3	0x04, 0
bit3_r:		BITR3	0x08, 0
bit4_r:		BITR3	0x10, 0
bit5_r:		BITR3	0x20, 0
bit6_r:		BITR3	0x40, 0
bit7_r:		BITR3	0x80, S_FLAG

	.code	16

br0_m:		Z80WORK	#3
br0_r:		BRR3	0x01
br1_m:		Z80WORK	#3
br1_r:		BRR3	0x02
br2_m:		Z80WORK	#3
br2_r:		BRR3	0x04
br3_m:		Z80WORK	#3
br3_r:		BRR3	0x08
br4_m:		Z80WORK	#3
br4_r:		BRR3	0x10
br5_m:		Z80WORK	#3
br5_r:		BRR3	0x20
br6_m:		Z80WORK	#3
br6_r:		BRR3	0x40
br7_m:		Z80WORK	#3
br7_r:		BRR3	0x80

bs0_m:		Z80WORK	#3
bs0_r:		BSR3	0x01
bs1_m:		Z80WORK	#3
bs1_r:		BSR3	0x02
bs2_m:		Z80WORK	#3
bs2_r:		BSR3	0x04
bs3_m:		Z80WORK	#3
bs3_r:		BSR3	0x08
bs4_m:		Z80WORK	#3
bs4_r:		BSR3	0x10
bs5_m:		Z80WORK	#3
bs5_r:		BSR3	0x20
bs6_m:		Z80WORK	#3
bs6_r:		BSR3	0x40
bs7_m:		Z80WORK	#3
bs7_r:		BSR3	0x80


	.code	32

z80a_cb:	GETPC8
			add		r9, r9, #(1 << 24)
			and		r1, r0, #7					; @ reg
			and		r2, r0, #0xf8				; @ ctrl
			cmp		r1, #6
			bcc		cb_r
			beq		cb_xhl

cb_a:		Z80WORK	#4
			ldr		r12, [pc, r2, lsr #1]
			bx		r12

		.word	rlc_a,		rrc_a,		rl_a,		rr_a
		.word	sla_a,		sra_a,		sll_a,		srl_a
		.word	bit0_a,		bit1_a,		bit2_a,		bit3_a
		.word	bit4_a,		bit5_a,		bit6_a,		bit7_a
		.word	br0_a,		br1_a,		br2_a,		br3_a
		.word	br4_a,		br5_a,		br6_a,		br7_a
		.word	bs0_a,		bs1_a,		bs2_a,		bs3_a
		.word	bs4_a,		bs5_a,		bs6_a,		bs7_a


cb_r:		eor		r1, r1, #1
			add		r3, r1, r4
			Z80WORK	#4
			ldrb	r0, [r3]
			ldr		r12, [pc, r2, lsr #1]
			bx		r12

		.word	rlc_r,		rrc_r,		rl_r,		rr_r
		.word	sla_r,		sra_r,		sll_r,		srl_r
		.word	bit0_r,		bit1_r,		bit2_r,		bit3_r
		.word	bit4_r,		bit5_r,		bit6_r,		bit7_r
		.word	br0_r+1,	br1_r+1,	br2_r+1,	br3_r+1
		.word	br4_r+1,	br5_r+1,	br6_r+1,	br7_r+1
		.word	bs0_r+1,	bs1_r+1,	bs2_r+1,	bs3_r+1
		.word	bs4_r+1,	bs5_r+1,	bs6_r+1,	bs7_r+1


cb_xhl:		ldrh	r1, [r4, #CPU_HL]
			MEMRD8	r0, r1
			MEMWRP	r3, r1
			Z80WORK	#8
			ldr		r12, [pc, r2, lsr #1]
			bx		r12

cbr3_m:	.word	rlc_m,		rrc_m,		rl_m,		rr_m
		.word	sla_m,		sra_m,		sll_m,		srl_m
		.word	bit0_r,		bit1_r,		bit2_r,		bit3_r
		.word	bit4_r,		bit5_r,		bit6_r,		bit7_r
		.word	br0_m+1,	br1_m+1,	br2_m+1,	br3_m+1
		.word	br4_m+1,	br5_m+1,	br6_m+1,	br7_m+1
		.word	bs0_m+1,	bs1_m+1,	bs2_m+1,	bs3_m+1
		.word	bs4_m+1,	bs5_m+1,	bs6_m+1,	bs7_m+1


z80a_icb:	GETPC8
			adr		r12, cbr3_m
			and		r0, r0, #0xf8
			add		r9, r9, #(1 << 24)
			ldr		r12, [r12, r0, lsr #1]
			MEMRD8	r0, r2
			MEMWRP	r3, r2
			Z80WORK	#1
			bx		r12

	.end

