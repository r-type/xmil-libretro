
	.include	"./nds/ramptr.inc"
	.include	"../z80ands/z80a.inc"
	.include	"./patch/iocore.inc"
	.include	"./vram/makescrn.inc"

	.global	_out_byte_a
	.global	_in_a_byte
	.global	_outx
	.global	_otxr
	.global	_out_c_0
	.global	_out_c_a
	.global	_out_c_r

	.global	fastioout
	.global	_fastioinp

	.extern	iocore
	.extern	crtc
	.extern	tram

	.section	.itcm
	.code	32
	.align	2


_out_byte_a:	Z80WORK	#7
				GETPC8
				and		r2, rAF, #(0xff << 8)
				mov		r1, r0
				orr		r0, r2, r0
				b		fastioout

_in_a_byte:		Z80WORK	#7
				GETPC8
				and		r1, rAF, #(0xff << 8)
				orr		r0, r1, r0
				Z80INP
				and		r1, rAF, #0xff
				orr		rAF, r1, r0, lsl #8
				mov		pc, r11


			; @	1010x011
_outx:			Z80WORK	#12
				ldrh	r2, [r4, #CPU_HL]
				ldrh	r3, [r4, #CPU_BC]
				MEMRD8	r1, r2
				tst		r0, #0x08
				addeq	r2, r2, #1
				subne	r2, r2, #1
				subs	r0, r3, #(1 << 8)
				addcc	r0, r0, #(0x100 << 8)
				strh	r2, [r4, #CPU_HL]
				strh	r0, [r4, #CPU_BC]
				and		rAF, rAF, #(0xff << 8)
				tst		r0, #0xff00
				orr		rAF, rAF, #N_FLAG
				orreq	rAF, rAF, #Z_FLAG
				b		fastioout

			; @	1011x011
_otxr:			Z80WORK	#12
				ldrh	r2, [r4, #CPU_HL]
				ldrh	r3, [r4, #CPU_BC]
				MEMRD8	r1, r2
				tst		r0, #0x08
				addeq	r2, r2, #1
				subne	r2, r2, #1
				subs	r0, r3, #(1 << 8)
				addcc	r0, r0, #(0x100 << 8)
				strh	r2, [r4, #CPU_HL]
				strh	r0, [r4, #CPU_BC]
				and		rAF, rAF, #(0xff << 8)
				tst		r0, #0xff00
				orr		rAF, rAF, #N_FLAG
				subne	rREMCLOCK, rREMCLOCK, #5
				orreq	rAF, rAF, #Z_FLAG
				subne	r8, r8, #(2 << 16)
				b		fastioout


_out_c_0:		Z80WORK	#8
				mov		r1, #0
				ldrh	r0, [r4, #CPU_BC]
				b		fastioout

_out_c_a:		Z80WORK	#8
				mov		r1, rAF, lsr #8
				ldrh	r0, [r4, #CPU_BC]
				b		fastioout

_out_c_r:		Z80WORK	#8
				eor		r2, r0, #0x49			; @ 01???001
				ldrh	r0, [r4, #CPU_BC]
				ldrb	r1, [r4, r2, lsr #3]

				; @ r0, r1, ret->r11
fastioout:
				add		r2, r4, #IWRAM_IOCORE
				and		r12, r0, #0xff00
				ldrb	r3, [r2, #IOCORE_MODE]
				cmp		r3, #0
				bne		20f
				subs	r3, r0, #0x4000
				bcs		10f
				ldr		r3, [r2, r12, lsr #6]
				str		rREMCLOCK, [r4, #_CPU_REMCLOCK]
.if INTERWORK
				blx		r3
.else
				bl		r3
.endif
				ldr		rREMCLOCK, [r4, #_CPU_REMCLOCK]
				mov		pc, r11

10:				ldr		r0, [r2, #(IOCORE2CRTC + CRTCE_GRAMACC)]
				ldr		r12, [r2, #(IOCORE2CRTC + _CRTCE_UPDATEBIT)]
				strb	r1, [r0, r3]
				and		r3, r3, r12, lsr #16
				add		r0, r2, #IOCORE2VRAMUPD
				ldrb	r1, [r0, r3]
				orr		r1, r1, r12
				strb	r12, [r2, #(IOCORE2CRTC + CRTCE_SCRNFLASH)]
				strb	r1, [r0, r3]
				mov		pc, r11

20:				ldr		r12, [r2, #(IOCORE2CRTC + CRTCE_GRAMACC)]
				bic		r3, r0, #0xc000
				mov		r0, r0, lsr #14
				cmp		r0, #1
				strneb	r1, [r3, r12]
				add		r3, r3, #0x4000
				cmp		r0, #2
				strneb	r1, [r3, r12]
				add		r3, r3, #0x4000
				cmp		r0, #3
				strneb	r1, [r3, r12]
				ldr		r1, [r2, #(IOCORE2CRTC + _CRTCE_UPDATEBIT)]
				and		r12, r3, r1, lsr #16
				add		r3, r2, #IOCORE2VRAMUPD
				strb	r1, [r2, #(IOCORE2CRTC + CRTCE_SCRNFLASH)]
				ldrb	r0, [r3, r12]
				orr		r0, r0, r1
				strb	r0, [r3, r12]
				mov		pc, r11

_fastioinp:		add		r2, r4, #IWRAM_IOCORE
				subs	r3, r0, #0x4000
				strb	r4, [r2, #IOCORE_MODE]
				bcs		100f

				bic		r3, r3, #0xff
				str		rREMCLOCK, [r4, #_CPU_REMCLOCK]
.if INTERWORK
				ldr		r3, [r2, r3, asr #6]
				bx		r3
.else
				ldr		pc, [r2, r3, asr #6]
.endif

100:			ldr		r1, [r2, #(IOCORE2CRTC + CRTCE_GRAMACC)]
				ldrb	r0, [r1, r3]
				add		pc, lr, #4

	.end

