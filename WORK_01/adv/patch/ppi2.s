
	.include	"../z80aadv/z80a.inc"
	.include	"./patch/iocore.inc"

PPI_PORTA		= 0
PPI_PORTB		= 1
PPI_PORTC		= 2
PPI_MODE		= 3
PPI_SIZE		= 4

	.global	ppi
	.global	ppi_i

	.section	.iwram.text
	.code	32
	.align	0


ppi:	.fill   PPI_SIZE,1,0


ppi_i:			and		r0, r0, #15
				cmp		r0, #1
				bne		ppi_i_r
	.if 0
				ldrb	r1, ppi+PPI_MODE
				tst		r1, #0x02
				beq		ppi_i_r
	.endif

				@ r2Ç… iocoreÇ™ãèÇÈî§
				ldrb	r0, [r2, #IOCORE_PPIB]
				orr		r1, r0, #0x01
				bic		r1, r1, #0x40
				strb	r1, [r2, #IOCORE_PPIB]
		; @		sub		r6, r6, #32				; @ !
				add		pc, lr, #4

ppi_i_r:		adr		r3, ppi
				cmp		r0, #4
				ldrccb	r0, [r3, r0]
				movcs	r0, #0xff
				add		pc, lr, #4

	.end

