
%macro	Z80WORK		1
			sub		dword [edi + z80core_t.remainclock], %1
%endmacro

%macro	FETCH		0
			inc		cx
			inc		word [edi + R_Z80PC]
			movzx	ecx, cx
			call	memfetch
%endmacro

%macro	OPBYTE		0
			inc		cx
			inc		word [edi + R_Z80PC]
			movzx	ecx, cx
			call	memrd8_ecx_al
%endmacro

%macro	OPXHL		0
			movzx	ecx, word [edi + R_Z80HL]
			call	memrd8_ecx_al
%endmacro


%macro	GETPCAL		0
			inc		cx
			inc		word [edi + R_Z80PC]
			movzx	ecx, cx
			call	memrd8_ecx_al
%endmacro

%macro	GETPCDL		0
			inc		cx
			inc		word [edi + R_Z80PC]
			movzx	ecx, cx
			call	memrd8_ecx_dl
%endmacro

%macro	GETPCAX		0
			inc		cx
			add		word [edi + R_Z80PC], byte 2
			movzx	ecx, cx
			call	memrd16_ecx_ax
%endmacro


%macro	LDR8		2
			mov		al, [edi + %2]
			mov		[edi + %1], al
			ret
%endmacro

%macro	LDI8		1
			GETPCDL
			mov		[edi + %1], dl
			ret
%endmacro



%macro	LDR16		2
			mov		al, [edi + %2 + 0]
			mov		ah, [edi + %2 + 1]
			mov		[edi + %1 + 0], al
			mov		[edi + %1 + 1], ah
			ret
%endmacro


%macro	LDB_b		1
			OPBYTE
			mov		[edi + %1], al
			ret
%endmacro

%macro	LDB_x		2
			movzx	ecx, word [edi + %2]
			call	memrd8_ecx_al
			mov		[edi + %1], al
			ret
%endmacro

%macro	LDx_B		2
			movzx	ecx, word [edi + %1]
			mov		dl, [edi + %2]
			jmp		memwr8_ecx_dl
%endmacro


%macro	LDW_w		1
			GETPCAX
			mov		[edi + %1], ax
			ret
%endmacro

%macro	LDW_x		1
			GETPCAX
			movzx	ecx, ax
			call	memrd16_ecx_ax
			mov		[edi + %1], ax
			ret
%endmacro

%macro	LDx_W		1
			GETPCAX
			movzx	ecx, ax
			mov		dx, [edi + %1]
			jmp		memwr16_ecx_dx
%endmacro


%macro	INCR8		1
			mov		al, [edi + %1]
			mov		ah, [edi + R_Z80F]
			inc		al
			movzx	edx, al
			and		ah, C_FLAG
			or		ah, [_z80inc_flag + edx]
			mov		[edi + %1], al
			mov		[edi + R_Z80F], ah
			ret
%endmacro

%macro	INCM8		0
			call	memrd8_ecx_dl
			inc		dl
			mov		dh, [edi + R_Z80F]
			movzx	eax, dl
			and		dh, C_FLAG
			or		dh, [_z80inc_flag + eax]
			mov		[edi + R_Z80F], dh
			jmp		memwr8_ecx_dl
%endmacro

%macro	DECR8		1
			mov		al, [edi + %1]
			mov		ah, [edi + R_Z80F]
			dec		al
			movzx	edx, al
			and		ah, C_FLAG
			or		ah, [_z80dec_flag + edx]
			mov		[edi + %1], al
			mov		[edi + R_Z80F], ah
			ret
%endmacro

%macro	DECM8		0
			call	memrd8_ecx_dl
			dec		dl
			mov		dh, [edi + R_Z80F]
			movzx	eax, dl
			and		dh, C_FLAG
			or		dh, [_z80dec_flag + eax]
			mov		[edi + R_Z80F], dh
			jmp		memwr8_ecx_dl
%endmacro


%macro	MCRADD8		0
			mov		dl, [edi + R_Z80A]
			mov		dh, dl
			add		dh, al
			lahf
			seto	cl
			and		ah, (S_FLAG | Z_FLAG | C_FLAG)
			xor		dl, dh
			xor		dl, al
			and		dl, H_FLAG
			shl		cl, 2
			or		dl, cl
			or		dl, ah
			mov		[edi + R_Z80AF], dx
			ret
%endmacro

%macro	MCRADC8		0
			mov		dx, [edi + R_Z80AF]
			bt		dx, 0
			mov		dl, dh
			adc		dh, al
			lahf
			seto	cl
			and		ah, (S_FLAG | Z_FLAG | C_FLAG)
			xor		dl, dh
			xor		dl, al
			and		dl, H_FLAG
			shl		cl, 2
			or		dl, cl
			or		dl, ah
			mov		[edi + R_Z80AF], dx
			ret
%endmacro

