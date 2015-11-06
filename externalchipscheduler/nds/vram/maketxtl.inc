
@ r5 = r0
@ r4 = makescrn
@ r7 = src mask
@ r6 = work
@ r12 = next pc

_txt8_nor:		mov		r3, #0							@ ÉmÅ[É}Éã
1:
				ldr		r6, [r3, r2]
				add		r3, r3, #4
				str		r6, [r5]
				add		r5, r5, #4
				and		r3, r7
				cmp		r5, r1
				bcc		1b
_draw_nop:		bx		r12

_txt8_x2left:	mov		r3, #0							@ â°î{äpç∂
				add		r4, #MAKESCRN_PATX2
1:				ldrb	r6, [r3, r2]
				lsr		r6, r6, #4
				add		r3, r3, #1
				ldrb	r6, [r6, r4]
				and		r3, r7
				strb	r6, [r5]
				add		r5, r5, #1
				cmp		r5, r1
				bcc		1b
				sub		r4, #MAKESCRN_PATX2
				bx		r12

_txt8_x2right:	mov		r3, #0							@ â°î{äpâE
				add		r4, #MAKESCRN_PATX2
1:				ldrb	r6, [r3, r2]
				add		r3, r3, #1
				ldrb	r6, [r6, r4]
				and		r3, r7
				strb	r6, [r5]
				add		r5, r5, #1
				cmp		r5, r1
				bcc		1b
				sub		r4, #MAKESCRN_PATX2
				bx		r12

