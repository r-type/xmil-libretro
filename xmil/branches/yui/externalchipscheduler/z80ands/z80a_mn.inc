
.macro	sLD8	dst, src
			ldrb	r0, [r4, \src]
			strb	r0, [r4, \dst]
			bx		r11
.endm

.macro	sLD8A	dst
			lsr		r0, rAF, #8
			strb	r0, [r4, \dst]
			bx		r11
.endm

.macro	LDA8	src
			ldrb	r0, [r4, \src]
			and		rAF, rAF, #0xff
			orr		rAF, rAF, r0, lsl #8
			mov		pc, r11
.endm

.macro	LDAx	src
			ldrh	r0, [r4, \src]
			and		rAF, rAF, #0xff
			MEMRD8	r0, r0
			add		rAF, rAF, r0, lsl #8
			mov		pc, r11
.endm

.macro	LD8x	dst, src
			ldrh	r0, [r4, \src]
			MEMRD8	r1, r0
			strb	r1, [r4, \dst]
			mov		pc, r11
.endm

.macro	sLDxA	dst
			ldrh	r0, [r4, \dst]
			lsr		r1, rAF, #8
			MEMWR8	r0, r1
			bx		r11
.endm

.macro	sLDx8	dst, src
			ldrh	r0, [r4, \dst]
			ldrb	r1, [r4, \src]
			MEMWR8	r0, r1
			bx		r11
.endm

.macro	LD8b	dst
			GETPC8
			strb	r0, [r4, \dst]
			mov		pc, r11
.endm

.macro	LD16w 	dst
			GETPC16
			strh	r0, [r4, \dst]
			mov		pc, r11
.endm

.macro	LDx16	src
			GETPC16
			ldrh	r1, [r4, \src]
			b		_mem_write16_ret
.endm

.macro	LD16x	dst
			GETPC16
			bl		_mem_read16
			strh	r0, [r4, \dst]
			mov		pc, r11
.endm


.macro	MINC8	dst
			add		r0, r4, #CPU_INCFLAG
			ldrb	r1, [r4, \dst]
			bic		rAF, rAF, #(0xff - C_FLAG)
			ldrb	r2, [r1, r0]
			add		r1, r1, #1
			orr		rAF, r2, rAF
			strb	r1, [r4, \dst]
			mov		pc, r11
.endm

.macro	MINCX8	addr
			MEMRD8	r0, \addr
			add		r12, r4, #CPU_INCFLAG
			bic		rAF, rAF, #(0xff - C_FLAG)
			ldrb	r1, [r12, r0]
			add		r0, r0, #1
			orr		rAF, r1, rAF
			MEMWR8	\addr, r0
			mov		pc, r11
.endm

.macro	MDEC8	dst
			add		r0, r4, #CPU_DECFLAG
			ldrb	r1, [r4, \dst]
			bic		rAF, rAF, #(0xff - C_FLAG)
			ldrb	r2, [r1, r0]
			sub		r1, r1, #1
			orr		rAF, r2, rAF
			strb	r1, [r4, \dst]
			mov		pc, r11
.endm

.macro	MDECX8	addr
			MEMRD8	r0, \addr
			add		r12, r4, #CPU_DECFLAG
			bic		rAF, rAF, #(0xff - C_FLAG)
			ldrb	r1, [r12, r0]
			sub		r0, r0, #1
			orr		rAF, r1, rAF
			MEMWR8	\addr, r0
			mov		pc, r11
.endm


.macro	sMINC16	dst
			ldrh	r0, [r4, \dst]
			add		r0, #1
			strh	r0, [r4, \dst]
			bx		r11
.endm

.macro	sMDEC16	dst
			ldrh	r0, [r4, \dst]
			sub		r0, #1
			strh	r0, [r4, \dst]
			bx		r11
.endm


.macro	MADD16	dst, src
			ldrh	r1, [r4, \src]
			ldrh	r0, [r4, \dst]
			bic		rAF, rAF, #(0xff - (S_FLAG + Z_FLAG + V_FLAG))
			add		r2, r0, r1
			eor		r1, r0, r1
			add		rAF, rAF, r2, lsr #16
			eor		r1, r1, r2
			and		r1, r1, #(H_FLAG << 8)
			strh	r2, [r4, \dst]
			orr		rAF, rAF, r1, lsr #8
			mov		pc, r11
.endm

