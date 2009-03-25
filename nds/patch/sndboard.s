
	.include	"./nds/ramptr.inc"


PSG_REG			= 0
PSG_CLOCK		= 16
PSG_ADDR		= 20
PSG_UPDATEREG	= 21
PSG_UPDATEENV	= 22


; @	.global	sndboard_reset
	.global sndboard_psgreg
	.global sndboard_psgdat
	.global	sndboard_psgsta


	.section	.text
	.code	32
	.align	2


	.if 0
sndboard_reset:
				mov		r0, #IWRAM_PTR
				ldr		r1, =nds9psg_reset
				add		r0, r0, #IWRAM_NDS9PSG
				bx		r1

	.align	2
	.pool
	.endif

	.section	.itcm
	.code	32
	.align	2


sndboard_psgreg:
				strb	r1, [r4, #(IWRAM_NDS9PSG + PSG_ADDR)]
				add		pc, lr, #4


sndboard_psgdat:
				ldrb	r12, [r4, #(IWRAM_NDS9PSG + PSG_ADDR)]
				add		r3, r4, #(IWRAM_NDS9PSG + PSG_REG)
				cmp		r12, #0x10
				strccb	r1, [r3, r12]
				mov		r2, #1
				cmp		r12, #13
				streqb	r2, [r4, #(IWRAM_NDS9PSG + PSG_UPDATEENV)]
				strccb	r2, [r4, #(IWRAM_NDS9PSG + PSG_UPDATEREG)]
				add		pc, lr, #4

sndboard_psgsta:
				ldrb	r12, [r4, #(IWRAM_NDS9PSG + PSG_ADDR)]
				add		r3, r4, #(IWRAM_NDS9PSG + PSG_REG)
				cmp		r12, #0x10
				movcs	r0, #0xff
				ldrccb	r0, [r3, r12]
				add		pc, lr, #4

	.end

