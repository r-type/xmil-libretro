
	MACRO
$label	LD8		$d, $s
$label		ldrb	r0, [r7, #$s]
			strb	r0, [r7, #$d]
			mov		pc, r11
	MEND

	MACRO
$label	LD8A	$d
$label		mov		r0, r8, lsr #8
			strb	r0, [r7, #$d]
			mov		pc, r11
	MEND

	MACRO
$label	LDA8	$s
$label		ldrb	r0, [r7, #$s]
			and		r8, r8, #&ff
			orr		r8, r8, r0, lsl #8
			mov		pc, r11
	MEND

	MACRO
$label	LDAx	$src
$label		ldrh	r0, [r7, #$src]
			and		r8, r8, #&ff
			MEMRD8	r0, r0
			add		r8, r8, r0, lsl #8
			mov		pc, r11
	MEND

	MACRO
$label	LD8x	$d, $s
$label		ldrh	r0, [r7, #$s]
			MEMRD8	r1, r0
			strb	r1, [r7, #$d]
			mov		pc, r11
	MEND

	MACRO
$label	LDxA	$dst
$label		ldrh	r0, [r7, #$dst]
			mov		r1, r8, lsr #8
			MEMWR8	r0, r1
			mov		pc, r11
	MEND

	MACRO
$label	LDx8	$dst, $src
$label		ldrh	r0, [r7, #$dst]
			ldrb	r1, [r7, #$src]
			MEMWR8	r0, r1
			mov		pc, r11
	MEND

	MACRO
$label	LD8b	$dst
$label		GETPC8
			strb	r0, [r7, #$dst]
			mov		pc, r11
	MEND

	MACRO
$label	LD16w 	$dst
$label		GETPC16
			strh	r0, [r7, #$dst]
			mov		pc, r11
	MEND

	MACRO
$label	LDx16 $src
$label		GETPC16
			ldrh	r1, [r7, #$src]
			mov		lr, r11
			b		mem_write16
	MEND

	MACRO
$label	LD16x $dst
$label		GETPC16
			bl		mem_read16
			strh	r0, [r7, #$dst]
			mov		pc, r11
	MEND

	MACRO
$label	MINC16	$dst
$label		ldrh	r0, [r7, #$dst]
			add		r0, r0, #1
			strh	r0, [r7, #$dst]
			mov		pc, r11
	MEND

	MACRO
$label	MINC8	$dst
$label		ldrb	r0, [r7, #$dst]
			add		r1, r7, #CPU_INCFLAG
			bic		r8, r8, #(&ff - C_FLAG)
			ldrb	r2, [r0, r1]
			add		r3, r0, #1
			orr		r8, r8, r2
			strb	r3, [r7, #$dst]
			mov		pc, r11
	MEND

	MACRO
$label	MINCM8	$dst
$label		ldrh	r0, [r7, #$dst]
			MEMLEA8	r0, r1, r2
			ldrb	r3, [r1, r0]
			add		r12, r7, #CPU_INCFLAG
			bic		r8, r8, #(&ff - C_FLAG)
			ldrb	r1, [r12, r3]
			add		r3, r3, #1
			orr		r8, r8, r1
			strb	r3, [r2, r0]
			mov		pc, r11
	MEND

	MACRO
$label	MDEC8	$dst
$label		ldrb	r0, [r7, #$dst]
			add		r1, r7, #CPU_DECFLAG
			bic		r8, r8, #(&ff - C_FLAG)
			ldrb	r2, [r0, r1]
			sub		r3, r0, #1
			orr		r8, r8, r2
			strb	r3, [r7, #$dst]
			mov		pc, r11
	MEND

	MACRO
$label	MDECM8	$dst
$label		ldrh	r0, [r7, #$dst]
			MEMLEA8	r0, r1, r2
			ldrb	r3, [r1, r0]
			add		r12, r7, #CPU_DECFLAG
			bic		r8, r8, #(&ff - C_FLAG)
			ldrb	r1, [r12, r3]
			sub		r3, r3, #1
			orr		r8, r8, r1
			strb	r3, [r2, r0]
			mov		pc, r11
	MEND


	MACRO
$label	MADD16	$dst, $src
$label		ldrh	r0, [r7, #$dst]
			ldrh	r1, [r7, #$src]
			bic		r8, r8, #(&ff - (S_FLAG + Z_FLAG + V_FLAG))
			add		r2, r1, r0
			eor		r3, r1, r0
			add		r8, r8, r2, lsr #16
			eor		r3, r3, r2
			strh	r2, [r7, #$dst]
			and		r3, r3, #(H_FLAG << 8)
			orr		r8, r8, r3, lsr #8
			mov		pc, r11
	MEND

	MACRO
$label	MADD16D	$dst
$label		ldrh	r0, [r7, #$dst]
			bic		r8, r8, #(&ff - (S_FLAG + Z_FLAG + V_FLAG))
			and		r1, r0, #(H_FLAG << 8)
			add		r8, r8, r0, lsr #15
			add		r2, r0, r0
			add		r8, r8, r1, lsr #8
			strh	r2, [r7, #$dst]
			mov		pc, r11
	MEND

	MACRO
$label	MADDSP	$dst
$label		ldrh	r0, [r7, #$dst]
			mov		r1, r9, lsl #16
			bic		r8, r8, #(&ff - (S_FLAG + Z_FLAG + V_FLAG))
			add		r2, r0, r1, lsr #16
			eor		r3, r0, r1, lsr #16
			add		r8, r8, r2, lsr #16
			eor		r3, r3, r2
			and		r3, r3, #(H_FLAG << 8)
			strh	r2, [r7, #$dst]
			orr		r8, r8, r3, lsr #8
			mov		pc, r11
	MEND

	MACRO
$label	MDEC16	$dst
$label		ldrh	r0, [r7, #$dst]
			sub		r0, r0, #1
			strh	r0, [r7, #$dst]
			mov		pc, r11
	MEND


	MACRO
$label	MADD8	$r
$label		and		r1, r8, #(&ff << 8)
			mov		r2, $r lsl #24
			adds	r3, r2, r1, lsl #16
			orrcs	r3, r3, #(C_FLAG << 16)
			orrvs	r3, r3, #(V_FLAG << 16)
			eor		r0, r2, r1, lsl #16
			orreq	r3, r3, #(Z_FLAG << 16)
			eor		r1, r0, r3
			orrmi	r3, r3, #(S_FLAG << 16)
			and		r2, r1, #(H_FLAG << 24)
			mov		r8, r3, lsr #16
			orr		r8, r8, r2, lsr #24
			mov		pc, r11
	MEND

	MACRO
$label	MADDR8	$src
$label		ldrb	r0, [r7, #$src]
			MADD8	r0
	MEND

	MACRO
$label	MADDM8	$src
$label		ldrh	r0, [r7, #$src]
			MEMRD8	r12, r0
			MADD8	r12
	MEND

	MACRO
$label	MADC8	$r
$label		and		r1, r8, #C_FLAG
			add		r1, r1, r8, lsr #8
			add		r1, r1, $r
			eor		r2, r1, $r				; r ^ b
			eor		r3, r1, r8, lsr #8		; r ^ A
			movs	r1, r1, lsl #24
			eor		r0, r2, r8, lsr #8		; r ^ b ^ A
			orrcs	r1, r1, #(C_FLAG << 16)
			and		r2, r3, r2				; (r ^ b) & (r ^ A)
			orreq	r1, r1, #(Z_FLAG << 16)
			and		r0, r0, #H_FLAG
			orrmi	r1, r1, #(S_FLAG << 16)
			and		r2, r2, #(V_FLAG << 5)
			orr		r8, r0, r1, lsr #16
			orr		r8, r8, r2, lsr #5
			mov		pc, r11
	MEND

	MACRO
$label	MADCR8	$src
$label		ldrb	r0, [r7, #$src]
			MADC8	r0
	MEND

	MACRO
$label	MADCM8	$src
$label		ldrh	r0, [r7, #$src]
			MEMRD8	r12, r0
			MADC8	r12
	MEND

	MACRO
$label	MSUB8	$r
$label		and		r1, r8, #(&ff << 8)
			mov		r2, $r lsl #24
			rsbs	r3, r2, r1, lsl #16
			orr		r3, r3, #(N_FLAG << 16)
			orrcc	r3, r3, #(C_FLAG << 16)
			orrvs	r3, r3, #(V_FLAG << 16)
			eor		r0, r2, r1, lsl #16
			orreq	r3, r3, #(Z_FLAG << 16)
			eor		r1, r0, r3
			orrmi	r3, r3, #(S_FLAG << 16)
			and		r2, r1, #(H_FLAG << 24)
			mov		r8, r3, lsr #16
			orr		r8, r8, r2, lsr #24
			mov		pc, r11
	MEND

	MACRO
$label	MSUBR8	$src
$label		ldrb	r0, [r7, #$src]
			MSUB8	r0
	MEND

	MACRO
$label	MSUBM8	$src
$label		ldrh	r0, [r7, #$src]
			MEMRD8	r12, r0
			MSUB8	r12
	MEND


	MACRO
$label	MSBC8	$r
$label		and		r1, r8, #C_FLAG
			rsb		r2, $r, r8, lsr #8
			eor		r3, $r, r8, lsr #8		; A ^ b
			sub		r2, r2, r1
			eor		r0, r2, r8, lsr #8		; A ^ r
			eor		r1, r2, r3				; A ^ b ^ r
			movs	r2, r2, lsl #24
			and		r0, r0, r3				; (A ^ b) & (A ^ r)
			orr		r2, r2, #(N_FLAG << 16)
			orrcs	r2, r2, #(C_FLAG << 16)
			orreq	r2, r2, #(Z_FLAG << 16)
			and		r1, r1, #H_FLAG
			orrmi	r2, r2, #(S_FLAG << 16)
			and		r0, r0, #(V_FLAG << 5)
			orr		r8, r1, r2, lsr #16
			orr		r8, r8, r0, lsr #5
			mov		pc, r11
	MEND

	MACRO
$label	MSBCR8	$src
$label		ldrb	r0, [r7, #$src]
			MSBC8	r0
	MEND

	MACRO
$label	MSBCM8	$src
$label		ldrh	r0, [r7, #$src]
			MEMRD8	r12, r0
			MSBC8	r12
	MEND


	MACRO
$label	MAND8	$r
$label		and		r2, $r, r8, lsr #8
			ldrb	r3, [r2, r7]
			orr		r8, r3, r2, lsl #8
			mov		pc, r11
		MEND

	MACRO
$label	MANDR8	$src
$label		ldrb	r0, [r7, #$src]
			MAND8	r0
	MEND

	MACRO
$label	MANDM8	$src
$label		ldrh	r0, [r7, #$src]
			MEMRD8	r12, r0
			MAND8	r12
	MEND


	MACRO
$label	MXOR8	$r
$label		eor		r2, $r, r8, lsr #8
			ldrb	r3, [r2, r7]
			orr		r8, r3, r2, lsl #8
			mov		pc, r11
	MEND

	MACRO
$label	MXORR8	$src
$label		ldrb	r0, [r7, #$src]
			MXOR8	r0
	MEND

	MACRO
$label	MXORM8	$src
$label		ldrh	r0, [r7, #$src]
			MEMRD8	r12, r0
			MXOR8	r12
	MEND


	MACRO
$label	MOR8	$r
$label		orr		r2, $r, r8, lsr #8
			ldrb	r3, [r2, r7]
			orr		r8, r3, r2, lsl #8
			mov		pc, r11
	MEND

	MACRO
$label	MORR8	$src
$label		ldrb	r0, [r7, #$src]
			MOR8	r0
	MEND

	MACRO
$label	MORM8	$src
$label		ldrh	r0, [r7, #$src]
			MEMRD8	r12, r0
			MOR8	r12
	MEND


	MACRO
$label	MCP8	$r
$label		and		r1, r8, #(&ff << 8)
			mov		r2, $r lsl #24
			rsbs	r3, r2, r1, lsl #16
			orr		r1, r1, #N_FLAG
			eor		r0, r2, r8, lsl #16
			orrcc	r1, r1, #C_FLAG
			eor		r2, r0, r3
			orrvs	r1, r1, #V_FLAG
			and		r0, r2, #(H_FLAG << 24)
			orreq	r1, r1, #Z_FLAG
			orrmi	r1, r1, #S_FLAG
			orr		r8, r1, r0, lsr #24
			mov		pc, r11
	MEND

	MACRO
$label	MCPR8	$src
$label		ldrb	r0, [r7, #$src]
			MCP8	r0
	MEND

	MACRO
$label	MCPM8	$src
$label		ldrh	r0, [r7, #$src]
			MEMRD8	r12, r0
			MCP8	r12
	MEND





	MACRO
$label	MJR
$label		GETPC8S
			Z80WORK	#5
			add		r9, r9, r0, lsl #16
			mov		pc, r11
	MEND

	MACRO
$label	MJREQ
$label		addne	r9, r9, #&10000
			movne	pc, r11
			MJR
	MEND

	MACRO
$label	MJRNFLG	$flag
$label		tst		r8, #$flag
			MJREQ
	MEND

	MACRO
$label	MJRNE
$label		addeq	r9, r9, #&10000
			moveq	pc, r11
			MJR
	MEND

	MACRO
$label	MJRFLG	$flag
$label		tst		r8, #$flag
			MJRNE
	MEND


	MACRO
$label	MJP
$label		mov		r4, r9, lsl #16
			GETPC16
			mov		r4, r4, lsr #16
			orr		r9, r4, r0, lsl #16
			mov		pc, r11
	MEND

	MACRO
$label	MJPR16	$r
$label		ldrh	r0, [r7, #$r]
			mov		r4, r9, lsl #16
			mov		r9, r0, lsl #16
			orr		r9, r9, r4, lsr #16
			mov		pc, r11
	MEND

	MACRO
$label	MJPNFLG	$flag
$label		tst		r8, #$flag
			addne	r9, r9, #&20000
			movne	pc, r11
			MJP
	MEND

	MACRO
$label	MJPFLG	$flag
$label		tst		r8, #$flag
			addeq	r9, r9, #&20000
			moveq	pc, r11
			MJP
	MEND


	MACRO
$label	MCALL
$label		GETPC16
			mov		r2, r9, lsl #16		; sp
			mov		r3, r0, lsl #16		; new pc
			sub		r2, r2, #&20000
			mov		r1, r9, lsr #16
			mov		r0, r2, lsr #16
			orr		r9, r3, r2 lsr #16
			mov		lr, r11
			Z80WORK	#7
			b		mem_write16
	MEND

	MACRO
$label	MCALLNF	$flag
$label		tst		r8, #$flag
			addne	r9, r9, #&20000
			movne	pc, r11
			MCALL
	MEND

	MACRO
$label	MCALLF	$flag
$label		tst		r8, #$flag
			addeq	r9, r9, #&20000
			moveq	pc, r11
			MCALL
	MEND


	MACRO
$label	MRET
$label		mov		r4, r9, lsl #16		; sp
			mov		r0, r4, lsr #16
			bl		mem_read16
			add		r4, r4, #&20000
			mov		r0, r0, lsl #16
			Z80WORK	#6
			add		r9, r0, r4, lsr #16
			mov		pc, r11
	MEND

	MACRO
$label	MRETNF	$flag
$label		tst		r8, #$flag
			movne	pc, r11
			MRET
	MEND

	MACRO
$label	MRETF	$flag
$label		tst		r8, #$flag
			moveq	pc, r11
			MRET
	MEND


	MACRO
$label	MRST	$vct
$label		mov		r0, r9, lsl #16
			mov		r1, r9, lsr #16
			sub		r0, r0, #&20000
			mov		lr, r11
			mov		r0, r0, lsr #16
			orr		r9, r0, #($vct << 16)
			b		mem_write16
	MEND


	MACRO
$label	MPUSHr1
$label		mov		r0, r9, lsl #16
			mov		lr, r11
			subs	r0, r0, #&20000
			addcc	r9, r9, #&10000
			mov		r0, r0, lsr #16
			sub		r9, r9, #2
			b		mem_write16
	MEND

	MACRO
$label	MPUSH	$dst
$label		ldrh	r1, [r7, #$dst]
			MPUSHr1
	MEND

	MACRO
$label	MPOPr0
$label		mov		r1, r9, lsl #16
			cmn		r1, #(2 << 16)
			subcs	r9, r9, #(1 << 16)				; round down...
			mov		r0, r1, lsr #16
			bl		mem_read16
			add		r9, r9, #2
	MEND

	MACRO
$label	MPOP	$dst
$label		MPOPr0
			strh	r0, [r7, #$dst]
			mov		pc, r11
	MEND


	MACRO
$label	MEXSP	$src
$label		mov		r4, r9, lsl #16
			mov		r0, r4, lsr #16
			bl		mem_read16
			ldrh	r1, [r7, #$src]
			mov		lr, r11
			strh	r0, [r7, #$src]
			mov		r0, r4, lsr #16
			b		mem_write16

	MEND

	MACRO
$label	MLDSP	$src
$label		mov		r0, r9, lsr #16
			ldrh	r1, [r7, #$src]
			orr		r9, r1, r0, lsl #16
			mov		pc, r11
	MEND


	END