%macro	MCRSUB8		0
			mov		dl, [edi + R_Z80A]
			mov		dh, dl
			sub		dh, al
			lahf
			seto	cl
			and		ah, (S_FLAG | Z_FLAG | C_FLAG)
			or		ah, N_FLAG
			xor		dl, dh
			xor		dl, al
			and		dl, H_FLAG
			shl		cl, 2
			or		dl, cl
			or		dl, ah
			mov		[edi + R_Z80AF], dx
			ret
%endmacro

%macro	MCRSBC8		0
			mov		dx, [edi + R_Z80AF]
			bt		dx, 0
			mov		dl, dh
			sbb		dh, al
			lahf
			seto	cl
			and		ah, (S_FLAG | Z_FLAG | C_FLAG)
			or		ah, N_FLAG
			xor		dl, dh
			xor		dl, al
			and		dl, H_FLAG
			shl		cl, 2
			or		dl, cl
			or		dl, ah
			mov		[edi + R_Z80AF], dx
			ret
%endmacro

%macro	MCRAND8		0
			and		al, [edi + R_Z80A]
			movzx	edx, al
			mov		ah, [_ZSPtable + edx]
			mov		[edi + R_Z80A], al
			mov		[edi + R_Z80F], ah
			ret
%endmacro

%macro	MCROR8		0
			or		al, [edi + R_Z80A]
			movzx	edx, al
			mov		ah, [_ZSPtable + edx]
			mov		[edi + R_Z80A], al
			mov		[edi + R_Z80F], ah
			ret
%endmacro

%macro	MCRXOR8		0
			xor		al, [edi + R_Z80A]
			movzx	edx, al
			mov		ah, [_ZSPtable + edx]
			mov		[edi + R_Z80A], al
			mov		[edi + R_Z80F], ah
			ret
%endmacro

%macro	MCRCP8		0
			mov		dl, [edi + R_Z80A]
			mov		dh, dl
			sub		dh, al
			lahf
			seto	cl
			and		ah, (S_FLAG | Z_FLAG | C_FLAG)
			or		ah, N_FLAG
			xor		dl, dh
			xor		dl, al
			and		dl, H_FLAG
			shl		cl, 2
			or		dl, cl
			or		dl, ah
			mov		[edi + R_Z80F], dl
			ret
%endmacro


%macro	ADDR8		1
			mov		al, [edi + %1]
			MCRADD8
%endmacro

%macro	ADCR8		1
			mov		al, [edi + %1]
			MCRADC8
%endmacro

%macro	SUBR8		1
			mov		al, [edi + %1]
			MCRSUB8
%endmacro

%macro	SBCR8		1
			mov		al, [edi + %1]
			MCRSBC8
%endmacro

%macro	ANDR8		1
			mov		al, [edi + %1]
			MCRAND8
%endmacro

%macro	ORR8		1
			mov		al, [edi + %1]
			MCROR8
%endmacro

%macro	XORR8		1
			mov		al, [edi + %1]
			MCRXOR8
%endmacro

%macro	CPR8		1
			mov		al, [edi + %1]
			MCRCP8
%endmacro



%macro	ADDR16		2
			mov		al, [edi + R_Z80F]
			and		al, (S_FLAG | Z_FLAG | V_FLAG)
			mov		dx, [edi + %1]
			mov		bx, dx
			mov		cx, [edi + %2]
			add		dx, cx
			adc		al, 0
			xor		ch, bh
			xor		ch, dh
			and		ch, H_FLAG
			or		al, ch
			mov		[edi + %1], dx
			mov		[edi + R_Z80F], al
			ret
%endmacro

%macro	ADDR16d		1
			mov		al, [edi + R_Z80F]
			and		al, (S_FLAG | Z_FLAG | V_FLAG)
			shl		word [edi + %1], 1
			adc		al, 0
			test	byte [edi + %1 + 1], H_FLAG
			je		short .noh
			or		al, H_FLAG
.noh:		mov		[edi + R_Z80F], al
			ret
%endmacro

%macro ADCR16	1
			mov		dx, [edi + R_Z80HL]
			mov		bx, dx
			mov		cx, [edi + %1]
			bt		dword [edi + R_Z80AF], 0
			adc		dx, cx
			lahf
			seto	al
			and		ah, (S_FLAG | Z_FLAG | C_FLAG)
			xor		bh, dh
			xor		bh, ch
			and		bh, H_FLAG
			shl		al, 2
			or		al, ah
			or		al, bh
			mov		[edi + R_Z80F], al
			mov		[edi + R_Z80HL], dx
			ret
%endmacro

%macro SBCR16	1
			mov		dx, [edi + R_Z80HL]
			mov		bx, dx
			mov		cx, [edi + %1]
			bt		dword [edi + R_Z80AF], 0
			sbb		dx, cx
			lahf
			seto	al
			and		ah, (S_FLAG | Z_FLAG | C_FLAG)
			mov		[edi + R_Z80HL], dx
			xor		dh, bh
			xor		dh, ch
			and		dh, H_FLAG
			shl		al, 2
			or		al, ah
			or		al, dh
			or		al, N_FLAG
			mov		[edi + R_Z80F], al
			ret