.macro	MADD16D	dst
			ldrh	r0, [r4, \dst]
			bic		rAF, rAF, #(0xff - (S_FLAG + Z_FLAG + V_FLAG))
			and		r1, r0, #(H_FLAG << 8)
			add		rAF, rAF, r0, lsr #15
			add		r2, r0, r0
			add		rAF, rAF, r1, lsr #8
			strh	r2, [r4, \dst]
			mov		pc, r11
.endm

.macro	MADDSP	dst
			ldrh	r0, [r4, \dst]
			mov		r1, r8, lsl #16
			bic		rAF, rAF, #(0xff - (S_FLAG + Z_FLAG + V_FLAG))
			add		r2, r0, r1, lsr #16
			eor		r12, r0, r1, lsr #16
			add		rAF, rAF, r2, lsr #16
			eor		r12, r12, r2
			and		r12, r12, #(H_FLAG << 8)
			strh	r2, [r4, \dst]
			orr		rAF, rAF, r12, lsr #8
			mov		pc, r11
.endm


.macro	MADD8	reg
			and		r1, rAF, #(0xff << 8)
			mov		r2, \reg, lsl #24
			adds	r3, r2, r1, lsl #16
			orrcs	r3, r3, #(C_FLAG << 16)
			orrvs	r3, r3, #(V_FLAG << 16)
			eor		r0, r2, r1, lsl #16
			orreq	r3, r3, #(Z_FLAG << 16)
			eor		r1, r0, r3
			orrmi	r3, r3, #(S_FLAG << 16)
			and		r2, r1, #(H_FLAG << 24)
			mov		rAF, r3, lsr #16
			orr		rAF, rAF, r2, lsr #24
			mov		pc, r11
.endm

.macro	MADC8	reg
			and		r1, rAF, #C_FLAG
			add		r1, r1, rAF, lsr #8
			add		r1, r1, \reg
			eor		r2, r1, \reg				; @ r ^ b
			eor		r3, r1, rAF, lsr #8			; @ r ^ A
			movs	r1, r1, lsl #24
			eor		r0, r2, rAF, lsr #8			; @ r ^ b ^ A
			orrcs	r1, r1, #(C_FLAG << 16)
			and		r2, r3, r2					; @ (r ^ b) & (r ^ A)
			orreq	r1, r1, #(Z_FLAG << 16)
			and		r0, r0, #H_FLAG
			orrmi	r1, r1, #(S_FLAG << 16)
			and		r2, r2, #(V_FLAG << 5)
			orr		rAF, r0, r1, lsr #16
			orr		rAF, rAF, r2, lsr #5
			mov		pc, r11
.endm

.macro	MSUB8	reg
			and		r1, rAF, #(0xff << 8)
			mov		r2, \reg, lsl #24
			rsbs	r3, r2, r1, lsl #16
			orr		r3, r3, #(N_FLAG << 16)
			orrcc	r3, r3, #(C_FLAG << 16)
			orrvs	r3, r3, #(V_FLAG << 16)
			eor		r0, r2, r1, lsl #16
			orreq	r3, r3, #(Z_FLAG << 16)
			eor		r1, r0, r3
			orrmi	r3, r3, #(S_FLAG << 16)
			and		r2, r1, #(H_FLAG << 24)
			mov		rAF, r3, lsr #16
			orr		rAF, rAF, r2, lsr #24
			mov		pc, r11
.endm

.macro	MSBC8	reg
			and		r1, rAF, #C_FLAG
			rsb		r2, \reg, rAF, lsr #8
			eor		r3, \reg, rAF, lsr #8		; @ A ^ b
			sub		r2, r2, r1
			eor		r0, r2, rAF, lsr #8			; @ A ^ r
			eor		r1, r2, r3					; @ A ^ b ^ r
			movs	r2, r2, lsl #24
			and		r0, r0, r3					; @ (A ^ b) & (A ^ r)
			orr		r2, r2, #(N_FLAG << 16)
			orrcs	r2, r2, #(C_FLAG << 16)
			orreq	r2, r2, #(Z_FLAG << 16)
			and		r1, r1, #H_FLAG
			orrmi	r2, r2, #(S_FLAG << 16)
			and		r0, r0, #(V_FLAG << 5)
			orr		rAF, r1, r2, lsr #16
			orr		rAF, rAF, r0, lsr #5
			mov		pc, r11
.endm

.macro	MAND8	reg
			and		r2, \reg, rAF, lsr #8
			add		r3, r4, #CPU_SZPFLAG
			ldrb	r0, [r3, r2]
			orr		rAF, r0, r2, lsl #8
			mov		pc, r11
.endm

