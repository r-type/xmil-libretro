
	.include	"./nds/ramptr.inc"
	.include	"./patch/iocore.inc"
	.include	"./vram/makescrn.inc"

	.global	dummy_inp
	.global	dummy_out

	.section	.itcm
	.code	32
	.align	2

dummy_inp:		mov		r0, #0xff
dummy_out:
.if INTERWORK
				bx		lr
.else
				mov		pc, lr
.endif

	.if 0

	.global	iocore_out
	.global	iocore_inp

	.section	.text
	.code	32
	.align	2

iocore_out:		ldr		r2, _iocore
				and		r12, r0, #0xff00
				ldrb	r3, [r2, #IOCORE_MODE]
				cmp		r3, #0
				bne		20f
				subs	r3, r0, #0x4000
				bcs		10f
				ldrcc	r12, [r2, r12, lsr #6]
				str		lr, [sp, #-4]!
				mov		lr, pc
				mov		pc, r12
				mov		r12, r12
				ldr		pc, [sp], #4

10:				ldr		r0, [r2, #(IOCORE2CRTC + CRTCE_GRAMACC)]
				ldr		r12, [r2, #(IOCORE2CRTC + _CRTCE_UPDATEBIT)]
				strb	r1, [r0, r3]
				and		r3, r3, r12, lsr #16
				add		r0, r2, #IOCORE2VRAMUPD
				ldrb	r1, [r0, r3]
				orr		r1, r1, r12
				strb	r12, [r2, #(IOCORE2CRTC + CRTCE_SCRNFLASH)]
				strb	r1, [r0, r3]
				mov		pc, lr
20:				bic		r3, r0, #0xc000
				ldr		r12, [r2, #(IOCORE2CRTC + CRTCE_GRAMACC)]
				mov		r0, r0, lsr #14
				cmp		r0, #1
				strneb	r1, [r12, r3]
				add		r3, r3, #0x4000
				cmp		r0, #2
				strneb	r1, [r3, r12]
				add		r3, r3, #0x4000
				cmp		r0, #3
				strneb	r1, [r3, r12]
				ldr		r1, [r2, #(IOCORE2CRTC + _CRTCE_UPDATEBIT)]
				and		r12, r3, r1, lsr #16
				add		r3, r2, #IOCORE2VRAMUPD
				strb	r1, [r2, #(IOCORE2CRTC + CRTCE_SCRNFLASH)]
				ldrb	r0, [r3, r12]
				orr		r0, r0, r1
				strb	r0, [r3, r12]
				mov		pc, lr

_iocore		.word	iocore - 0x100

iocore_inp:		ldr		r2, _iocore
				and		r3, r0, #0xff00
				strb	r3, [r2, #IOCORE_MODE]
				subs	r3, r3, #0x4000
				bcs		30f
				ldr		r12, [r2, r3, asr #6]
				str		lr, [sp, #-4]!
				mov		lr, pc
				mov		pc, r12
				mov		r12, r12
				ldr		pc, [sp], #4
30:				sub		r12, r0, #0x4000
				ldr		r1, [r2, #(IOCORE2CRTC + CRTCE_GRAMACC)]
				ldrb	r0, [r1, r12]
				mov		pc, lr
	.endif


	.end

