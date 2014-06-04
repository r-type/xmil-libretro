
	.global	vsyncff_init
	.global	vsyncff_turn

	.section	.text
	.code	32
	.align	0

vsyncff_init:	ldr		r0, 100f
				ldr		r1, 101f
				mov		r2, #0x04000000
				str		r1, [r0]
				mov		r3, #8
				strh	r3, [r2, #4]
				ldr		r0, [r2, #0x200]!
				orr		r0, r0, #1
				strh	r0, [r2, #0]
				mov		pc, lr

100:			.word	__advirq
101:			.word	vsyncff_int


vsyncff_turn:	ands	r1, r0, #1
				ldr		r2, 200f
				strb	r1, [r2, #2]
				movne	pc, lr					@ éüÇÃäÑÇËçûÇ›Ç≈ÉZÉbÉg
				ldrh	r2, [r2]
				mov		r3, #0x04000000
				bic		r2, r2, #0x10
				strh	r2, [r3]
				mov		pc, lr

200:			.word	vsyncff



	.section	.iwram.text
	.code	32

vsyncff:		.word	0x1404

vsyncff_int:	tst		r0, #1
				moveq	pc, lr

				ldr		r1, vsyncff
				mov		r2, #0x04000000
				eor		r3, r1, #0x10
				tst		r1, #0x10000
				strneb	r3, vsyncff
				strneh	r3, [r2]
				mov		pc, lr

	.end