.macro	MXOR8	reg
			eor		r2, \reg, rAF, lsr #8
			add		r3, r4, #CPU_SZPFLAG
			ldrb	r0, [r3, r2]
			orr		rAF, r0, r2, lsl #8
			mov		pc, r11
.endm

.macro	MOR8	reg
			orr		r2, \reg, rAF, lsr #8
			add		r3, r4, #CPU_SZPFLAG
			ldrb	r0, [r3, r2]
			orr		rAF, r0, r2, lsl #8
			mov		pc, r11
.endm

.macro	MCP8	reg
			and		r1, rAF, #(0xff << 8)
			mov		r2, \reg, lsl #24
			rsbs	r3, r2, r1, lsl #16
			orr		r1, r1, #N_FLAG
			eor		r0, r2, rAF, lsl #16
			orrcc	r1, r1, #C_FLAG
			eor		r2, r0, r3
			orrvs	r1, r1, #V_FLAG
			and		r0, r2, #(H_FLAG << 24)
			orreq	r1, r1, #Z_FLAG
			orrmi	r1, r1, #S_FLAG
			orr		rAF, r1, r0, lsr #24
			mov		pc, r11
.endm


.macro	MJP
			mov		r3, r8, lsl #16
			GETPC16
			mov		r3, r3, lsr #16
			orr		r8, r3, r0, lsl #16
			sub		r8, r8, #(1 << 16)
			mov		pc, r11
.endm

.macro	MJPR16	reg
			ldrh	r0, [r4, \reg]
			mov		r1, r8, lsl #16
			mov		r2, r0, lsl #16
			orr		r8, r2, r1, lsr #16
			sub		r8, r8, #(1 << 16)
			mov		pc, r11
.endm

.macro	MJPNFLG	flag
			Z80WORK	#6
			tst		rAF, \flag
			beq		_jp
			add		r8, r8, #(2 << 16)
			mov		pc, r11
.endm

.macro	MJPFLG	flag
			Z80WORK	#6
			tst		rAF, \flag
			bne		_jp
			add		r8, r8, #(2 << 16)
			mov		pc, r11
.endm


.macro	MCALL
			GETPC16
			add		r8, r8, #(1 << 16)
			mov		r2, r8, lsl #16				; @ sp
			mov		r3, r0, lsl #16				; @ new pc
			sub		r2, r2, #(2 << 16)
			mov		r1, r8, lsr #16
			mov		r0, r2, lsr #16
			orr		r8, r3, r2, lsr #16
			sub		r8, r8, #(1 << 16)
			b		_mem_write16_ret
.endm

.macro	MCALLNF	flag
			tst		rAF, \flag
			beq		_call
			Z80WORK	#6
			add		r8, r8, #(2 << 16)
			mov		pc, r11
.endm

.macro	MCALLF	flag
			tst		rAF, \flag
			bne		_call
			Z80WORK	#6
			add		r8, r8, #(2 << 16)
			mov		pc, r11
.endm


.macro	MRETNF	flag
			Z80WORK	#1
			tst		rAF, \flag
			beq		_ret
			mov		pc, r11
.endm

.macro	MRETF	flag
			Z80WORK	#1
			tst		rAF, \flag
			bne		_ret
			mov		pc, r11
.endm


.macro	MPUSHr1
			mov		r0, r8, lsl #16
			subs	r2, r0, #(2 << 16)
			addcc	r8, r8, #(1 << 16)
			mov		r0, r2, lsr #16
			sub		r8, r8, #2
			tst		r0, #1
			streqh	r1, [r0, r5]
			moveq	pc, r11
			mov		lr, r11
			b		_mw16_odd
.endm

.macro	MPOPr0
			movs	r1, r8, lsl #16
			movmi	r12, r5
			movpl	r12, r10
			cmn		r1, #(2 << 16)
			subcs	r8, r8, #(1 << 16)			; @ round down...
			mov		r0, r1, lsr #16
			tst		r8, #1
			ldreqh	r0, [r0, r12]
			blne	_mr16_oddr12
			add		r8, r8, #2
.endm


.macro	MEXSP	src
			mov		r1, r8, lsl #16
			mov		r0, r1, lsr #16
			bl		_mem_read16
			ldrh	r1, [r4, \src]
			mov		r2, r8, lsl #16
			strh	r0, [r4, \src]
			mov		r0, r2, lsr #16
			b		_mem_write16_ret
.endm

.macro	MLDSP	src
			mov		r0, r8, lsr #16
			ldrh	r1, [r4, \src]
			orr		r8, r1, r0, lsl #16
			mov		pc, r11
.endm

