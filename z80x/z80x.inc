
				struc	reg8_t
.f				resb	1
.a				resb	1
.c				resb	1
.b				resb	1
.e				resb	1
.d				resb	1
.l				resb	1
.h				resb	1
.ixl			resb	1
.ixh			resb	1
.iyl			resb	1
.iyh			resb	1
				endstruc

				struc	reg16_t
.af				resw	1
.bc				resw	1
.de				resw	1
.hl				resw	1
.ix				resw	1
.iy				resw	1
				endstruc

				struc	z80core_t
.reg			resw	6
.pc				resw	1
.sp				resw	1
.af2			resw	1
.bc2			resw	1
.de2			resw	1
.hl2			resw	1
.i				resb	1
.im				resb	1
.r1				resb	1
.r2				resb	1
.iff			resw	1
				resw	1
.icount			resw	1
				resw	1
				endstruc

S_FLAG		equ		0x80
Z_FLAG		equ		0x40
H_FLAG		equ		0x10
V_FLAG		equ		0x04
N_FLAG		equ		0x02
C_FLAG		equ		0x01

IFF_IFLAG	equ		0
IFF_IRQ		equ		1
IFF_NMI		equ		2
IFF_HALT	equ		3

R_Z80A		equ		reg8_t.a
R_Z80F		equ		reg8_t.f
R_Z80B		equ		reg8_t.b
R_Z80C		equ		reg8_t.c
R_Z80D		equ		reg8_t.d
R_Z80E		equ		reg8_t.e
R_Z80H		equ		reg8_t.h
R_Z80L		equ		reg8_t.l

R_Z80AF		equ		reg16_t.af
R_Z80BC		equ		reg16_t.bc
R_Z80DE		equ		reg16_t.de
R_Z80HL		equ		reg16_t.hl
R_Z80IX		equ		reg16_t.ix
R_Z80IY		equ		reg16_t.iy

R_Z80PC		equ		z80core_t.pc
R_Z80SP		equ		z80core_t.sp
R_Z80AF2	equ		z80core_t.af2
R_Z80BC2	equ		z80core_t.bc2
R_Z80DE2	equ		z80core_t.de2
R_Z80HL2	equ		z80core_t.hl2

R_Z80I		equ		z80core_t.i
R_Z80IM		equ		z80core_t.im
R_Z80R		equ		z80core_t.r1
R_Z80R2		equ		z80core_t.r2
R_Z80IFF	equ		z80core_t.iff