_txt8_Yx2:		ldr		r3, [r4, #MAKESCRN_FONTYCNT]	@ ècî{äpè„
				lsr		r3, r3, #1
1:				ldr		r6, [r3, r2]
				add		r3, r3, #1
				strb	r6, [r5, #0]
				strb	r6, [r5, #1]
				add		r5, r5, #2
				and		r3, r7
				cmp		r5, r1
				bcc		1b
				bx		r12

_txt8_x4left:	ldr		r3, [r4, #MAKESCRN_FONTYCNT]	@ ÇSî{äpç∂
				add		r4, #MAKESCRN_PATX2
				lsr		r3, r3, #1
1:				ldrb	r6, [r3, r2]
				lsr		r6, r6, #4
				add		r3, r3, #1
				ldrb	r6, [r6, r4]
				and		r3, r7
				strb	r6, [r5, #0]
				strb	r6, [r5, #1]
				add		r5, r5, #2
				cmp		r5, r1
				bcc		1b
				sub		r4, #MAKESCRN_PATX2
				bx		r12

_txt8_x4right:	ldr		r3, [r4, #MAKESCRN_FONTYCNT]	@ ÇSî{äpâE
				add		r4, #MAKESCRN_PATX2
				lsr		r3, r3, #1
1:				ldrb	r6, [r3, r2]
				add		r3, r3, #1
				ldrb	r6, [r6, r4]
				and		r3, r7
				strb	r6, [r5, #0]
				strb	r6, [r5, #1]
				add		r5, r5, #2
				cmp		r5, r1
				bcc		1b
				sub		r4, #MAKESCRN_PATX2
				bx		r12

_txt8_right4dot:mov		r3, #0							@ âEÇSÉhÉbÉg
1:				ldrb	r6, [r3, r2]
				add		r3, r3, #1
				lsl		r6, r6, #4
				strb	r6, [r5]
				and		r3, r7
				add		r5, r5, #1
				cmp		r5, r1
				bcc		1b
				bx		r12

_txt8_right4half:										@ í◊ÇÍâEÇSÉhÉbÉg
				ldr		r3, [r4, #MAKESCRN_FONTYCNT]
				lsr		r3, r3, #1
				beq		1f
				ldrb	r6, [r3, r2]
				mov		r3, #0
				b		2f
1:				ldrb	r6, [r3, r2]
				add		r3, r3, #1
				and		r3, r7
2:				lsl		r6, r6, #4
				strb	r6, [r5, #0]
				strb	r6, [r5, #1]
				add		r5, r5, #2
				cmp		r5, r1
				bcc		1b
				bx		r12

_txt8_right4x2:	ldr		r3, [r4, #MAKESCRN_FONTYCNT]	@ âEÇSÉhÉbÉgècî{äpè„
				lsr		r3, r3, #1
1:				ldrb	r6, [r3, r2]
				add		r3, r3, #1
				lsl		r6, r6, #4
				strb	r6, [r5, #0]
				strb	r6, [r5, #1]
				add		r5, r5, #2
				and		r3, r7
				cmp		r5, r1
				bcc		1b
				bx		r12

_txt8_halfx2:	ldr		r3, [r4, #MAKESCRN_FONTYCNT]	@ í◊ÇµÇƒècî{äp
				lsr		r3, r3, #1
				beq		1f
				ldrb	r6, [r3, r2]
				mov		r3, #0
				b		2f
1:				ldrb	r6, [r3, r2]
				add		r3, r3, #2
				and		r3, r7
2:				strb	r6, [r5, #0]
				strb	r6, [r5, #1]
				add		r5, r5, #2
				cmp		r5, r1
				bcc		1b
				bx		r12

_txt8_halfx4left:										@ í◊ÇµÇƒÇSî{äpç∂
				ldr		r3, [r4, #MAKESCRN_FONTYCNT]
				add		r4, #MAKESCRN_PATX2
				lsr		r3, r3, #1
				beq		1f
				ldrb	r6, [r3, r2]
				mov		r3, #0
				b		2f
1:				ldrb	r6, [r3, r2]
				add		r3, r3, #2
2:				lsr		r6, r6, #4
				ldrb	r6, [r6, r4]
				and		r3, r7
				strb	r6, [r5, #0]
				strb	r6, [r5, #1]
				add		r5, r5, #2
				cmp		r5, r1
				bcc		1b
				sub		r4, #MAKESCRN_PATX2
				bx		r12

_txt8_halfx4right:										@ í◊ÇµÇƒÇSî{äpâE
				ldr		r3, [r4, #MAKESCRN_FONTYCNT]
				add		r4, #MAKESCRN_PATX2
				lsr		r3, r3, #1
				beq		1f
				ldrb	r6, [r3, r2]
				mov		r3, #0
				b		2f
1:				ldrb	r6, [r3, r2]
				add		r3, r3, #2
2:				ldrb	r6, [r6, r4]
				and		r3, r7
				strb	r6, [r5, #0]
				strb	r6, [r5, #1]
				add		r5, r5, #2
				cmp		r5, r1
				bcc		1b
				sub		r4, #MAKESCRN_PATX2
				bx		r12


; @ ----

_knj8_nor:		mov		r3, #0							@ ÉmÅ[É}Éã
1:				ldrb	r6, [r3, r2]
				add		r3, r3, #2
				strb	r6, [r5]
				add		r5, r5, #1
				and		r3, r7
				cmp		r5, r1
				bcc		1b
				bx		r12

_knj8_x2left:	mov		r3, #0							@ â°î{äpç∂
				add		r4, #MAKESCRN_PATX2
1:				ldrb	r6, [r3, r2]
				lsr		r6, r6, #4
				add		r3, r3, #2
				ldrb	r6, [r6, r4]
				and		r3, r7
				strb	r6, [r5]
				add		r5, r5, #1
				cmp		r5, r1
				bcc		1b
				sub		r4, #MAKESCRN_PATX2
				bx		r12

_knj8_x2right:	mov		r3, #0							@ â°î{äpâE
				add		r4, #MAKESCRN_PATX2
1:				ldrb	r6, [r3, r2]
				add		r3, r3, #2
				ldrb	r6, [r6, r4]
				and		r3, r7
				strb	r6, [r5]
				add		r5, r5, #1
				cmp		r5, r1
				bcc		1b
				sub		r4, #MAKESCRN_PATX2
				bx		r12

_knj8_Yx2:		ldr		r3, [r4, #MAKESCRN_FONTYCNT]	@ ècî{äpè„
1:				ldr		r6, [r3, r2]
				add		r3, r3, #1
				strb	r6, [r5]
				add		r5, r5, #1
				and		r3, r7
				cmp		r5, r1
				bcc		1b
				bx		r12

_knj8_x4left:	ldr		r3, [r4, #MAKESCRN_FONTYCNT]	@ ÇSî{äpç∂
				add		r4, #MAKESCRN_PATX2
1:				ldrb	r6, [r3, r2]
				lsr		r6, r6, #4
				add		r3, r3, #1
				ldrb	r6, [r6, r4]
				and		r3, r7
				strb	r6, [r5]
				add		r5, r5, #1
				cmp		r5, r1
				bcc		1b
				sub		r4, #MAKESCRN_PATX2
				bx		r12

_knj8_x4right:	ldr		r3, [r4, #MAKESCRN_FONTYCNT]	@ ÇSî{äpâE
				add		r4, #MAKESCRN_PATX2
1:				ldrb	r6, [r3, r2]
				add		r3, r3, #1
				ldrb	r6, [r6, r4]
				and		r3, r7
				strb	r6, [r5]
				add		r5, r5, #1
				cmp		r5, r1
				bcc		1b
				sub		r4, #MAKESCRN_PATX2
				bx		r12

_knj8_right4dot:mov		r3, #0							@ âEÇSÉhÉbÉg
1:				ldrb	r6, [r3, r2]
				add		r3, r3, #2
				lsl		r6, r6, #4
				strb	r6, [r5]
				and		r3, r7
				add		r5, r5, #1
				cmp		r5, r1
				bcc		1b
				bx		r12

_knj8_right4half:										@ í◊ÇÍâEÇSÉhÉbÉg
				ldr		r3, [r4, #MAKESCRN_FONTYCNT]
				cmp		r3, #0
				beq		1f
				ldrb	r6, [r3, r2]
				mov		r3, #0
				b		2f
1:				ldrb	r6, [r3, r2]
				add		r3, r3, #2
				and		r3, r7
2:				lsl		r6, r6, #4
				strb	r6, [r5, #0]
				strb	r6, [r5, #1]
				add		r5, r5, #2
				cmp		r5, r1
				bcc		1b
				bx		r12

_knj8_right4x2:	ldr		r3, [r4, #MAKESCRN_FONTYCNT]	@ âEÇSÉhÉbÉgècî{äpè„
1:				ldrb	r6, [r3, r2]
				add		r3, r3, #1
				lsl		r6, r6, #4
				strb	r6, [r5, #0]
				add		r5, r5, #1
				and		r3, r7
				cmp		r5, r1
				bcc		1b
				bx		r12

_knj8_halfx2:	ldr		r3, [r4, #MAKESCRN_FONTYCNT]	@ í◊ÇµÇƒècî{äp
				cmp		r3, #0
				beq		1f
				ldrb	r6, [r3, r2]
				mov		r3, #0
				b		2f
1:				ldrb	r6, [r3, r2]
				add		r3, r3, #4
				and		r3, r7
2:				strb	r6, [r5, #0]
				strb	r6, [r5, #1]
				add		r5, r5, #2
				cmp		r5, r1
				bcc		1b
				bx		r12

_knj8_halfx4left:										@ í◊ÇµÇƒÇSî{äpç∂
				ldr		r3, [r4, #MAKESCRN_FONTYCNT]
				add		r4, #MAKESCRN_PATX2
				cmp		r3, #0
				beq		1f
				ldrb	r6, [r3, r2]
				mov		r3, #0
				b		2f
1:				ldrb	r6, [r3, r2]
				add		r3, r3, #4
2:				lsr		r6, r6, #4
				ldrb	r6, [r6, r4]
				and		r3, r7
				strb	r6, [r5, #0]
				strb	r6, [r5, #1]
				add		r5, r5, #2
				cmp		r5, r1
				bcc		1b
				sub		r4, #MAKESCRN_PATX2
				bx		r12

_knj8_halfx4right:										@ í◊ÇµÇƒÇSî{äpâE
				ldr		r3, [r4, #MAKESCRN_FONTYCNT]
				add		r4, #MAKESCRN_PATX2
				cmp		r3, #0
				beq		1f
				ldrb	r6, [r3, r2]
				mov		r3, #0
				b		2f
1:				ldrb	r6, [r3, r2]
				add		r3, r3, #4
2:				ldrb	r6, [r6, r4]
				and		r3, r7
				strb	r6, [r5, #0]
				strb	r6, [r5, #1]
				add		r5, r5, #2
				cmp		r5, r1
				bcc		1b
				sub		r4, #MAKESCRN_PATX2
				bx		r12

