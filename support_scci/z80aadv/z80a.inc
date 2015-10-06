
MAINMEM				=		0x02000000
Z80CORE				=		0x03000000

C_FLAG				=		0x01
N_FLAG				=		0x02
V_FLAG				=		0x04
H_FLAG				=		0x10
Z_FLAG				=		0x40
S_FLAG				=		0x80

IFF_IFLAG			=		0
IFF_NMI				=		1
IFF_HALT			=		2

CPU_BC				=		0
CPU_DE				=		2
CPU_HL				=		4
CPU_IX				=		6
CPU_IY				=		8
CPU_AF2				=		10
CPU_BC2				=		12
CPU_DE2				=		14
CPU_HL2				=		16
CPU_R2				=		18
CPU_IRQ				=		20
CPU_REQIRQ			=		24
CPU_CLOCK			=		32
CPU_BASECLOCK		=		36
_CPU_REMCLOCK		=		40					; @ shadow
_CPU_AF				=		44					; @ shadow
_CPU_SP				=		48					; @ shadow
; @ CPU_PC			=		50
_CPU_IFF			=		52					; @ shadow
; @ CPU_I			=		53
; @ CPU_IM			=		54
; @ CPU_R1			=		55
CPU_S_SIZE			=		56

CPU_C				=		0
CPU_B				=		1
CPU_E				=		2
CPU_D				=		3
CPU_L				=		4
CPU_H				=		5
CPU_IXL				=		6
CPU_IXH				=		7
CPU_IYL				=		8
CPU_IYH				=		9
; @ CPU_F			=		40
; @ CPU_A			=		41

_CPU_MEMREAD		=		CPU_S_SIZE + 0		; @ shadow
; @ CPU_MEMWRITE	=		CPU_S_SIZE + 4		; @ always mainmem
; @ CPU_SIZE		=		CPU_S_SIZE + 8

CPU_SZPFLAG			=		0x080
CPU_INCFLAG			=		0x180
CPU_DECFLAG			=		0x280


rZ80CORE	.req	r4
rREMCLOCK	.req	r6
rAF			.req	r7
rPCSP		.req	r8
rRIII		.req	r9


.macro	Z80WORK	c
			sub		rREMCLOCK, rREMCLOCK, \c
.endm

.macro	MEMRD8	reg, addr
			tst		\addr, #0x8000
			ldrneb	\reg, [r5, \addr]
			ldreqb	\reg, [r10, \addr]
.endm

.macro	MEMWR8	addr, reg
			strb	\reg, [r5, \addr]
.endm

.macro	MEMWRP	ptr, addr
			add		\ptr, r5, \addr
.endm

.macro	GETPC8
			adds	r8, r8, #(1 << 16)
			ldrmib	r0, [r5, r8, lsr #16]
			ldrplb	r0, [r10, r8, lsr #16]
.endm

.macro	GETPC8S
			adds	r8, r8, #(1 << 16)
			addmi	lr, r5, r8, lsr #16
			addpl	lr, r10, r8, lsr #16
			ldrsb	r0, [lr]
.endm

.macro	GETPC16
			tst		r8, #(1 << 16)
			bleq	_fetch16odd
			adds	r8, r8, #(2 << 16)
			addmi	lr, r5, r8, lsr #16
			addpl	lr, r10, r8, lsr #16
			ldrh	r0, [lr, #-1]
.endm


.macro	Z80INP
			bl		_fastioinp
			ldr		rREMCLOCK, [r4, #_CPU_REMCLOCK]
.endm

