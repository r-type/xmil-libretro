
	INCLUDE	z80a.inc

	EXPORT	z80mem_read8
	EXPORT	z80mem_write8
	EXPORT	mem_read16
	EXPORT	mem_write16

	IMPORT	z80core

	AREA	.text, CODE, READONLY


z8a_r7			dcd		z80core + CPU_SIZE


z80mem_read8	ldr		r2, z8a_r7
				tst		r0, #&8000
				ldrne	r3, [r2, #CPU_MEMBASE]
				ldreq	r3, [r2, #CPU_MEMREAD]
				ldrb	r0, [r3, r0]
				mov		pc, lr

z80mem_write8	ldr		r2, z8a_r7
				tst		r0, #&8000
				ldrne	r3, [r2, #CPU_MEMBASE]
				ldreq	r3, [r2, #CPU_MEMWRITE]
				strb	r1, [r3, r0]
				mov		pc, lr


; ---- .S‚©‚çŒÄ‚Î‚ê‚é

mem_read16		tst		r0, #&8000
				ldrne	r12, [r7, #CPU_MEMBASE]
				ldreq	r12, [r7, #CPU_MEMREAD]
				tst		r0, #1
				bne		_mr16a
				ldrh	r0, [r12, r0]
				mov		pc, lr
_mr16a			add		r2, r0, #1
				ldrb	r0, [r12, r0]
				movs	r3, r2, lsl #17
				beq		_mr16b
				ldrb	r1, [r12, r2]
				orr		r0, r0, r1, lsl #8
				mov		pc, lr
_mr16b			tst		r2, #&8000
				ldrne	r12, [r7, #CPU_MEMBASE]
				ldreq	r12, [r7, #CPU_MEMREAD]
				bic		r2, r2, #&10000
				ldrb	r1, [r12, r2]
				orr		r0, r0, r1, lsl #8
				mov		pc, lr


mem_write16		tst		r0, #&8000
				ldrne	r12, [r7, #CPU_MEMBASE]
				ldreq	r12, [r7, #CPU_MEMWRITE]
				tst		r0, #1
				bne		_mw16a
				strh	r1, [r12, r0]
				mov		pc, lr
_mw16a			add		r2, r0, #1
				strb	r1, [r12, r0]
				movs	r3, r2, lsl #17
				mov		r1, r1, lsr #8
				beq		_mw16b
				strb	r1, [r12, r2]
				mov		pc, lr
_mw16b			tst		r2, #&8000
				ldrne	r12, [r7, #CPU_MEMBASE]
				ldreq	r12, [r7, #CPU_MEMWRITE]
				bic		r2, r2, #&10000
				strb	r1, [r12, r2]
				mov		pc, lr

	END

