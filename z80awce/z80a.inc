
C_FLAG				equ		&01
N_FLAG				equ		&02
V_FLAG				equ		&04
H_FLAG				equ		&10
Z_FLAG				equ		&40
S_FLAG				equ		&80

IFF_IFLAG			equ		0
IFF_NMI				equ		1
IFF_HALT			equ		2

CPU_SIZE			equ		(56 + 16)
CPU_STAT			equ		(0 - CPU_SIZE)
CPU_EXT				equ		(56 - CPU_SIZE)

CPU_BC				equ		CPU_STAT + 0
CPU_DE				equ		CPU_STAT + 2
CPU_HL				equ		CPU_STAT + 4
CPU_IX				equ		CPU_STAT + 6
CPU_IY				equ		CPU_STAT + 8
_CPU_AF				equ		CPU_STAT + 10			; shadow
_CPU_SP				equ		CPU_STAT + 12			; shadow
_CPU_PC				equ		CPU_STAT + 14			; shadow
CPU_AF2				equ		CPU_STAT + 16
CPU_BC2				equ		CPU_STAT + 18
CPU_DE2				equ		CPU_STAT + 20
CPU_HL2				equ		CPU_STAT + 22
_CPU_IFF			equ		CPU_STAT + 24			; shadow
; CPU_I				equ		CPU_STAT + 25
; CPU_IM			equ		CPU_STAT + 26
; CPU_R1			equ		CPU_STAT + 27
CPU_R2				equ		CPU_STAT + 28
CPU_IRQ				equ		CPU_STAT + 32
CPU_REQIRQ			equ		CPU_STAT + 36
CPU_REMCLOCK		equ		CPU_STAT + 40
CPU_BASECLOCK		equ		CPU_STAT + 44
; cpu_clock			equ		CPU_STAT + 48
; cpu_stat_size		equ		52

CPU_C				equ		CPU_STAT + 0
CPU_B				equ		CPU_STAT + 1
CPU_E				equ		CPU_STAT + 2
CPU_D				equ		CPU_STAT + 3
CPU_L				equ		CPU_STAT + 4
CPU_H				equ		CPU_STAT + 5
CPU_IXL				equ		CPU_STAT + 6
CPU_IXH				equ		CPU_STAT + 7
CPU_IYL				equ		CPU_STAT + 8
CPU_IYH				equ		CPU_STAT + 9
; CPU_F				equ		CPU_STAT + 10
; CPU_A				equ		CPU_STAT + 11

CPU_MEMREAD			equ		CPU_EXT + 0
CPU_MEMWRITE		equ		CPU_EXT + 4
CPU_MEMBASE			equ		CPU_EXT + 8
; cpu_ext_size		equ		12

CPU_SZPFLAG			equ		0
CPU_INCFLAG			equ		256
CPU_DECFLAG			equ		512



	MACRO
$label	Z80WORK	$c
$label		sub		r10, r10, $c
	MEND

	MACRO
$label	MEMRD8	$r, $m
$label		tst		$m, #&8000
			ldrne	r12, [r7, #CPU_MEMBASE]
			ldreq	r12, [r7, #CPU_MEMREAD]
			ldrb	$r, [r12, $m]
	MEND

	MACRO
$label	MEMWR8	$m, $v
$label		tst		$m, #&8000
			ldrne	r12, [r7, #CPU_MEMBASE]
			ldreq	r12, [r7, #CPU_MEMWRITE]
			strb	$v, [r12, $m]
	MEND

	MACRO
$label	MEMLEA8	$m, $s, $d
$label		tst		$m, #&8000
			ldrne	$s, [r7, #CPU_MEMBASE]
			ldreq	$s, [r7, #CPU_MEMREAD]
			movne	$d, $s
			ldreq	$d, [r7, #CPU_MEMWRITE]
	MEND

	MACRO
$label	GETPC8
$label		tst		r9, #(&8000 << 16)
			ldrne	r12, [r7, #CPU_MEMBASE]
			ldreq	r12, [r7, #CPU_MEMREAD]
			ldrb	r0, [r12, r9, lsr #16]
			add		r9, r9, #&10000
	MEND

	MACRO
$label	GETPC8S
$label		tst		r9, #(&8000 << 16)
			ldrne	r12, [r7, #CPU_MEMBASE]
			ldreq	r12, [r7, #CPU_MEMREAD]
			add		r0, r12, r9, lsr #16
			add		r9, r9, #&10000
			ldrsb	r0, [r0]
	MEND

	MACRO
$label	GETPC16
$label		mov		r0, r9, lsr #16
			add		r9, r9, #&20000
			bl		mem_read16
	MEND

	MACRO
$label	GETIPC	$r, $b
$label		tst		r9, #(&8000 << 16)
			ldrne	r12, [r7, #CPU_MEMBASE]
			ldreq	r12, [r7, #CPU_MEMREAD]
			ldrh	r0, [r7, #$b]
			ldrb	r1, [r12, r9, lsr #16]
			add		r9, r9, #&10000
			tst		r1, #&80
			addne	r1, r1, #&ff00
			add		r1, r1, r0
			bic		$r, r1, #&10000
	MEND


	END

