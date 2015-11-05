
	.include	"./agb/ramptr.inc"
	.include	"../z80aadv/z80a.inc"
	.include	"../z80aadv/z80dma.inc"

	.global	z80dmap
	.global	z80dmap2
	.extern	iocore_inp
	.extern	fastioout
	.extern	ievent_setbit

rDMA		.req	r4
rDMACA		.req	r8
rDMACB		.req	r9

	.section	.text
	.code	32
	.align	0

z80dmap:		stmdb	sp!, {r4 - r11, lr}
				mov		r4, #Z80CORE
				mov		r5, #MAINMEM
				add		r2, r4, #_CPU_REMCLOCK
				ldr		r1, 00f
				ldmia	r2, {r6 - r10}
				mov		lr, pc
				bx		r1
				ldmia	sp!, {r4 - r11, pc}

00:				.word	z80dmap2

	.section	.iwram.text
	.code	32
	.align	0

z80dmap2:		ldr		r11, [rDMA, #(Z80CORE2DMA + DMA_FLAG)]
				tst		r11, #DMAF_WORKING
				moveq	pc, lr

				stmdb	sp!, {r8, r9, lr}

				ldr		rDMACA, [rDMA, #(Z80CORE2DMA + DMA_CNTA)]
				ldr		rDMACB, [rDMA, #(Z80CORE2DMA + DMA_CNTB)]

00:				sub		rREMCLOCK, rREMCLOCK, #6
				tst		r11, #DMAF_INCREMENT
				orr		r11, r11, #DMAF_INCREMENT
				beq		20f

				tst		rDMACA, #0x20
				bne		11f
				tst		rDMACA, #0x10
				addne	rDMACA, rDMACA, #(1 << 16)
				subeq	rDMACA, rDMACA, #(1 << 16)
				str		rDMACA, [rDMA, #(Z80CORE2DMA + DMA_CNTA)]

11:				tst		rDMACB, #0x20
				bne		20f
				tst		rDMACB, #0x10
				addne	rDMACB, rDMACB, #(1 << 16)
				subeq	rDMACB, rDMACB, #(1 << 16)
				str		rDMACB, [rDMA, #(Z80CORE2DMA + DMA_CNTB)]

20:				tst		r11, #DMAF_DIR
				movne	r0, rDMACA
				moveq	r0, rDMACB
				tst		r0, #0x08
				bne		21f
				teq		r0, #0
				ldrmib	r1, [r5, r0, lsr #16]
				ldrplb	r1, [r10, r0, lsr #16]

30:				tst		r11, #DMAF_CMDM
				beq		40f

				ldrh	r0, [rDMA, #(Z80CORE2DMA + DMA_MASKBYTE)]
				eor		r12, r0, r1, lsl #8
				bics	r12, r0, r12, lsr #8
				biceq	r11, r11, #(DMAF_WORKING + DMAF_MACH)

40:				tst		r11, #DMAF_CMDO
				beq		50f

				tst		r11, #DMAF_DIR
				movne	r0, rDMACB
				moveq	r0, rDMACA
				tst		r0, #0x08
				bne		41f
				strb	r1, [r5, r0, lsr #16]

50:				ldr		r0, [rDMA, #(Z80CORE2DMA + DMA_BYTEL)]
				movs	r12, r0, lsl #16
				cmpne	r0, r12
				bichi	r11, r11, #(DMAF_WORKING + DMAF_ENDB)
				adds	r0, r0, #(1 << 16)
				biccs	r11, r11, #(DMAF_WORKING + DMAF_ENDB)
				str		r0, [rDMA, #(Z80CORE2DMA + DMA_BYTEL)]
				tst		r11, #DMAF_WORKING
				beq		60f
				tst		r11, #(0xff << 16)
				bne		00b
				strb	r11, [rDMA, #(Z80CORE2DMA + DMA_FLAG)]
				ldmia	sp!, {r8, r9, pc}

60:				strb	r11, [rDMA, #(Z80CORE2DMA + DMA_FLAG)]
				tst		r11, #(0xff << 24)
				ldmeqia	sp!, {r8, r9, pc}
				str		rREMCLOCK, [rZ80CORE, #_CPU_REMCLOCK]
				mov		r0, #(1 << 1)			; @ IEVENT_DMA
				mov		lr, pc
				ldr		pc, 61f
				ldr		rREMCLOCK, [rZ80CORE, #_CPU_REMCLOCK]
				ldmia	sp!, {r8, r9, pc}
61:				.word	ievent_setbit


21:				mov		r0, r0, lsr #16
				strb	r11, [rDMA, #(Z80CORE2DMA + DMA_FLAG)]
				Z80INP
				ldr		r11, [rDMA, #(Z80CORE2DMA + DMA_FLAG)]
				mov		r1, r0
				b		30b

41:				mov		r0, r0, lsr #16
				strb	r11, [rDMA, #(Z80CORE2DMA + DMA_FLAG)]
				mov		r11, pc
				b		fastioout
				ldr		r11, [rDMA, #(Z80CORE2DMA + DMA_FLAG)]
				b		50b

	.end

