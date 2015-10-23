
.macro	MADC16	src
			ldrh	r0, [r4, #CPU_HL]
			ldrh	r1, [r4, \src]
			movs	r12, rAF, lsr #1
			adc		r2, r1, r0
			and		rAF, rAF, #(0xff << 8)
			eor		r3, r2, r0					; @ r ^ b
			eor		r12, r2, r1					; @ r ^ A
			strh	r2, [r4, #CPU_HL]
			movs	r0, r2, lsl #16
			eor		r0, r3, r1					; @ r ^ b ^ A
			orrcs	rAF, rAF, #C_FLAG
			and		r3, r12, r3					; @ (r ^ b) & (r ^ A)
			orreq	rAF, rAF, #Z_FLAG
			and		r0, r0, #(H_FLAG << 8)
			orrmi	rAF, rAF, #S_FLAG
			and		r3, r3, #(V_FLAG << 13)
			orr		rAF, rAF, r0, lsr #8
			orr		rAF, rAF, r3, lsr #13
			mov		pc, r11
.endm

.macro	MADCHL2
			ldrh	r0, [r4, #CPU_HL]
			movs	r12, rAF, lsr #1
			adc		r1, r0, r0
			and		rAF, rAF, #(0xff << 8)
			movs	r12, r1, lsl #16
			orrcs	rAF, rAF, #C_FLAG
			strh	r1, [r4, #CPU_HL]
			eor		r2, r1, r0
			orreq	rAF, rAF, #Z_FLAG
			and		r3, r2, #(V_FLAG << 13)
			and		r2, r1, #(H_FLAG << 8)
			orrmi	rAF, rAF, #S_FLAG
			orr		rAF, rAF, r3, lsr #13
			orr		rAF, rAF, r2, lsr #8
			mov		pc, r11
.endm

.macro	MADCSP
			ldrh	r0, [r4, #CPU_HL]
			mov		r1, r8, lsl #16
			and		r2, rAF, #C_FLAG
			add		r2, r2, r0
			add		r2, r2, r1, lsr #16
			and		rAF, rAF, #(0xff << 8)
			eor		r3, r2, r0					; @ r ^ b
			eor		r12, r2, r8					; @ r ^ A
			strh	r2, [r4, #CPU_HL]
			movs	r0, r2, lsl #16
			eor		r0, r3, r8					; @ r ^ b ^ A
			orrcs	rAF, rAF, #C_FLAG
			and		r3, r12, r3					; @ (r ^ b) & (r ^ A)
			orreq	rAF, rAF, #Z_FLAG
			and		r0, r0, #(H_FLAG << 8)
			orrmi	rAF, rAF, #S_FLAG
			and		r3, r3, #(V_FLAG << 13)
			orr		rAF, rAF, r0, lsr #8
			orr		rAF, rAF, r3, lsr #13
			mov		pc, r11
.endm


.macro	MSBC16	src
			ldrh	r0, [r4, #CPU_HL]
			ldrh	r1, [r4, \src]
			and		r2, rAF, #C_FLAG
			rsb		r2, r2, r0
			eor		r3, r1, r0					; @ A ^ b
			sub		r2, r2, r1
			eor		r0, r2, r0					; @ A ^ r
			eor		r1, r2, r3					; @ A ^ b ^ r
			movs	r12, r2, lsl #16
			and		rAF, rAF, #(0xff << 8)
			strh	r2, [r4, #CPU_HL]
			orr		rAF, rAF, #N_FLAG
			and		r0, r0, r3					; @ (A ^ b) & (A ^ r)
			orrcs	rAF, rAF, #C_FLAG
			and		r1, r1, #(H_FLAG << 8)
			orreq	rAF, rAF, #Z_FLAG
			and		r0, r0, #(V_FLAG << 13)
			orrmi	rAF, rAF, #S_FLAG
			orr		rAF, rAF, r1, lsr #8
			orr		rAF, rAF, r0, lsr #13
			mov		pc, r11
.endm

.macro	MSBCHL2
			mov		r0, #0
			tst		rAF, #C_FLAG
			subne	r0, r0, #1
			and		rAF, rAF, #(0xff << 8)
			strh	r0, [r4, #CPU_HL]
			orreq	rAF, rAF, #(Z_FLAG + N_FLAG)
			orrne	rAF, rAF, #(S_FLAG + H_FLAG + N_FLAG + C_FLAG)
			mov		pc, r11
.endm

.macro	MSBCSP
			ldrh	r0, [r4, #CPU_HL]
			mov		r1, r8, lsl #16
			and		r2, rAF, #C_FLAG
			rsb		r2, r2, r0
			eor		r3, r0, r8					; @ A ^ b
			sub		r2, r2, r1, lsr #16
			eor		r0, r2, r0					; @ A ^ r
			eor		r1, r2, r3					; @ A ^ b ^ r
			movs	r12, r2, lsl #16
			and		rAF, rAF, #(0xff << 8)
			strh	r2, [r4, #CPU_HL]
			orr		rAF, rAF, #N_FLAG
			and		r0, r0, r3					; @ (A ^ b) & (A ^ r)
			orrcs	rAF, rAF, #C_FLAG
			and		r1, r1, #(H_FLAG << 8)
			orreq	rAF, rAF, #Z_FLAG
			and		r0, r0, #(V_FLAG << 13)
			orrmi	rAF, rAF, #S_FLAG
			orr		rAF, rAF, r1, lsr #8
			orr		rAF, rAF, r0, lsr #13
			mov		pc, r11
			mov		pc, r11
.endm


.macro	INP8
			ldrh	r0, [r4, #CPU_BC]
			Z80INP
			add		r1, r4, #CPU_SZPFLAG
			bic		r2, rAF, #(0xff - C_FLAG)
			ldrb	r3, [r1, r0]
			orr		rAF, r3, r2
.endm

.macro	INPR8	reg
			INP8
			strb	r0, [r4, #\reg]
			mov		pc, r11
.endm

