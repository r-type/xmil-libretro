
@ maketxt8_make / makepcg8_make ÉeÉXÉg

	.include	"./vram/makescrn.inc"

	.global	maketxt8_make
	.global	makepcg8_make
	.extern	makescrn
	.extern	maketxt8fn

	.section	.itcm
	.code	32
	.align	0


_makescrn:		.word	makescrn
_maketxt8fn:	.word	maketxt8fn


maketxt8_make:	stmdb	sp!, {r4 - r7, lr}
				ldr		r5, _maketxt8fn
				ldr		r4, _makescrn
				tst		r3, #0x10
				movne	r7, #0x07
				moveq	r7, #0x0f
				ldr		r6, [r5, r3, lsl #2]
				ldr		r12, [sp, #20]
				add		r3, r5, #(32*4)
				and		r12, r12, #15
				mov		r5, r0					@ for atr
				ldr		r12, [r3, r12, lsl #2]
				mov		lr, pc
				bx		r6
.if INTERWORK
				ldmia	sp!, {r4 - r7, lr}
				bx		lr
.else
				ldmia	sp!, {r4 - r7, pc}
.endif


makepcg8_make:	stmdb	sp!, {r4 - r8, lr}
				ldr		r4, _makescrn
				ldr		lr, _maketxt8fn
				tst		r3, #0x10
				movne	r7, #0x07
				moveq	r7, #0x0f
				ldr		lr, [lr, r3, lsl #2]	@ proc!

				ldr		r8, [sp, #24]			@ atr
				mov		r5, r0					@ dst

				tst		r8, #1
				adr		r12, 1f
				bxne	lr
				b		_cls8

1:				add		r5, r0, #MAKETEXT_ROW
				add		r1, r1, #MAKETEXT_ROW
				add		r2, r2, #0x800
				tst		r8, #2
				adr		r12, 2f
				bxne	lr
				b		_cls8

2:				add		r5, r0, #(MAKETEXT_ROW * 2)
				add		r1, r1, #MAKETEXT_ROW
				add		r2, r2, #0x800
				tst		r8, #4
				adr		r12, 3f
				bxne	lr
				b		_cls8
3:
				@ îΩì]Å`

.if INTERWORK
				ldmia	sp!, {r4 - r8, lr}
				bx		lr
.else
				ldmia	sp!, {r4 - r8, pc}
.endif

_cls8:			mov		r3, #0
1:				str		r3, [r5]
				add		r5, r5, #4
				cmp		r5, r1
				bcc		1b
				mov		pc, r12

	.end

