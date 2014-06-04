
	.include	"./nds/ramptr.inc"
	.include	"./patch/iocore.inc"

SCPU_INPUT			= 0x00
SCPU_OUTPUT			= 0x08

SUBCPU_TIMER		= 0
SUBCPU_WORK			= 48
SUBCPU_VECT			= 52
SUBCPU_CMTCMD		= 53
SUBCPU_ZERO			= 54
SUBCPU_DATCNT		= 56
SUBCPU_DATPTR		= 57
SUBCPU_CMDCNT		= 58
SUBCPU_CMDPTR		= 59
SUBCPU_KEYCOUNT		= 60
SUBCPU_KEYCOUNTREP	= 64
SUBCPU_MODE			= 68
SUBCPU_KEYDATA		= 69
SUBCPU_INTRCLOCK	= 72
SUBCPU_SIZE			= 76


	.global	subcpu_i
	.global	subcpu_o
	.extern	subcpusetbuffer
	.extern	subcpusendctrl
	.extern	keystat

	.section	.itcm
	.code	32
	.align	2


subcpu_i:		add		r0, r2, #(IWRAM_SUBCPU - IWRAM_IOCORE)
				ldrb	r3, [r2, #IOCORE_PPIB]
				ldrh	r1, [r0, #SUBCPU_DATCNT]
				add		r12, r0, r1, lsr #8
				and		r1, r1, #0xff
				subs	r1, r1, #1
				bcc		110f
				strb	r1, [r0, #SUBCPU_DATCNT]
				orreq	r3, r3, #0x20
				orrne	r3, r3, #0x40
				ldrb	r0, [r12, r1]
				strb	r3, [r2, #IOCORE_PPIB]
				add		pc, lr, #4

110:			orr		r3, r3, #0x20
				strb	r3, [r2, #IOCORE_PPIB]
				stmdb	sp!, {r12, lr}
				ldrb	r1, [r0, #SUBCPU_MODE]
				bl		subcpusetbuffer2
				ldmia	sp!, {r12, lr}
				ldrb	r0, [r12]
				mov		pc, lr


subcpu_o:		add		r0, r2, #(IWRAM_SUBCPU - IWRAM_IOCORE)
				ldrb	r3, [r2, #IOCORE_PPIB]
				tst		r3, #0x40
				addne	pc, lr, #4

				ldrb	r12, [r0, #SUBCPU_CMDCNT]
				subs	r12, r12, #1
				bcs		220f

				sub		r12, r1, #0xd0
				cmp		r12, #0x20
				movcs	r12, #0x10
				strb	r1, [r0, #SUBCPU_MODE]
				add		r12, pc, r12, lsl #1
				ldrh	r12, [r12, #(1001f - 1000f)]
1000:
				bic		r3, r3, #0x60
				tst		r12, #SCPU_OUTPUT
				bne		210f

				orr		r3, r3, #0x40
				str		r12, [r0, #SUBCPU_DATCNT]
				strb	r3, [r2, #IOCORE_PPIB]

subcpusetbuffer2:
				ldr		r2, 200f
				bx		r2

				cmp		r1, #0xe3
.if INTERWORK
				ldrne	r3, 200f
				ldreq	r2, 201f
				bxne	r3
.else
				ldrne	pc, 200f
				ldr		r2, 201f
.endif
				ldr		r3, [r2, #4]
				str		r3, [r0, #SUBCPU_WORK]
				mov		pc, lr

200:			.word	subcpusetbuffer
201:			.word	keystat

210:			mov		r12, r12, lsl #16
				bic		r12, r12, #(SCPU_OUTPUT << 16)
				add		r12, r12, #(SUBCPU_ZERO	<< 8)
				orr		r3, r3, #0x20
				str		r12, [r0, #SUBCPU_DATCNT]
				strb	r3, [r2, #IOCORE_PPIB]
				add		pc, lr, #4

220:			strb	r12, [r0, #SUBCPU_CMDCNT]
				ldrb	r2, [r0, #SUBCPU_CMDPTR]
				add		r12, r12, r0
				strb	r1, [r12, r2]
				addne	pc, lr, #4
.if INTERWORK
				ldr		r1, 221f
				bx		r1
.else
				ldr		pc, 221f
.endif
221:			.word	subcpusendctrl


1001:	.byte	6 + SCPU_OUTPUT,	SUBCPU_TIMER+0*6	; @ d0: timer0 set
		.byte	6 + SCPU_OUTPUT,	SUBCPU_TIMER+1*6	; @ d1: timer1 set
		.byte	6 + SCPU_OUTPUT,	SUBCPU_TIMER+2*6	; @ d2: timer2 set
		.byte	6 + SCPU_OUTPUT,	SUBCPU_TIMER+3*6	; @ d3: timer3 set
		.byte	6 + SCPU_OUTPUT,	SUBCPU_TIMER+4*6	; @ d4: timer4 set
		.byte	6 + SCPU_OUTPUT,	SUBCPU_TIMER+5*6	; @ d5: timer5 set
		.byte	6 + SCPU_OUTPUT,	SUBCPU_TIMER+6*6	; @ d6: timer6 set
		.byte	6 + SCPU_OUTPUT,	SUBCPU_TIMER+7*6	; @ d7: timer7 set
		.byte	6 + SCPU_INPUT,		SUBCPU_TIMER+0*6	; @ d8: timer0 get
		.byte	6 + SCPU_INPUT,		SUBCPU_TIMER+1*6	; @ d9: timer1 get
		.byte	6 + SCPU_INPUT,		SUBCPU_TIMER+2*6	; @ da: timer2 get
		.byte	6 + SCPU_INPUT,		SUBCPU_TIMER+3*6	; @ db: timer3 get
		.byte	6 + SCPU_INPUT,		SUBCPU_TIMER+4*6	; @ dc: timer4 get
		.byte	6 + SCPU_INPUT,		SUBCPU_TIMER+5*6	; @ dd: timer5 get
		.byte	6 + SCPU_INPUT,		SUBCPU_TIMER+6*6	; @ de: timer6 get
		.byte	6 + SCPU_INPUT,		SUBCPU_TIMER+7*6	; @ df: timer7 get
		.byte	0 + SCPU_OUTPUT,	SUBCPU_ZERO			; @ e0:
		.byte	0 + SCPU_OUTPUT,	SUBCPU_ZERO			; @ e1:
		.byte	0 + SCPU_OUTPUT,	SUBCPU_ZERO			; @ e2:
		.byte	3 + SCPU_INPUT,		SUBCPU_WORK			; @ e3: game keys
		.byte	1 + SCPU_OUTPUT,	SUBCPU_VECT			; @ e4: intr vector
		.byte	0 + SCPU_OUTPUT,	SUBCPU_ZERO			; @ e5:
		.byte	2 + SCPU_INPUT,		SUBCPU_WORK			; @ e6: game keys
		.byte	1 + SCPU_OUTPUT,	SUBCPU_CMTCMD		; @ e7: cmt?
		.byte	1 + SCPU_INPUT,		SUBCPU_CMTCMD		; @ e8: cmt?
		.byte	1 + SCPU_OUTPUT,	SUBCPU_WORK			; @ e9: cmt ctrl
		.byte	1 + SCPU_INPUT,		SUBCPU_WORK			; @ ea: cmtctrlstat
		.byte	1 + SCPU_INPUT,		SUBCPU_WORK			; @ eb: cmtctrlstat
		.byte	3 + SCPU_OUTPUT,	SUBCPU_WORK			; @ ec: date set
		.byte	3 + SCPU_INPUT,		SUBCPU_WORK			; @ ed: date get
		.byte	3 + SCPU_OUTPUT,	SUBCPU_WORK			; @ ee: time set
		.byte	3 + SCPU_INPUT,		SUBCPU_WORK			; @ ef: time get

	.end

