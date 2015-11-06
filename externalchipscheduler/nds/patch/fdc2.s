
	.include	"./nds/ramptr.inc"

	.section	.itcm
	.code	32
	.align	2


	.global fdc_o
	.global fdc_i


fdc_o:			movs	r3, r0, lsl #21
				addcss	r3, #(8 << 21)
				addcc	pc, lr, #4
				add		r0, r4, #IWRAM_FDC
				ldr		r3, [pc, r3, lsr #19]
				bx		r3
				.word	fdc_o0ff8
				.word	fdc_o0ff9
				.word	fdc_o0ffa
				.word	fdc_o0ffb
				.word	fdc_o0ffc
				.word	fdc_o0
				.word	fdc_o0
				.word	fdc_o0


fdc_i:			movs	r3, r0, lsl #21
				addcss	r3, #(8 << 21)
				addcc	pc, lr, #4
				add		r0, r4, #IWRAM_FDC
				ldr		r3, [pc, r3, lsr #19]
				bx		r3
				.word	fdc_i0ff8
				.word	fdc_i0ff9
				.word	fdc_i0ffa
				.word	fdc_i0ffb
				.word	fdc_i0ffc
				.word	fdc_i0ffd
				.word	fdc_i0ffe
				.word	fdc_i0fff

	.end