%endmacro


%macro	INCR16		1
			inc		word [edi + %1]
			ret
%endmacro

%macro	DECR16		1
			dec		word [edi + %1]
			ret
%endmacro

%macro	MCREXX		2
			mov		ax, [edi + %1]
			xchg	[edi + %2], ax
			mov		[edi + %1], ax
%endmacro

%macro	EXR16		2
			MCREXX	%1, %2
			ret
%endmacro

%macro	EXXSP		1
			movzx	ecx, word [edi + R_Z80SP]
			call	memrd16_ecx_ax
			xchg	[edi + %1], ax
			mov		dx, ax
			jmp		memwr16_ecx_dx
%endmacro

%macro	PUSH16		1
			sub		word [edi + R_Z80SP], byte 2
			mov		dx, [edi + %1]
			movzx	ecx, word [edi + R_Z80SP]
			jmp		memwr16_ecx_dx
%endmacro

%macro	POP16		1
			movzx	ecx, word [edi + R_Z80SP]
			add		word [edi + R_Z80SP], byte 2
			call	memrd16_ecx_ax
			mov		[edi + %1], ax
			ret
%endmacro


%macro	MCRRST		1
			sub		word [edi + R_Z80SP], byte 2
			mov		dx, %1
			xchg	[edi + R_Z80PC], dx
			movzx	ecx, word [edi + R_Z80SP]
			jmp		memwr16_ecx_dx
%endmacro


%macro	MCALL		0
			Z80WORK	byte 7
			inc		cx
			mov		dx, cx
			add		dx, byte 2
			call	memrd16_ecx_ax				; !!
			mov		[edi + R_Z80PC], ax
			sub		word [edi + R_Z80SP], byte 2
			movzx	ecx, word [edi + R_Z80SP]
			jmp		memwr16_ecx_dx
%endmacro

%macro	MCALLF		1
			test	byte [edi + R_Z80F], %1
			jne		short %%callop
			add		word [edi + R_Z80PC], byte 2
			ret
%%callop:	MCALL
%endmacro

%macro	MCALLNF		1
			test	byte [edi + R_Z80F], %1
			je		short %%callop
			add		word [edi + R_Z80PC], byte 2
			ret
%%callop:	MCALL
%endmacro


%macro MRETS		0
			movzx	ecx, word [edi + R_Z80SP]
			add		word [edi + R_Z80SP], 2
			call	memrd16_ecx_ax
			mov		[edi + R_Z80PC], ax
			Z80WORK	byte 6
%endmacro

%macro MRET			0
			MRETS
			ret
%endmacro

%macro	MRETF		1
			test	byte [edi + R_Z80F], %1
			je		short %%noret
			MRETS
%%noret:	ret
%endmacro

%macro	MRETNF		1
			test	byte [edi + R_Z80F], %1
			jne		short %%noret
			MRETS
%%noret:	ret
%endmacro


%macro	MCRJP		0
			inc		cx
			movzx	ecx, cx
			call	memrd16_ecx_ax
			mov		[edi + R_Z80PC], ax
			ret
%endmacro

%macro	MCRJPF		1
			test	byte [edi + R_Z80F], %1
			jne		short %%jp
			add		word [edi + R_Z80PC], byte 2
			ret
%%jp:		MCRJP
%endmacro

%macro	MCRJPNF		1
			test	byte [edi + R_Z80F], %1
			je		short %%jp
			add		word [edi + R_Z80PC], byte 2
			ret
%%jp:		MCRJP
%endmacro


%macro	MCRJR		0
			inc		cx
			movzx	ecx, cx
			call	memrd8_ecx_al
			cbw
			inc		ax
			add		[edi + R_Z80PC], ax
			Z80WORK	byte 5
			ret
%endmacro

%macro	MCRJRF		1
			test	byte [edi + R_Z80F], %1
			jne		short %%jr
			inc		word [edi + R_Z80PC]
			ret
%%jr:		MCRJR
%endmacro

%macro	MCRJRNF		1
			test	byte [edi + R_Z80F], %1
			je		short %%jr
			inc		word [edi + R_Z80PC]
			ret
%%jr:		MCRJR
%endmacro

%macro	MCRDJNZ		0
			dec		byte [edi + R_Z80B]
			jne		short %%jr
			inc		word [edi + R_Z80PC]
			ret
%%jr:		MCRJR
%endmacro


%macro	Z80OUT		1
			movzx	ecx, word [edi + R_Z80BC]
			mov		dl, [edi + %1]
			jmp		@iocore_out@8
%endmacro

%macro	Z80IN		1
			movzx	ecx, word [edi + R_Z80BC]
			call	@iocore_inp@4
			movzx	edx, al
			mov		[edi + %1], al
			mov		al, [edi + R_Z80F]
			and		al, C_FLAG
			or		al, [_ZSPtable + edx]
			mov		[edi + R_Z80F], al
			ret
%endmacro

