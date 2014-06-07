
	MACRO
$label	LDA_xI	$b
$label		GETIPC	r0, $b
			MEMRD8	r1, r0
			and		r2, r8, #&ff
			orr		r8, r2, r1, lsl #8
			mov		pc, r11
	MEND

	MACRO
$label	LD8_xI	$dst, $b
$label		GETIPC	r0, $b
			MEMRD8	r1, r0
			strb	r1, [r7, #$dst]
			mov		pc, r11
	MEND

	MACRO
$label	LDxI_A	$b
$label		GETIPC	r0, $b
			mov		r1, r8, lsr #8
			MEMWR8	r0, r1
			mov		pc, r11
	MEND

	MACRO
$label	LDxI_8	$b, $src
$label		GETIPC	r0, $b
			ldrb	r1, [r7, #$src]
			MEMWR8	r0, r1
			mov		pc, r11
	MEND

	MACRO
$label	LDxI_b	$b
$label		GETIPC	r4, $b
			GETPC8
			MEMWR8	r4, r0
			mov		pc, r11
	MEND


	MACRO
$label	MINCI8	$b
$label		GETIPC	r0, $b
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
$label	MDECI8	$b
$label		GETIPC	r0, $b
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
$label	MADDI8	$b
$label		GETIPC	r0, $b
			MEMRD8	r12, r0
			MADD8	r12
	MEND

	MACRO
$label	MADCI8	$b
$label		GETIPC	r0, $b
			MEMRD8	r12, r0
			MADC8	r12
	MEND

	MACRO
$label	MSUBI8	$b
$label		GETIPC	r0, $b
			MEMRD8	r12, r0
			MSUB8	r12
	MEND

	MACRO
$label	MSBCI8	$b
$label		GETIPC	r0, $b
			MEMRD8	r12, r0
			MSBC8	r12
	MEND

	MACRO
$label	MANDI8	$b
$label		GETIPC	r0, $b
			MEMRD8	r12, r0
			MAND8	r12
	MEND

	MACRO
$label	MXORI8	$b
$label		GETIPC	r0, $b
			MEMRD8	r12, r0
			MXOR8	r12
	MEND

	MACRO
$label	MORI8	$b
$label		GETIPC	r0, $b
			MEMRD8	r12, r0
			MOR8	r12
	MEND

	MACRO
$label	MCPI8	$b
$label		GETIPC	r0, $b
			MEMRD8	r12, r0
			MCP8	r12
	MEND


	END

