
	.global	advpsg
	.global	sndboard_psgreg
	.global	sndboard_psgdat
	.global	sndboard_psgsta
	.global	sndboard_initialize
	.global	sndboard_deinitialize
	.global	sndboard_reset
	.extern	advpsg_setreg
	.extern	advpsg_reset

ADVPSG_REG			= 0x00
ADVPSG_ADDR			= 0x10
ADVPSG_ENVBMP		= 0x11
ADVPSG_CH3BANK		= 0x12
ADVPSG_VOL			= 0x14
ADVPSG_FREQ			= 0x18
ADVPSG_LASTCLOCK	= 0x20
ADVPSG_ENVCNT		= 0x24
ADVPSG_ENVMAX		= 0x28
ADVPSG_MIXER		= 0x2c
ADVPSG_ENVMODE		= 0x2d
ADVPSG_ENVVOL		= 0x2e
ADVPSG_ENVVOLCNT	= 0x2f
ADVPSG_SIZE			= 0x30

	.section	.iwram.text
	.code	32
	.align	0


sndboard_psgreg:
				strb	r1, advpsg + ADVPSG_ADDR
				add		pc, lr, #4

sndboard_psgdat:
				adr		r0, advpsg
				mov		r2, r1
				ldrb	r1, advpsg + ADVPSG_ADDR
				ldr		pc, 100f
100:			.word	advpsg_setreg

sndboard_psgsta:
				ldrb	r1, advpsg + ADVPSG_ADDR
				cmp		r1, #0x10
				movcs	r0, #0xff
				ldrccb	r0, [pc, r1]
				add		pc, lr, #4

advpsg:			.fill	ADVPSG_SIZE, 1, 0


	.section	.text
	.code	32
	.align	0

sndboard_initialize:
sndboard_deinitialize:
				mov		pc, lr

sndboard_reset:
				ldr		r0, 200f
				b		advpsg_reset

200:			.word	advpsg

	.end

