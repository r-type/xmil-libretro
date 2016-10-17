
	.include	"../z80ands/z80a.inc"


CTCCHS_BASECLOCK	= 0x00
CTCCHS_VECTOR		= 0x04
CTCCHS_INTR			= 0x05
CTCCHS_IRQ			= 0x06
CTCCHS_NUM			= 0x07
CTCCHS_CMD			= 0x08
CTCCHS_SCALE		= 0x0c
CTCCHS_BASECNT		= 0x10
CTCCHS_COUNT		= 0x14
CTCCHS_COUNTMAX		= 0x24
CTCCHS_RANGE		= 0x34
CTCCHE_NEVENTID		= 0x44
CTCCHE_IEVENTBIT	= 0x48
CTCCH_SIZE			= 0x4c


	.section	.text
	.code	32
	.align	2


		; @ ch3にチェインしてるかどうかでルーチン変える？

		.global	ctcwork

ctcwork:		stmdb	sp!, {r4 - r5, lr}
				mov		r4, r0
				mov		r12, #Z80CORE
				ldr		r1, [r12, #CPU_CLOCK]
				ldr		r2, [r12, #CPU_BASECLOCK]
				ldr		r3, [r12, #_CPU_REMCLOCK]
				add		r5, r1, r2
				sub		r5, r5, r3
				ldr		r1, [r4, #CTCCHS_BASECLOCK]
				rsb		r5, r1, r5				; @ step
				mov		r5, r5, lsr #1
				add		r1, r1, r5, lsl #1
				str		r1, [r4, #CTCCHS_BASECLOCK]

				ldr		r12, [r4, #CTCCHS_CMD]
				mov		r2, #0					; @ pulse3

				; @ 0
				tst		r12, #(0x02 << 0)
				bic		r12, r12, #0x0f			; @ intr
				bne		30f
				ldr		r0, [r4, #(CTCCHS_COUNT + 0*4)]
				subs	r0, r0, r5
				ble		01f
02:				str		r0, [r4, #(CTCCHS_COUNT + 0*4)]

30:				tst		r12, #(0x02 << 24)
				bne		10f
				tst		r12, #(0x40 << 24)
				moveq	r2, r5
				ldr		r0, [r4, #(CTCCHS_COUNT + 3*4)]
				subs	r0, r0, r2
				ble		31f
32:				str		r0, [r4, #(CTCCHS_COUNT + 3*4)]

10:				tst		r12, #(0x02 << 8)
				bne		20f
				ldr		r0, [r4, #(CTCCHS_COUNT + 1*4)]
				subs	r0, r0, r5
				ble		11f
12:				str		r0, [r4, #(CTCCHS_COUNT + 1*4)]

20:				tst		r12, #(0x02 << 16)
				bne		40f
				ldr		r0, [r4, #(CTCCHS_COUNT + 2*4)]
				subs	r0, r0, r5
				ble		21f
22:				str		r0, [r4, #(CTCCHS_COUNT + 2*4)]

40:				and		r0, r12, #0x0f
.if INTERWORK
				ldmia	sp!, {r4 - r5, lr}
				bx		lr
.else
				ldmia	sp!, {r4 - r5, pc}
.endif


01:				tst		r12, #(0x80 << 0)
				orrne	r12, r12, #(1 << 0)
				ldr		r1, [r4, #(CTCCHS_COUNTMAX + 0*4)]
				mov		r2, #1
				adds	r0, r1, r0
				bgt		02b
				rsb		r0, r0, #0
				bl		__udiv
				add		r2, r2, #1
				add		r0, r0, r1
				b		02b

31:				tst		r12, #(0x80 << 24)
				orrne	r12, r12, #(1 << 3)
				ldr		r1, [r4, #(CTCCHS_COUNTMAX + 3*4)]
				adds	r0, r1, r0
				bgt		32b
				rsb		r0, r0, #0
				bl		__udiv
				add		r0, r1, r0
				b		32b

11:				tst		r12, #(0x80 << 8)
				orrne	r12, r12, #(1 << 1)
				ldr		r1, [r4, #(CTCCHS_COUNTMAX + 1*4)]
				adds	r0, r1, r0
				bgt		12b
				rsb		r0, r0, #0
				bl		__udiv
				add		r0, r1, r0
				b		12b

21:				tst		r12, #(0x80 << 16)
				orrne	r12, r12, #(1 << 2)
				ldr		r1, [r4, #(CTCCHS_COUNTMAX + 2*4)]
				adds	r0, r1, r0
				bgt		22b
				rsb		r0, r0, #0
				bl		__udiv
				add		r0, r1, r0
				b		22b


			; @ 割り算～  unsigned r0 / unsigned r1 = r2*r1+r0 (r3==r1)
__udiv:			movs	r3, r1
				beq		__udiv
				mov		r2, #0
1:				cmp		r3, r0, lsr #2
				movls	r3, r3, lsl #2
				bcc		1b
				cmp		r3, r0, lsr #1
				bhi		3f
2:				movhi	r3, r3, lsr #2
				cmp		r0, r3, lsl #1
				adc		r2, r2, r2
				subcs	r0, r0, r3, lsl #1
3:				cmp		r0, r3
				adc		r2, r2, r2
				subcs	r0, r0, r3
				cmp		r3, r1
				bne		2b
				mov		pc, lr

	.end

