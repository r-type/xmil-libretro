
	.include	"../z80aadv/z80a.inc"

	.global		z80mem_read8
	.global		z80mem_write8
	.global		_fetch16odd
	.global		_mem_read16
	.global		_mr16_oddr12
	.global		_mem_write16_ret
	.global		_mem_write16
	.global		_mw16_odd

	.section	.iwram.text
	.code	32
	.align	0


z80mem_read8:	mov		r2, #Z80CORE
				tst		r0, #0x8000
				movne	r3, #MAINMEM
				ldreq	r3, [r2, #_CPU_MEMREAD]
				ldrb	r0, [r3, r0]
				mov		pc, lr

	.if 1
z80mem_write8:	mov		r3, #MAINMEM
				strb	r1, [r3, r0]
				mov		pc, lr
	.else
z80mem_write8:	mov		r2, #Z80CORE
				tst		r0, #0x8000
				movne	r3, #MAINMEM
				ldreq	r3, [r2, #CPU_MEMWRITE]
				strb	r1, [r3, r0]
				mov		pc, lr
	.endif


@ ---- .S‚©‚çŒÄ‚Î‚ê‚é

_fetch16odd:	adds	r8, r8, #(1 << 16)
				ldrmib	r0, [r5, r8, lsr #16]
				ldrplb	r0, [r10, r8, lsr #16]
				adds	r8, r8, #(1 << 16)
				ldrmib	r1, [r5, r8, lsr #16]
				ldrplb	r1, [r10, r8, lsr #16]
				orr		r0, r0, r1, lsl #8
				add		pc, lr, #16


_mem_read16:	tst		r0, #0x8000
				movne	r12, r5
				moveq	r12, r10
				tst		r0, #1
				bne		_mr16_oddr12
				ldrh	r0, [r12, r0]
				mov		pc, lr
_mr16_oddr12:	add		r2, r0, #1
				ldrb	r0, [r0, r12]
				movs	r1, r2, lsl #17
				beq		_mr16b
				ldrb	r1, [r2, r12]
				orr		r0, r0, r1, lsl #8
				mov		pc, lr
_mr16b:			tst		r2, #0x8000
				movne	r12, r5
				moveq	r12, r10
				bic		r2, r2, #0x10000
				ldrb	r1, [r12, r2]
				orr		r0, r0, r1, lsl #8
				mov		pc, lr


_mem_write16_ret:
				mov		lr, r11
_mem_write16:	tst		r0, #1
				streqh	r1, [r0, r5]
				moveq	pc, lr
_mw16_odd:		add		r12, r0, #1
				strb	r1, [r0, r5]
				bic		r12, r12, #0x10000
				mov		r1, r1, lsr #8
				strb	r1, [r12, r5]
				mov		pc, lr

	.end

