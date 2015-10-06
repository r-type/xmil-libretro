
	.include	"./nds/ramptr.inc"
	.include	"../z80ands/z80a.inc"
	.include	"./patch/iocore.inc"
	.include	"./vram/vram.inc"

	.global	tram_ank_o

	.section	.itcm
	.code	32
	.align	2

tram_ank_o:		mov		r0, r0, lsl #21
				ldr		r12, =tram
				add		r12, r12, r0, lsr #19
				ldrh	r3, [r12]
				cmp		r1, r3, lsr #8
				addeq	pc, lr, #4
				strb	r1, [r12, #1]

				add		r12, r2, #IOCORE2VRAMUPD
				ldrb	r1, [r12, r0, lsr #21]
				orr		r1, r1, #UPDATE_TRAM
				strb	r1, [r12, r0, lsr #21]
				strb	r1, [r2, #(IOCORE2CRTC + CRTCE_SCRNFLASH)]

				tst		r3, #TRAMATR_Xx2
				addeq	pc, lr, #4

				add		r0, r0, #(1 << 21)
				ldrb	r1, [r12, r0, lsr #21]
				orr		r1, r1, #UPDATE_TRAM
				strb	r1, [r12, r0, lsr #21]
				add		pc, lr, #4

	.align	2
	.pool

	.end

