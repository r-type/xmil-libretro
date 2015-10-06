
	.section	.text
	.code	32

	.global ndscore_timer3init
	.global	ndscore_timer3int
	.global ndscore_sleep
	.global	ndscore_rest
	.global ndscore_gettick

ndscore_sleep:	movs	r0, r0, lsl #9			; @ 256*256/125*ms

				@ 引き数はクロック数にする？
ndscore_rest:	rsb		r2, r0, #0

				mov		r0, #0x04000000
				mov		r1, #0xc1
				add		r3, r0, #0x200

				@ 割り込み停止
				strh	r0, [r3, #8]

				@ タイマ値更新
				strh	r0, [r3, #(0x10e - 0x200)]
				ldrh	r12, [r3, #(0x10c - 0x200)]
				strh	r2, [r3, #(0x10c - 0x200)]		@ set value
				strh	r1, [r3, #(0x10e - 0x200)]
				strh	r0, [r3, #(0x10c - 0x200)]		@ set reload value

				mov		r1, r2, lsl #16
				ldr		r0, counter_fract
				sub		r12, r12, r1, lsr #16	@ カウンタ設定差
				add		r0, r12, r0
				ldr		r1, counter_tick
				mov		r12, r0, asr #16
				rsb		r1, r12, r1				@ -1
				sub		r0, r0, r12, lsl #16
				add		r1, r1, r12, lsl #7		@ 128
				str		r0, counter_fract
				sub		r1, r1, r12, lsl #1		@ 126
				mov		r0, #1
				str		r1, counter_tick
				strh	r0, [r3, #8]			@ Go!

10:				mov		r12, r1
11:				mov		r0, #1
				mov		r1, #(1 << 6)
				swi		#0x040000
				ldr		r1, counter_tick
				cmp		r1, r12
				beq		11b
				adds	r2, r2, #(1 << 16)
				bcc		10b

				bx		lr


counter_tick:	.word		0
counter_fract:	.word		0


ndscore_gettick:ldr		r3, timer3
				ldr		r0, counter_tick
1:				ldrh	r1, [r3]
				mov		r12, r0
				ldr		r3, counter_fract
				ldr		r0, counter_tick
				cmp		r0, r12
				bne		1b
				add		r1, r3, r1
				rsb		r12, r1, r1, lsl #7		@ 127
				sub		r12, r12, r1, lsl #1	@ 125
				add		r0, r0, r12, asr #16
				bx		lr

timer3:		.word		0x0400010c

ndscore_timer3init:
				mov		r0, #0x04000000
				add		r2, r0, #0x200
				mov		r3, #0xc1
				strh	r0, [r2, #(0x10e - 0x200)]		@ ... TIMER3
				strh	r0, [r2, #(0x10c - 0x200)]
				strh	r3, [r2, #(0x10e - 0x200)]
				bx		lr

ndscore_timer3int:
				ldr		r0, counter_tick
				add		r0, r0, #125
				str		r0, counter_tick
				bx		lr


	.end

