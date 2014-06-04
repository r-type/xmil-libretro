
	.include	"./vram/vram.inc"
	.include	"./vram/makescrn.inc"

	.global	makemix_mixstep
	.global	makemix_mixtext40
	.global	makemix_mixgrph40
	.global	makemix_mixtext80
	.global	makemix_mixgrph80

	.section	.itcm
	.code	32
	.align	2

textmask:	.word		0x07070707


240:			add		r0, r0, #(MAKESCRN_VIDEOYALIGN)
480:			add		r0, r0, #(MAKESCRN_VIDEOYALIGN * 2)


makemix_mixstep:
				cmp		r0, #0
				ldreq	r1, 240b
				ldrne	r1, 480b
				str		r1, 2401f
				str		r1, 2402f
				str		r1, 2403f
				str		r1, 2404f
.if INTERWORK
				bx		lr
.else
				mov		pc, lr
.endif

makemix_mixtext40:
				stmdb	sp!, {r5 - r7, lr}
				adr		r5, to256colex
				ldr		lr, textmask
1:				ldr		r6, [r0, #0]
				ldr		r7, [r0, #4]
				ldrb	r12, [r1]
				bic		r6, r6, lr
				ldr		r12, [r5, r12, lsl #2]
				bic		r7, r7, lr
				and		r3, r12, lr					@ bit0
				and		r12, r12, lr, lsl #1		@ bit3
				orr		r6, r6, r3
				ldrb	r3, [r1, #(MAKETEXT_ROW * 1)]
				orr		r7, r7, r12, lsr #3
				ldr		r3, [r5, r3, lsl #2]
				add		r1, r1, #1
				and		r12, r3, lr					@ bit0
				and		r3, r3, lr, lsl #1			@ bit3
				orr		r6, r6, r12, lsl #1
				ldrb	r12, [r1, #((MAKETEXT_ROW * 2) - 1)]
				orr		r7, r7, r3, lsr #2
				ldr		r12, [r5, r12, lsl #2]
				and		r3, r12, lr					@ bit0
				and		r12, r12, lr, lsl #1		@ bit3
				orr		r6, r6, r3, lsl #2
				orr		r7, r7, r12, lsr #1
				subs	r2, r2, #1
				str		r6, [r0, #0]
				str		r7, [r0, #4]
2401:			add		r0, r0, #MAKESCRN_VIDEOYALIGN
				bne		1b
.if INTERWORK
				ldmia	sp!, {r5 - r7, lr}
				bx		lr
.else
				ldmia	sp!, {r5 - r7, pc}
.endif

makemix_mixgrph40:
				stmdb	sp!, {r5 - r8, lr}
				adr		r5, to256colex
				ldr		lr, textmask
				mov		r8, #0
1:				ldr		r6, [r0, #0]
				ldr		r7, [r0, #4]
				ldrb	r12, [r1, r8]
				and		r6, r6, lr
				and		r7, r7, lr
				ldr		r12, [r5, r12, lsl #2]
				add		r8, r8, #0x4000
				and		r3, r12, lr					@ bit0
				and		r12, r12, lr, lsl #1		@ bit3
				orr		r6, r6, r3, lsl #3
				ldrb	r3, [r1, r8]
				orr		r7, r7, r12
				ldr		r3, [r5, r3, lsl #2]
				add		r8, r8, #0x4000
				and		r12, r3, lr					@ bit0
				and		r3, r3, lr, lsl #1			@ bit3
				orr		r6, r6, r12, lsl #4
				ldrb	r12, [r1, r8]
				orr		r7, r7, r3, lsl #1
				ldr		r12, [r5, r12, lsl #2]
				and		r3, r12, lr					@ bit0
				and		r12, r12, lr, lsl #1		@ bit3
				orr		r6, r6, r3, lsl #5
				orr		r7, r7, r12, lsl #2
				add		r8, r8, #0x0800
				bic		r8, r8, #0xc000
				str		r6, [r0, #0]
				str		r7, [r0, #4]
				subs	r2, r2, #1
2402:			add		r0, r0, #MAKESCRN_VIDEOYALIGN
				bne		1b
.if INTERWORK
				ldmia	sp!, {r5 - r8, lr}
				bx		lr
.else
				ldmia	sp!, {r5 - r8, pc}
.endif

makemix_mixtext80:
				stmdb	sp!, {r5 - r8, lr}
				adr		r5, to256colex
				ldr		lr, textmask
				mov		r8, #MAKESCRN_VIDEOPAGE
1:				ldr		r6, [r0, #0]
				ldr		r7, [r0, r8]
				ldrb	r12, [r1]
				bic		r6, r6, lr
				ldr		r12, [r5, r12, lsl #2]
				bic		r7, r7, lr
				and		r3, r12, lr, lsl #4			@ bit4
				and		r12, r12, lr, lsl #5		@ bit7
				orr		r6, r6, r3, lsr #4
				ldrb	r3, [r1, #(MAKETEXT_ROW * 1)]
				orr		r7, r7, r12, lsr #7
				ldr		r3, [r5, r3, lsl #2]
				add		r1, r1, #1
				and		r12, r3, lr, lsl #4			@ bit4
				and		r3, r3, lr, lsl #5			@ bit7
				orr		r6, r6, r12, lsr #3
				ldrb	r12, [r1, #((MAKETEXT_ROW * 2) - 1)]
				orr		r7, r7, r3, lsr #6
				ldr		r12, [r5, r12, lsl #2]
				and		r3, r12, lr, lsl #4			@ bit0
				and		r12, r12, lr, lsl #5		@ bit7
				orr		r6, r6, r3, lsr #2
				orr		r7, r7, r12, lsr #5
				subs	r2, r2, #1
				str		r6, [r0, #0]
				str		r7, [r0, r8]
2403:			add		r0, r0, #MAKESCRN_VIDEOYALIGN
				bne		1b
.if INTERWORK
				ldmia	sp!, {r5 - r8, lr}
				bx		lr
.else
				ldmia	sp!, {r5 - r8, pc}
.endif

makemix_mixgrph80:
				stmdb	sp!, {r5 - r9, lr}
				adr		r5, to256colex
				ldr		lr, textmask
				mov		r9, #0
				mov		r8, #MAKESCRN_VIDEOPAGE
1:				ldr		r6, [r0, #0]
				ldr		r7, [r0, r8]
				ldrb	r12, [r1, r9]
				and		r6, r6, lr
				and		r7, r7, lr
				ldr		r12, [r5, r12, lsl #2]
				add		r9, r9, #0x4000
				and		r3, r12, lr, lsl #4			@ bit4
				and		r12, r12, lr, lsl #5		@ bit7
				orr		r6, r6, r3, lsr #1			@ 4->3
				ldrb	r3, [r1, r9]
				orr		r7, r7, r12, lsr #4			@ 7->3
				ldr		r3, [r5, r3, lsl #2]
				add		r9, r9, #0x4000
				and		r12, r3, lr, lsl #4			@ bit4
				and		r3, r3, lr, lsl #5			@ bit7
				orr		r6, r6, r12					@ 4->4
				ldrb	r12, [r1, r9]
				orr		r7, r7, r3, lsr #3			@ 7->4
				ldr		r12, [r5, r12, lsl #2]
				and		r3, r12, lr, lsl #4			@ bit4
				and		r12, r12, lr, lsl #5		@ bit7
				orr		r6, r6, r3, lsl #1			@ 4->5
				orr		r7, r7, r12, lsr #2			@ 7->5
				add		r9, r9, #0x0800
				bic		r9, r9, #0xc000
				str		r6, [r0, #0]
				str		r7, [r0, r8]
				subs	r2, r2, #1
2404:			add		r0, r0, #MAKESCRN_VIDEOYALIGN
				bne		1b
.if INTERWORK
				ldmia	sp!, {r5 - r9, lr}
				bx		lr
.else
				ldmia	sp!, {r5 - r9, pc}
.endif

to256colex:
		.word	0x00000000, 0x18000000, 0x80080000, 0x98080000
		.word	0x00800800, 0x18800800, 0x80880800, 0x98880800
		.word	0x00100008, 0x18100008, 0x80180008, 0x98180008
		.word	0x00900808, 0x18900808, 0x80980808, 0x98980808
		.word	0x01001000, 0x19001000, 0x81081000, 0x99081000
		.word	0x01801800, 0x19801800, 0x81881800, 0x99881800
		.word	0x01101008, 0x19101008, 0x81181008, 0x99181008
		.word	0x01901808, 0x19901808, 0x81981808, 0x99981808
		.word	0x00018000, 0x18018000, 0x80098000, 0x98098000
		.word	0x00818800, 0x18818800, 0x80898800, 0x98898800
		.word	0x00118008, 0x18118008, 0x80198008, 0x98198008
		.word	0x00918808, 0x18918808, 0x80998808, 0x98998808
		.word	0x01019000, 0x19019000, 0x81099000, 0x99099000
		.word	0x01819800, 0x19819800, 0x81899800, 0x99899800
		.word	0x01119008, 0x19119008, 0x81199008, 0x99199008
		.word	0x01919808, 0x19919808, 0x81999808, 0x99999808
		.word	0x00000180, 0x18000180, 0x80080180, 0x98080180
		.word	0x00800980, 0x18800980, 0x80880980, 0x98880980
		.word	0x00100188, 0x18100188, 0x80180188, 0x98180188
		.word	0x00900988, 0x18900988, 0x80980988, 0x98980988
		.word	0x01001180, 0x19001180, 0x81081180, 0x99081180
		.word	0x01801980, 0x19801980, 0x81881980, 0x99881980
		.word	0x01101188, 0x19101188, 0x81181188, 0x99181188
		.word	0x01901988, 0x19901988, 0x81981988, 0x99981988
		.word	0x00018180, 0x18018180, 0x80098180, 0x98098180
		.word	0x00818980, 0x18818980, 0x80898980, 0x98898980
		.word	0x00118188, 0x18118188, 0x80198188, 0x98198188
		.word	0x00918988, 0x18918988, 0x80998988, 0x98998988
		.word	0x01019180, 0x19019180, 0x81099180, 0x99099180
		.word	0x01819980, 0x19819980, 0x81899980, 0x99899980
		.word	0x01119188, 0x19119188, 0x81199188, 0x99199188
		.word	0x01919988, 0x19919988, 0x81999988, 0x99999988
		.word	0x00000011, 0x18000011, 0x80080011, 0x98080011
		.word	0x00800811, 0x18800811, 0x80880811, 0x98880811
		.word	0x00100019, 0x18100019, 0x80180019, 0x98180019
		.word	0x00900819, 0x18900819, 0x80980819, 0x98980819
		.word	0x01001011, 0x19001011, 0x81081011, 0x99081011
		.word	0x01801811, 0x19801811, 0x81881811, 0x99881811
		.word	0x01101019, 0x19101019, 0x81181019, 0x99181019
		.word	0x01901819, 0x19901819, 0x81981819, 0x99981819
		.word	0x00018011, 0x18018011, 0x80098011, 0x98098011
		.word	0x00818811, 0x18818811, 0x80898811, 0x98898811
		.word	0x00118019, 0x18118019, 0x80198019, 0x98198019
		.word	0x00918819, 0x18918819, 0x80998819, 0x98998819
		.word	0x01019011, 0x19019011, 0x81099011, 0x99099011
		.word	0x01819811, 0x19819811, 0x81899811, 0x99899811
		.word	0x01119019, 0x19119019, 0x81199019, 0x99199019
		.word	0x01919819, 0x19919819, 0x81999819, 0x99999819
		.word	0x00000191, 0x18000191, 0x80080191, 0x98080191
		.word	0x00800991, 0x18800991, 0x80880991, 0x98880991
		.word	0x00100199, 0x18100199, 0x80180199, 0x98180199
		.word	0x00900999, 0x18900999, 0x80980999, 0x98980999
		.word	0x01001191, 0x19001191, 0x81081191, 0x99081191
		.word	0x01801991, 0x19801991, 0x81881991, 0x99881991
		.word	0x01101199, 0x19101199, 0x81181199, 0x99181199
		.word	0x01901999, 0x19901999, 0x81981999, 0x99981999
		.word	0x00018191, 0x18018191, 0x80098191, 0x98098191
		.word	0x00818991, 0x18818991, 0x80898991, 0x98898991
		.word	0x00118199, 0x18118199, 0x80198199, 0x98198199
		.word	0x00918999, 0x18918999, 0x80998999, 0x98998999
		.word	0x01019191, 0x19019191, 0x81099191, 0x99099191
		.word	0x01819991, 0x19819991, 0x81899991, 0x99899991
		.word	0x01119199, 0x19119199, 0x81199199, 0x99199199
		.word	0x01919999, 0x19919999, 0x81999999, 0x99999999

	.end

