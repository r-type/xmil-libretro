
	.include	"./agb/ramptr.inc"
	.include	"./agb/romimg.inc"
	.include	"./vram/makescrn.inc"

	.global	makechr8
	.global	maketxt8fn
	.extern	makescrn

	.section	.iwram.text
	.code	16
	.align	0

	.include	"./vram/maketxtl.inc"
	.include	"./vram/makeatr.inc"

	.code	32
	.align	2

maketxt8fn:
		.word	1+_txt8_nor					@ ƒm[ƒ}ƒ‹
		.word	1+_txt8_halfx2				@ ’×‚µ‚Äc”{Šp
		.word	1+_txt8_right4dot			@ ‰E‚Sƒhƒbƒg
		.word	1+_txt8_right4half			@ ’×‚ê‰E‚Sƒhƒbƒg
		.word	1+_txt8_Yx2					@ c”{Šp
		.word	1+_draw_nop					@ ‚ ‚è‚¦‚È‚¢
		.word	1+_txt8_right4x2			@ ‰E‚Sƒhƒbƒgc”{Šp
		.word	1+_draw_nop					@ ‚ ‚è‚¦‚È‚¢
		.word	1+_txt8_x2left				@ ‰¡”{Šp¶
		.word	1+_txt8_halfx4left			@ ’×‚µ‚Ä‚S”{Šp¶
		.word	1+_txt8_x2right				@ ‰¡”{Šp‰E
		.word	1+_txt8_halfx4right			@ ’×‚µ‚Ä‚S”{Šp‰E
		.word	1+_txt8_x4left				@ ‚S”{Šp¶
		.word	1+_draw_nop					@ ‚ ‚è‚¦‚È‚¢
		.word	1+_txt8_x4right				@ ‚S”{Šp‰E
		.word	1+_draw_nop					@ ‚ ‚è‚¦‚È‚¢

		.word	1+_knj8_nor					@ ƒm[ƒ}ƒ‹
		.word	1+_knj8_halfx2				@ ’×‚µ‚Äc”{Šp
		.word	1+_knj8_right4dot			@ ‰E‚Sƒhƒbƒg
		.word	1+_knj8_right4half			@ ’×‚ê‰E‚Sƒhƒbƒg
		.word	1+_knj8_Yx2					@ c”{Šp
		.word	1+_draw_nop					@ ‚ ‚è‚¦‚È‚¢
		.word	1+_knj8_right4x2			@ ‰E‚Sƒhƒbƒgc”{Šp
		.word	1+_draw_nop					@ ‚ ‚è‚¦‚È‚¢
		.word	1+_knj8_x2left				@ ‰¡”{Šp¶
		.word	1+_knj8_halfx4left			@ ’×‚µ‚Ä‚S”{Šp¶
		.word	1+_knj8_x2right				@ ‰¡”{Šp‰E
		.word	1+_knj8_halfx4right			@ ’×‚µ‚Ä‚S”{Šp‰E
		.word	1+_knj8_x4left				@ ‚S”{Šp¶
		.word	1+_draw_nop					@ ‚ ‚è‚¦‚È‚¢
		.word	1+_knj8_x4right				@ ‚S”{Šp‰E
		.word	1+_draw_nop					@ ‚ ‚è‚¦‚È‚¢

makeatr8fn:
		.word	_atrb0+1,	_atrb1+1,	_atrb2+1,	_atrb3+1
		.word	_atrb4+1,	_atrb5+1,	_atrb6+1,	_atrb7+1
		.word	_atrb0r+1,	_atrb1r+1,	_atrb2r+1,	_atrb3r+1
		.word	_atrb4r+1,	_atrb5r+1,	_atrb6r+1,	_atrb7r+1


@ _makescrn:	.word	makescrn
_tram:			.word	tram
@ _font_ank:	.word	font_ank
@ _pcg:			.word	pcg


makechr8:		stmdb	sp!, {r4 - r8, lr}
				ldr		r5, _tram
				mov		r4, r1

				ldr		r8, [r5, r2, lsr #19]
				ldrb	r6, [r4, #MAKESCRN_BLINKTEST]
				tst		r6, r8
				eorne	r8, r8, #0x08
				tst		r3, #0x10
				and		r3, r3, #0x0f
				blne	mc_backword
				tst		r8, #0x20
				and		r1, r2, #0xff
				add		r1, r1, r0
				bne		mc8pcg

				tst		r8, #(0x80 << TRAM_KNJSFT)
				bne		mc8_knj
		.if 1
				add		r2, r5, #(EWRAM_ANK - EWRAM_TRAM)
		.else
				ldr		r2, _font_ank
		.endif
				and		r12, r8, #(0xff << TRAM_ANKSFT)
				mov		r7, #0x07
				add		r2, r2, r12, lsr #(TRAM_ANKSFT - 3)

mc8_tmake:		adr		r6, maketxt8fn
				and		r8, r8, #15
				adr		r5, makeatr8fn
				ldr		r6, [r6, r3, lsl #2]
				ldr		r12, [r5, r8, lsl #2]
				mov		r5, r0					@ for atr
				adr		lr, mc8_ret
				bx		r6

mc8pcg:
		.if 1
				add		r2, r5, #(EWRAM_PCG - EWRAM_TRAM)
		.else
				ldr		r2, _pcg
		.endif
				tst		r8, #(0x90 << TRAM_KNJSFT)
				andeq	r12, r8, #(0xff << TRAM_ANKSFT)
				andne	r12, r8, #(0xfe << TRAM_ANKSFT)
				addne	r3, r3, #16
				moveq	r7, #0x07
				movne	r7, #0x0f
				adr		lr, maketxt8fn
				add		r2, r2, r12, lsr #(TRAM_ANKSFT - 3)
				ldr		lr, [lr, r3, lsl #2]	@ proc!
				mov		r5, r0					@ dst
				tst		r8, #1
				adr		r12, 1f
				bxne	lr
				b		_cls8
1:				add		r5, r0, #MAKETEXT_ROW
				add		r1, r1, #MAKETEXT_ROW
				add		r2, r2, #0x800
				tst		r8, #2
				adr		r12, 2f
				bxne	lr
				b		_cls8
2:				add		r5, r0, #(MAKETEXT_ROW * 2)
				add		r1, r1, #MAKETEXT_ROW
				add		r2, r2, #0x800
				tst		r8, #4
				adr		r12, 3f
				bxne	lr
				b		_cls8
3:				tst		r8, #8
				blne	psgreverse
mc8_ret:		ldmia	sp!, {r4 - r8, pc}


_cls8:			mov		r3, #0
	.if 1												@ MAKETEXT_ROW <= 8
				str		r3, [r5]
				str		r3, [r5, #4]
	.else
1:				str		r3, [r5]
				add		r5, r5, #4
				cmp		r5, r1
				bcc		1b
	.endif
				mov		pc, r12


	.if 0
	.section	.text
	.endif

mc_backword:	sub		r12, r2, #(1 << 21)
				ldr		r1, [r5, r12, lsr #19]
				and		r8, r8, #0xff
				bic		r12, r1, #0xff
				tst		r1, #0x20
				orr		r8, r12, r8
				add		pc, lr, #4

_font_knjx1t:	.word	__extromimage + EXTROM_KNJX1T

mc8_knj:		ldr		r2, _font_knjx1t
				bic		r12, r8, #(0xe0 << TRAM_KNJSFT)
				bic		r12, r12, #(0xf << (TRAM_ANKSFT - 4))
				tst		r8, #(0x40 << TRAM_KNJSFT)
				addne	r2, r2, #0x20000
				mov		r7, #0x0f
				add		r3, r3, #16
				add		r2, r2, r12, lsr #(TRAM_ANKSFT - 4)
				b		mc8_tmake

psgreverse:		add		r5, r0, #(MAKETEXT_ROW * 2)
1:				ldr		r2, [r5, #(MAKETEXT_ROW * -2)]
				ldr		r3, [r5, #(MAKETEXT_ROW * -1)]
				ldr		r12, [r5, #(MAKETEXT_ROW * 0)]
				mvn		r2, r2
				mvn		r3, r3
				mvn		r12, r12
				str		r2, [r5, #(MAKETEXT_ROW * -2)]
				str		r3, [r5, #(MAKETEXT_ROW * -1)]
				str		r12, [r5, #(MAKETEXT_ROW * 0)]
				add		r5, r5, #4
				cmp		r5, r1
				bcc		1b
				mov		pc, lr

	.end

