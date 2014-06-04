
	MACRO
$label	MADC16	$r
$label		ldrh	r0, [r7, #CPU_HL]
			ldrh	r1, [r7, #$r]
			movs	r12, r8, lsr #1
			adc		r2, r1, r0
			and		r8, r8, #&ff00
			eor		r3, r2, r0				; r ^ b
			eor		r12, r2, r1				; r ^ A
			strh	r2, [r7, #CPU_HL]
			movs	r0, r2, lsl #16
			eor		r0, r3, r1				; r ^ b ^ A
			orrcs	r8, r8, #C_FLAG
			and		r3, r12, r3				; (r ^ b) & (r ^ A)
			orreq	r8, r8, #Z_FLAG
			and		r0, r0, #(H_FLAG << 8)
			orrmi	r8, r8, #S_FLAG
			and		r3, r3, #(V_FLAG << 13)
			orr		r8, r8, r0, lsr #8
			orr		r8, r8, r3, lsr #13
			mov		pc, r11
	MEND

	MACRO
$label	MADCHL2
$label		ldrh	r0, [r7, #CPU_HL]
			movs	r12, r8, lsr #1
			adc		r1, r0, r0
			and		r8, r8, #&ff00
			movs	r12, r1, lsl #16
			orrcs	r8, r8, #C_FLAG
			strh	r1, [r7, #CPU_HL]
			eor		r2, r1, r0
			orreq	r8, r8, #Z_FLAG
			and		r3, r2, #(V_FLAG << 13)
			and		r2, r1, #(H_FLAG << 8)
			orrmi	r8, r8, #S_FLAG
			orr		r8, r8, r3, lsr #13
			orr		r8, r8, r2, lsr #8
			mov		pc, r11
	MEND

	MACRO
$label	MADCSP
$label		ldrh	r0, [r7, #CPU_HL]
			mov		r1, r9, lsl #16
			and		r2, r8, #C_FLAG
			add		r2, r2, r0
			add		r2, r2, r1, lsr #16
			and		r8, r8, #&ff00
			eor		r3, r2, r0				; r ^ b
			eor		r12, r2, r9				; r ^ A
			strh	r2, [r7, #CPU_HL]
			movs	r0, r2, lsl #16
			eor		r0, r3, r9				; r ^ b ^ A
			orrcs	r8, r8, #C_FLAG
			and		r3, r12, r3				; (r ^ b) & (r ^ A)
			orreq	r8, r8, #Z_FLAG
			and		r0, r0, #(H_FLAG << 8)
			orrmi	r8, r8, #S_FLAG
			and		r3, r3, #(V_FLAG << 13)
			orr		r8, r8, r0, lsr #8
			orr		r8, r8, r3, lsr #13
			mov		pc, r11
	MEND


	MACRO
$label	MSBC16	$r
$label		ldrh	r0, [r7, #CPU_HL]
			ldrh	r1, [r7, #$r]
			and		r2, r8, #C_FLAG
			rsb		r2, r2, r0
			eor		r3, r1, r0				; A ^ b
			sub		r2, r2, r1
			eor		r0, r2, r0				; A ^ r
			eor		r1, r2, r3				; A ^ b ^ r
			movs	r12, r2, lsl #16
			and		r8, r8, #&ff00
			strh	r2, [r7, #CPU_HL]
			orr		r8, r8, #N_FLAG
			and		r0, r0, r3				; (A ^ b) & (A ^ r)
			orrcs	r8, r8, #C_FLAG
			and		r1, r1, #(H_FLAG << 8)
			orreq	r8, r8, #Z_FLAG
			and		r0, r0, #(V_FLAG << 13)
			orrmi	r8, r8, #S_FLAG
			orr		r8, r8, r1, lsr #8
			orr		r8, r8, r0, lsr #13
			mov		pc, r11
	MEND

	MACRO
$label	MSBCHL2
$label		mov		r0, #0
			tst		r8, #C_FLAG
			subne	r0, r0, #1
			and		r8, r8, #&ff00
			strh	r0, [r7, #CPU_HL]
			orreq	r8, r8, #(Z_FLAG + N_FLAG)
			orrne	r8, r8, #(S_FLAG + H_FLAG + N_FLAG + C_FLAG)
			mov		pc, r11
	MEND

	MACRO
$label	MSBCSP
$label		ldrh	r0, [r7, #CPU_HL]
			mov		r1, r9, lsl #16
			and		r2, r8, #C_FLAG
			rsb		r2, r2, r0
			eor		r3, r0, r9				; A ^ b
			sub		r2, r2, r1, lsr #16
			eor		r0, r2, r0				; A ^ r
			eor		r1, r2, r3				; A ^ b ^ r
			movs	r12, r2, lsl #16
			and		r8, r8, #&ff00
			strh	r2, [r7, #CPU_HL]
			orr		r8, r8, #N_FLAG
			and		r0, r0, r3				; (A ^ b) & (A ^ r)
			orrcs	r8, r8, #C_FLAG
			and		r1, r1, #(H_FLAG << 8)
			orreq	r8, r8, #Z_FLAG
			and		r0, r0, #(V_FLAG << 13)
			orrmi	r8, r8, #S_FLAG
			orr		r8, r8, r1, lsr #8
			orr		r8, r8, r0, lsr #13
			mov		pc, r11
			mov		pc, r11
	MEND


	MACRO
$label	OUT8
$label		ldrh	r0, [r7, #CPU_BC]
			str		r10, [r7, #CPU_REMCLOCK]
			bl		iocore_out
			ldr		r10, [r7, #CPU_REMCLOCK]
			mov		pc, r11
	MEND

	MACRO
$label	OUTR8	$r
$label		ldrb	r1, [r7, #$r]
			OUT8
	MEND

	MACRO
$label	INP8
$label		ldrh	r0, [r7, #CPU_BC]
			str		r10, [r7, #CPU_REMCLOCK]
			bl		iocore_inp
			ldrb	r1, [r0, r7]
			bic		r2, r8, #(&ff - C_FLAG)
			ldr		r10, [r7, #CPU_REMCLOCK]
			orr		r8, r2, r1
	MEND

	MACRO
$label	INPR8	$r
$label		INP8
			strb	r0, [r7, #$r]
			mov		pc, r11
	MEND

	END

