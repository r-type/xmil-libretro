
.macro	MOVM1	reg
			mov		\reg, #0
			mvn		\reg, \reg
.endm


_atrb0:			mov		r3, #0
1:				str		r3, [r0, #(MAKETEXT_ROW * 0)]
				str		r3, [r0, #(MAKETEXT_ROW * 1)]
				str		r3, [r0, #(MAKETEXT_ROW * 2)]
				add		r0, r0, #4
				cmp		r0, r1
				bcc		1b
				bx		lr

_atrb1:			mov		r2, #0
1:				str		r2, [r0, #(MAKETEXT_ROW * 1)]
				str		r2, [r0, #(MAKETEXT_ROW * 2)]
				add		r0, r0, #4
				cmp		r0, r1
				bcc		1b
				bx		lr

_atrb2:			mov		r2, #0
1:				ldr		r3, [r0, #(MAKETEXT_ROW * 0)]
				str		r2, [r0, #(MAKETEXT_ROW * 0)]
				str		r3, [r0, #(MAKETEXT_ROW * 1)]
				str		r2, [r0, #(MAKETEXT_ROW * 2)]
				add		r0, r0, #4
				cmp		r0, r1
				bcc		1b
				bx		lr

_atrb3:			mov		r2, #0
1:				ldr		r3, [r0, #(MAKETEXT_ROW * 0)]
				str		r3, [r0, #(MAKETEXT_ROW * 1)]
				str		r2, [r0, #(MAKETEXT_ROW * 2)]
				add		r0, r0, #4
				cmp		r0, r1
				bcc		1b
				bx		lr

_atrb4:			mov		r2, #0
1:				ldr		r3, [r0, #(MAKETEXT_ROW * 0)]
				str		r2, [r0, #(MAKETEXT_ROW * 0)]
				str		r2, [r0, #(MAKETEXT_ROW * 1)]
				str		r3, [r0, #(MAKETEXT_ROW * 2)]
				add		r0, r0, #4
				cmp		r0, r1
				bcc		1b
				bx		lr

_atrb5:			mov		r2, #0
1:				ldr		r3, [r0, #(MAKETEXT_ROW * 0)]
				str		r2, [r0, #(MAKETEXT_ROW * 1)]
				str		r3, [r0, #(MAKETEXT_ROW * 2)]
				add		r0, r0, #4
				cmp		r0, r1
				bcc		1b
				bx		lr

_atrb6:			mov		r2, #0
1:				ldr		r3, [r0, #(MAKETEXT_ROW * 0)]
				str		r2, [r0, #(MAKETEXT_ROW * 0)]
				str		r3, [r0, #(MAKETEXT_ROW * 1)]
				str		r3, [r0, #(MAKETEXT_ROW * 2)]
				add		r0, r0, #4
				cmp		r0, r1
				bcc		1b
				bx		lr

_atrb7:
1:				ldr		r3, [r0, #(MAKETEXT_ROW * 0)]
				str		r3, [r0, #(MAKETEXT_ROW * 1)]
				str		r3, [r0, #(MAKETEXT_ROW * 2)]
				add		r0, r0, #4
				cmp		r0, r1
				bcc		1b
				bx		lr


_atrb0r:		MOVM1	r2
1:				str		r2, [r0, #(MAKETEXT_ROW * 0)]
				str		r2, [r0, #(MAKETEXT_ROW * 1)]
				str		r2, [r0, #(MAKETEXT_ROW * 2)]
				add		r0, r0, #4
				cmp		r0, r1
				bcc		1b
				bx		lr

_atrb1r:		MOVM1	r2
1:				ldr		r3, [r0, #(MAKETEXT_ROW * 0)]
				str		r2, [r0, #(MAKETEXT_ROW * 1)]
				mvn		r3, r3
				str		r2, [r0, #(MAKETEXT_ROW * 2)]
				str		r3, [r0, #(MAKETEXT_ROW * 0)]
				add		r0, r0, #4
				cmp		r0, r1
				bcc		1b
				bx		lr

_atrb2r:		MOVM1	r2
1:				ldr		r3, [r0, #(MAKETEXT_ROW * 0)]
				str		r2, [r0, #(MAKETEXT_ROW * 0)]
				mvn		r3, r3
				str		r2, [r0, #(MAKETEXT_ROW * 2)]
				str		r3, [r0, #(MAKETEXT_ROW * 1)]
				add		r0, r0, #4
				cmp		r0, r1
				bcc		1b
				bx		lr

_atrb3r:		MOVM1	r2
1:				ldr		r3, [r0, #(MAKETEXT_ROW * 0)]
				str		r2, [r0, #(MAKETEXT_ROW * 2)]
				mvn		r3, r3
				str		r3, [r0, #(MAKETEXT_ROW * 0)]
				str		r3, [r0, #(MAKETEXT_ROW * 1)]
				add		r0, r0, #4
				cmp		r0, r1
				bcc		1b
				bx		lr

_atrb4r:		MOVM1	r2
1:				ldr		r3, [r0, #(MAKETEXT_ROW * 0)]
				str		r2, [r0, #(MAKETEXT_ROW * 0)]
				mvn		r3, r3
				str		r2, [r0, #(MAKETEXT_ROW * 1)]
				str		r3, [r0, #(MAKETEXT_ROW * 2)]
				add		r0, r0, #4
				cmp		r0, r1
				bcc		1b
				bx		lr

_atrb5r:		MOVM1	r2
1:				ldr		r3, [r0, #(MAKETEXT_ROW * 0)]
				str		r2, [r0, #(MAKETEXT_ROW * 1)]
				mvn		r3, r3
				str		r3, [r0, #(MAKETEXT_ROW * 1)]
				str		r3, [r0, #(MAKETEXT_ROW * 2)]
				add		r0, r0, #4
				cmp		r0, r1
				bcc		1b
				bx		lr

_atrb6r:		MOVM1	r2
1:				ldr		r3, [r0, #(MAKETEXT_ROW * 0)]
				str		r2, [r0, #(MAKETEXT_ROW * 0)]
				mvn		r3, r3
				str		r3, [r0, #(MAKETEXT_ROW * 1)]
				str		r3, [r0, #(MAKETEXT_ROW * 2)]
				add		r0, r0, #4
				cmp		r0, r1
				bcc		1b
				bx		lr

_atrb7r:
1:				ldr		r3, [r0, #(MAKETEXT_ROW * 0)]
				str		r3, [r0, #(MAKETEXT_ROW * 0)]
				str		r3, [r0, #(MAKETEXT_ROW * 1)]
				str		r3, [r0, #(MAKETEXT_ROW * 2)]
				add		r0, r0, #4
				cmp		r0, r1
				bcc		1b
				bx		lr

