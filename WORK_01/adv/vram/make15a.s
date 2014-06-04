
	.include	"./vram/vram.inc"
	.include	"./vram/makescrn.inc"

	.global	width40x25_200l
	.global	width80x25_200l


	.section	.iwram.text
@	.section	.text
	.code	32
	.align	0


_makescrn:		.word	makescrn
_updatetmp:		.word	vramupd

width40x25_200l:
				stmdb	sp!, {r4 - r11, lr}
				sub		sp, sp, #(MAKETEXT_ROW * 3)
				ldr		r4, _makescrn
				ldr		r5, _updatetmp
				mov		r6, #0x06000000
				ldr		r1, [r4, #MAKESCRN_FONTCY]
				ldrb	r2, [r4, #MAKESCRN_DISPFLAG]
				cmp		r1, #MAKETEXT_ROW
				movcc	r1, #MAKETEXT_ROW
				add		r9, r1, r0, lsl #21
				orr		r9, r9, r2, lsl #8
				ldrb	r7, [r4, #MAKESCRN_SURFCY]
00:				ldrb	r0, [r4, #MAKESCRN_SURFCX]
				sub		r7, r7, r0, lsl #8
10:				ldrb	r8, [r5, r9, lsr #21]
				ands	r0, r8, r9, lsr #8
				beq		12f
				eor		r0, r0, r8
				strb	r0, [r5, r9, lsr #21]
				tst		r8, #UPDATE_TRAM
				beq		11f
				mov		r0, sp
				mov		r1, r4
				mov		r2, r9
				mov		r3, r8
				bl		makechr8
				mov		r0, r6
				mov		r1, sp
				and		r2, r9, #0xff
				bl		makemix_mixtext40
				tst		r8, #UPDATE_VRAM
				beq		12f
11:				mov		r0, r6
				ldr		r1, [r4, #MAKESCRN_DISP1]
				add		r1, r1, r9, lsr #21
				and		r2, r9, #0xff
				bl		makemix_mixgrph40
12:				add		r6, r6, #8
				add		r9, r9, #(1 << 21)
				adds	r7, r7, #(1 << 8)
				bmi		10b
				ldr		r0, [r4, #MAKESCRN_CHARCY]
				tst		r8, #4
				ldrne	r1, [r4, #MAKESCRN_FONTYCNT]
				addeq	r0, r0, r0
				addne	r0, r1, r0
				and		r0, r0, #15
				str		r0, [r4, #MAKESCRN_FONTYCNT]
				ldr		r1, [r4, #MAKESCRN_SURFSTEP]
				ldr		r2, [r4, #MAKESCRN_SURFRX]
				add		r6, r1, r6
				add		r9, r9, r2, lsl #21
				subs	r7, r7, #1
				bne		00b
				add		sp, sp, #(MAKETEXT_ROW * 3)
				ldmia	sp!, {r4 - r11, pc}


width80x25_200l:
				stmdb	sp!, {r4 - r11, lr}
				sub		sp, sp, #(MAKETEXT_ROW * 3)
				ldr		r4, _makescrn
				ldr		r5, _updatetmp
				mov		r6, #0x06000000
				ldr		r1, [r4, #MAKESCRN_FONTCY]
				ldrb	r2, [r4, #MAKESCRN_DISPFLAG]
				cmp		r1, #MAKETEXT_ROW
				movcc	r1, #MAKETEXT_ROW
				add		r9, r1, r0, lsl #21
				orr		r9, r9, r2, lsl #8
				ldrb	r7, [r4, #MAKESCRN_SURFCY]
00:				ldrb	r0, [r4, #MAKESCRN_SURFCX]
				sub		r7, r7, r0, lsl #8
10:				ldrb	r8, [r5, r9, lsr #21]
				ands	r0, r8, r9, lsr #8
				beq		12f
				eor		r0, r0, r8
				strb	r0, [r5, r9, lsr #21]
				tst		r8, #UPDATE_TRAM
				beq		11f
				mov		r0, sp
				mov		r1, r4
				mov		r2, r9
				mov		r3, r8
				bl		makechr8
				mov		r0, r6
				mov		r1, sp
				and		r2, r9, #0xff
				bl		makemix_mixtext80
				tst		r8, #UPDATE_VRAM
				beq		12f
11:				mov		r0, r6
				ldr		r1, [r4, #MAKESCRN_DISP1]
				add		r1, r1, r9, lsr #21
				and		r2, r9, #0xff
				bl		makemix_mixgrph80
12:				add		r6, r6, #4
				add		r9, r9, #(1 << 21)
				adds	r7, r7, #(1 << 8)
				bmi		10b
				ldr		r0, [r4, #MAKESCRN_CHARCY]
				tst		r8, #4
				ldrne	r1, [r4, #MAKESCRN_FONTYCNT]
				addeq	r0, r0, r0
				addne	r0, r1, r0
				and		r0, r0, #15
				str		r0, [r4, #MAKESCRN_FONTYCNT]
				ldr		r1, [r4, #MAKESCRN_SURFSTEP]
				ldr		r2, [r4, #MAKESCRN_SURFRX]
				add		r6, r1, r6
				add		r9, r9, r2, lsl #21
				subs	r7, r7, #1
				bne		00b
				add		sp, sp, #(MAKETEXT_ROW * 3)
				ldmia	sp!, {r4 - r11, pc}

	.end

