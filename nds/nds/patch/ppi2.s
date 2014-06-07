
	.include	"./nds/ramptr.inc"
	.include	"./patch/iocore.inc"

PPI_PORTA		= 0
PPI_PORTB		= 1
PPI_PORTC		= 2
PPI_MODE		= 3
PPI_SIZE		= 4

	.global	ppi_i
	.global	ppi_o

	.section	.itcm
	.code	32
	.align	0


	.if 0
ppi_i:			and		r0, r0, #15
				cmp		r0, #1
				bne		ppi_i_r
	.if 1
				ldrb	r1, [r4, #(IWRAM_PPI + PPI_MODE)]
				tst		r1, #0x02
				beq		ppi_i_r
	.endif

				ldrb	r0, [r4, #(IWRAM_IOCORE + IOCORE_PPIB)]
				orr		r1, r0, #0x01
				bic		r1, r1, #0x40
				strb	r1, [r4, #(IWRAM_IOCORE + IOCORE_PPIB)]
				add		pc, lr, #4

				@ r2Ç… iocoreÇ™ãèÇÈî§
ppi_i_r:		add		r3, r2, #(IWRAM_PPI - IWRAM_IOCORE)
				cmp		r0, #4
				ldrccb	r0, [r3, r0]
				movcs	r0, #0xff
				add		pc, lr, #4
	.endif



ppi_i:			and		r1, r0, #15
				cmp		r1, #4
				addcc	pc, pc, r1, lsl #3
				b		dummy_inp

0:				ldrb	r0, [r4, #(IWRAM_PPI + PPI_PORTA)]
				add		pc, lr, #4

1:				ldrb	r1, [r4, #(IWRAM_PPI + PPI_MODE)]
				b		11f

2:				ldrb	r0, [r4, #(IWRAM_PPI + PPI_PORTC)]
				add		pc, lr, #4

3:				ldrb	r0, [r4, #(IWRAM_PPI + PPI_MODE)]
				add		pc, lr, #4

11:				tst		r1, #0x02
				ldreqb	r0, [r4, #(IWRAM_PPI + PPI_PORTB)]
				addeq	pc, lr, #4

				ldrb	r0, [r4, #(IWRAM_IOCORE + IOCORE_PPIB)]
				orr		r1, r0, #0x01
				bic		r1, r1, #0x40
				strb	r1, [r4, #(IWRAM_IOCORE + IOCORE_PPIB)]
				add		pc, lr, #4


ppi_o:			and		r0, r0, #15
				cmp		r0, #4
				addcc	pc, pc, r0, lsl #3
				add		pc, lr, #4

0:				strb	r1, [r4, #(IWRAM_PPI + PPI_PORTA)]
				add		pc, lr, #4

1:				strb	r1, [r4, #(IWRAM_PPI + PPI_PORTB)]
				add		pc, lr, #4

2:				b		31f
				nop

3:				tst		r1, #0x80
				strneb	r1, [r4, #(IWRAM_PPI + PPI_MODE)]
				addne	pc, lr, #4

				tst		r1, #1
				mov		r3, r1, lsr #1
				and		r3, r3, #7
				mov		r12, #1
				ldrb	r0, [r4, #(IWRAM_PPI + PPI_PORTC)]
				orrne	r1, r0, r12, lsl r3
				biceq	r1, r0, r12, lsl r3


	.if 0
				strb	r1, [r4, #(IWRAM_PPI + PPI_PORTC)]


				eor		r3, r1, r0

tst		r1, #0x20
tstne	r0, #0x20

0: 0 0 | 0 0
1: 1 0 | 1 0
0: 0 1 | 1 1
0: 1 1 | 0 1

				tst		r3, #0x40
				addeq	pc, lr, #4
				and		r0, r1, #0x40
	.endif

31:
.if INTERWORK
				ldr		r12, =ppi_setportc
				add		r0, r2, #(IWRAM_PPI - IWRAM_IOCORE)
				bx		r12
.else
				add		r0, r2, #(IWRAM_PPI - IWRAM_IOCORE)
				ldr		pc, =ppi_setportc
.endif

	.pool

	.end

