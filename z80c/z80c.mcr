//----------------------------------------------------------------------------
//
//  Z80C : Z80 Engine - GENERIC
//
//                                  Copyright by Studio Milmake 1999-2000,2004
//
//----------------------------------------------------------------------------


#define	__CBW(src)		(UINT16)((SINT8)(src))
#define	__CBD(src)		((SINT8)(src))
#define	WORD2LONG(src)	((SINT16)(src))

#define Z80_COUNT(clock)											\
	do {															\
		Z80_ICOUNT += (clock);										\
	} while (/*CONSTCOND*/ 0)

#define	Z80IRQCHECKTERM												\
	do { } while (/*CONSTCOND*/ 0)


#define	GET_PC_BYTE(b)												\
	do {															\
		(b) = Z80_RDMEM(R_Z80PC++);									\
	} while (/*CONSTCOND*/ 0)

#define	GET_PC_WORD(w)												\
	do {															\
		(w) = Z80_RDMEM_W(R_Z80PC);									\
		R_Z80PC += 2;												\
	} while (/*CONSTCOND*/ 0)


#define MCR_EX1(r1, r2)												\
	do {															\
		REG16 tmp;													\
		tmp = (r1);													\
		(r1) = (r2);												\
		(r2) = tmp;													\
	} while (/*CONSTCOND*/ 0)

#define JRBYFLAG(flg)												\
	do {															\
		if ((flg)) {												\
			UINT8 ads;												\
			ads = Z80_RDMEM(R_Z80PC++);								\
			R_Z80PC += __CBW(ads);									\
			Z80_COUNT(5);											\
		}															\
		else {														\
			R_Z80PC++;												\
		}															\
	} while (/*CONSTCOND*/ 0)



#define	LDW_w(reg) {												\
		(reg) = Z80_RDMEM_W(R_Z80PC);								\
		R_Z80PC += 2;												\
	}

#define	LDx_B(dst, src) {											\
		Z80_WRMEM((dst), (src));									\
	}

#define	MCR_INC_W(reg) {											\
		(reg)++;													\
	}

#define	MCR_INC(reg) {												\
		R_Z80F &= C_FLAG;											\
		(reg)++;													\
		R_Z80F |= z80inc_flag[(reg)];								\
	}

#define	MCR_DEC(reg) {												\
		R_Z80F &= C_FLAG;											\
		(reg)--;													\
		R_Z80F |= z80dec_flag[(reg)];								\
	}

#define	LDB_b(reg) {												\
		(reg) = Z80_RDMEM(R_Z80PC++);								\
	}

#define	MCR_RLCA {													\
		UINT8 tmp;													\
		tmp = (UINT8)(R_Z80A >> 7);									\
		R_Z80A = (R_Z80A << 1) | tmp;								\
		R_Z80F &= 0xec;												\
		R_Z80F |= tmp;												\
	}

#define MCR_EX(reg1, reg2) {										\
		MCR_EX1(reg1, reg2);										\
	}

#define MCR_ADD_W(dst, src) {										\
		UINT32 tmp;													\
		R_Z80F &= (S_FLAG | Z_FLAG | V_FLAG);						\
		tmp = (UINT32)(dst) + (UINT32)(src);						\
		R_Z80F |= (UINT8)(tmp >> 16);								\
		R_Z80F |= ((tmp ^ (dst) ^ (src)) >> 8) & H_FLAG;			\
		(dst) = (UINT16)tmp;										\
	}

#define	LDB_x(b, w) {												\
		(b) = Z80_RDMEM((w));										\
	}

#define MCR_DEC_W(reg) {											\
		(reg)--;													\
	}

#define MCR_RRCA {													\
		UINT8 tmp;													\
		tmp = (UINT8)(R_Z80A & 1);									\
		R_Z80F &= 0xec;												\
		R_Z80F |= tmp;												\
		R_Z80A = (R_Z80A >> 1) | (tmp << 7);						\
	}


#define MCR_DJNZ {													\
		R_Z80B--;													\
		JRBYFLAG(R_Z80B);											\
	}

#define MCR_RLA {													\
		UINT8 tmp;													\
		tmp = (UINT8)(R_Z80A >> 7);									\
		R_Z80A = (R_Z80A << 1) | (R_Z80F & 1);						\
		R_Z80F &= 0xec;												\
		R_Z80F |= tmp;												\
	}

#define MCR_JR {													\
		UINT8 ads;													\
		ads = Z80_RDMEM(R_Z80PC++);									\
		R_Z80PC += __CBW(ads);										\
		Z80_COUNT(5);												\
	}

#define MCR_RRA {													\
		UINT8 tmp;													\
		tmp = (UINT8)(R_Z80A & 1);									\
		R_Z80A = (R_Z80A >> 1) | (R_Z80F << 7);						\
		R_Z80F &= 0xec;												\
		R_Z80F |= tmp;												\
	}


#define	MCR_JRNFLG(flg) {											\
		JRBYFLAG(!(R_Z80F & (flg)));								\
	}

#define LDx_W(reg) {												\
		UINT adrs;													\
		GET_PC_WORD(adrs);											\
		Z80_WRMEM_W(adrs, (reg));									\
	}

#define	MCR_DDA {													\
			int dst;												\
			BYTE flg;												\
			int alow;												\
			dst = R_Z80A;											\
			alow = R_Z80A & 0x0f;									\
			flg = R_Z80F & N_FLAG;									\
			if (!flg) {												\
				if (alow >= 10) {									\
					flg |= H_FLAG;									\
					dst += 6;										\
				}													\
				else if (R_Z80F & H_FLAG) {							\
					dst += 6;										\
				}													\
				if ((dst >= 0xa0) || (R_Z80F & C_FLAG)) {			\
					flg |= C_FLAG;									\
					dst += 0x60;									\
				}													\
			}														\
			else {													\
				if ((dst > 0x99) || (R_Z80F & C_FLAG)) {			\
					dst -= 0x60;									\
					flg |= C_FLAG;									\
				}													\
				if ((alow > 9) || (R_Z80F & H_FLAG)) {				\
					if (alow < 6) {									\
						flg |= H_FLAG;								\
					}												\
					dst -= 6;										\
					if ((dst < 0) && (!(R_Z80F & H_FLAG))) {		\
						flg |= C_FLAG;								\
					}												\
				}													\
			}														\
			R_Z80A = (BYTE)dst;										\
			R_Z80F = flg | ZSPtable[dst & 0xff];					\
		}

#define	MCR_JRFLG(flg) {											\
		JRBYFLAG(R_Z80F & (flg));									\
	}

#define MCR_ADDx2(reg) {											\
			R_Z80F &= (S_FLAG | Z_FLAG | V_FLAG);					\
			R_Z80F |= (BYTE)((reg) >> 15);							\
			(reg) <<= 1;											\
			R_Z80F |= ((reg) >> 8) & H_FLAG;						\
		}

#define LDW_x(reg) {												\
		UINT adrs;													\
		GET_PC_WORD(adrs);											\
		(reg) = Z80_RDMEM_W(adrs);									\
	}

#define	MCR_CPL {													\
		R_Z80A = (UINT8)~R_Z80A;									\
		R_Z80F |= (H_FLAG | N_FLAG);								\
	}


#define LDB_x_a {													\
		UINT adrs;													\
		GET_PC_WORD(adrs);											\
		Z80_WRMEM(adrs, R_Z80A);									\
	}

#define MCR_INC_MEM(adrs) {											\
			BYTE tmp;												\
			tmp = Z80_RDMEM((adrs));								\
			tmp++;													\
			Z80_WRMEM((adrs), tmp);									\
			R_Z80F &= C_FLAG;										\
			R_Z80F |= z80inc_flag[tmp];								\
		}

#define	MCR_DEC_MEM(adrs) {											\
		UINT8 tmp;													\
		tmp = Z80_RDMEM((adrs));									\
		tmp--;														\
		Z80_WRMEM((adrs), tmp);										\
		R_Z80F &= C_FLAG;											\
		R_Z80F |= z80dec_flag[tmp];									\
	}

#define LDB_xhl_b {													\
		REG8 tmp;													\
		GET_PC_BYTE(tmp);											\
		Z80_WRMEM(R_Z80HL, tmp);									\
	}

#define MCR_SCF {													\
		R_Z80F &= ~(H_FLAG | N_FLAG);								\
		R_Z80F |= C_FLAG;											\
	}

#define LDB_a_x {													\
		UINT adrs;													\
		GET_PC_WORD(adrs);											\
		R_Z80A = Z80_RDMEM(adrs);									\
	}

#define MCR_CCF {													\
			R_Z80F &= ~(H_FLAG | N_FLAG);							\
			if (R_Z80F & C_FLAG) {									\
				R_Z80F |= H_FLAG;									\
			}														\
			R_Z80F ^= C_FLAG;										\
		}

#define MCR_LD(d, s) {												\
		(d) = (s);													\
	}

#define MCR_HALT {													\
		R_Z80PC--;													\
		Z80_IFF |= (1 << IFF_HALT);									\
		Z80IRQCHECKTERM;											\
	}


#define	MCR_ADD(b) {												\
			DWORD	res;											\
			res = (DWORD)R_Z80A + (DWORD)(b);						\
			R_Z80F = z80szc_flag[res & 0x1ff];						\
			R_Z80F |= (res ^ R_Z80A ^ (b)) & H_FLAG;				\
			R_Z80F |= (((res ^ R_Z80A) & (res ^ (b)))				\
													>> 5) & V_FLAG;	\
			R_Z80A = (BYTE)res;										\
		}

#define	MCR_ADD_XHL {												\
			BYTE tmp;												\
			tmp = Z80_RDMEM(R_Z80HL);								\
			MCR_ADD(tmp)											\
		}

#define	MCR_ADC(b) {												\
			DWORD	res;											\
			res = (DWORD)R_Z80A + (DWORD)(b) + (R_Z80F & 1);		\
			R_Z80F = z80szc_flag[res & 0x1ff];						\
			R_Z80F |= (res ^ R_Z80A ^ (b)) & H_FLAG;				\
			R_Z80F |= (((res ^ R_Z80A) & (res ^ (b)))				\
													>> 5) & V_FLAG;	\
			R_Z80A = (BYTE)res;										\
	}

#define	MCR_ADC_XHL {												\
			BYTE tmp;												\
			tmp = Z80_RDMEM(R_Z80HL);								\
			MCR_ADC(tmp)											\
		}


#define	MCR_SUB(b) {												\
			DWORD	res;											\
			res = (DWORD)R_Z80A - (DWORD)(b);						\
			R_Z80F = z80szc_flag[res & 0x1ff] | N_FLAG;				\
			R_Z80F |= (res ^ R_Z80A ^ (b)) & H_FLAG;				\
			R_Z80F |= (((R_Z80A^res) & (R_Z80A^(b)))				\
													>> 5) & V_FLAG;	\
			R_Z80A = (BYTE)res;										\
		}

#define	MCR_SUB_XHL {												\
			BYTE tmp;												\
			tmp = Z80_RDMEM(R_Z80HL);								\
			MCR_SUB(tmp)											\
		}

#define	MCR_SBC(b) {												\
			DWORD	res;											\
			res = (DWORD)R_Z80A - (DWORD)(b) - (R_Z80F & 1);		\
			R_Z80F = z80szc_flag[res & 0x1ff] | N_FLAG;				\
			R_Z80F |= (res ^ R_Z80A ^ (b)) & H_FLAG;				\
			R_Z80F |= (((R_Z80A^res) & (R_Z80A^(b)))				\
													>> 5) & V_FLAG;	\
			R_Z80A = (BYTE)res;										\
		}

#define	MCR_SBC_XHL {												\
			BYTE tmp;												\
			tmp = Z80_RDMEM(R_Z80HL);								\
			MCR_SBC(tmp)											\
		}


#define	MCR_AND(b) {												\
			R_Z80A &= (b);											\
			R_Z80F = ZSPtable[R_Z80A];								\
		}

#define	MCR_AND_XHL {												\
			BYTE tmp;												\
			tmp = Z80_RDMEM(R_Z80HL);								\
			MCR_AND(tmp)											\
		}

#define	MCR_XOR(b) {												\
			R_Z80A ^= (b);											\
			R_Z80F = ZSPtable[R_Z80A];								\
		}

#define	MCR_XOR_XHL {												\
			BYTE tmp;												\
			tmp = Z80_RDMEM(R_Z80HL);								\
			MCR_XOR(tmp)											\
		}


#define	MCR_OR(b) {													\
			R_Z80A |= (b);											\
			R_Z80F = ZSPtable[R_Z80A];								\
		}

#define	MCR_OR_XHL {												\
			BYTE tmp;												\
			tmp = Z80_RDMEM(R_Z80HL);								\
			MCR_OR(tmp)												\
		}

#define	MCR_CP(b) {													\
			DWORD	res;											\
			res = (DWORD)R_Z80A - (DWORD)(b);						\
			R_Z80F = z80szc_flag[res & 0x1ff] | N_FLAG;				\
			R_Z80F |= (res ^ R_Z80A ^ (b)) & H_FLAG;				\
			R_Z80F |= (((R_Z80A^res) & (R_Z80A^(b)))				\
													>> 5) & V_FLAG;	\
		}

#define	MCR_CP_XHL {												\
			BYTE tmp;												\
			tmp = Z80_RDMEM(R_Z80HL);								\
			MCR_CP(tmp)												\
		}


#define MCR_RETNFLG(flg) {											\
			if (!(R_Z80F & (flg))) {								\
				R_Z80PC = Z80_RDMEM_W(R_Z80SP);						\
				R_Z80SP += 2;										\
				Z80_COUNT(6);										\
			}														\
		}

#define MCR_POP(reg) {												\
			(reg) = Z80_RDMEM_W(R_Z80SP);							\
			R_Z80SP += 2;											\
		}

#define MCR_JPNFLG(flg) {											\
			if (!(R_Z80F & (flg))) {								\
				WORD tmp;											\
				tmp = Z80_RDMEM_W(R_Z80PC);							\
				R_Z80PC = tmp;										\
			}														\
			else {													\
				R_Z80PC += 2;										\
			}														\
		}

#define	MCR_JP {													\
			WORD tmp;												\
			tmp = Z80_RDMEM_W(R_Z80PC);								\
			R_Z80PC = tmp;											\
		}

#define MCR_CALLNFLG(flg) {											\
			if (!(R_Z80F & (flg))) {								\
				WORD tmp;											\
				GET_PC_WORD(tmp);									\
				R_Z80SP -= 2;										\
				Z80_WRMEM_W(R_Z80SP, R_Z80PC);						\
				R_Z80PC = tmp;										\
				Z80_COUNT(7);										\
			}														\
			else {													\
				R_Z80PC += 2;										\
			}														\
		}

#define MCR_PUSH(reg) {												\
			R_Z80SP -= 2;											\
			Z80_WRMEM_W(R_Z80SP, (reg));							\
		}

#define MCR_ADD_BYTE {												\
			BYTE tmp;												\
			GET_PC_BYTE(tmp);										\
			MCR_ADD(tmp)											\
		}

#define MCR_RST(vect) {												\
			R_Z80SP -= 2;											\
			Z80_WRMEM_W(R_Z80SP, R_Z80PC);							\
			R_Z80PC = (vect);										\
		}

#define MCR_RETFLG(flg) {											\
			if (R_Z80F & (flg)) {									\
				R_Z80PC = Z80_RDMEM_W(R_Z80SP);						\
				R_Z80SP += 2;										\
				Z80_COUNT(6);										\
			}														\
		}

#define MCR_RET {													\
			R_Z80PC = Z80_RDMEM_W(R_Z80SP);							\
			R_Z80SP += 2;											\
			Z80_COUNT(6);											\
		}

#define MCR_JPFLG(flg) {											\
			if (R_Z80F & (flg)) {									\
				WORD tmp;											\
				tmp = Z80_RDMEM_W(R_Z80PC);							\
				R_Z80PC = tmp;										\
			}														\
			else {													\
				R_Z80PC += 2;										\
			}														\
		}

#define MCR_CALLFLG(flg) {											\
			if (R_Z80F & (flg)) {									\
				WORD tmp;											\
				GET_PC_WORD(tmp);									\
				R_Z80SP -= 2;										\
				Z80_WRMEM_W(R_Z80SP, R_Z80PC);						\
				R_Z80PC = tmp;										\
				Z80_COUNT(7);										\
			}														\
			else {													\
				R_Z80PC += 2;										\
			}														\
		}

#define MCR_CALL {													\
			WORD tmp;												\
			GET_PC_WORD(tmp);										\
			R_Z80SP -= 2;											\
			Z80_WRMEM_W(R_Z80SP, R_Z80PC);							\
			R_Z80PC = tmp;											\
			Z80_COUNT(7);											\
		}

#define MCR_ADC_BYTE {												\
			BYTE tmp;												\
			GET_PC_BYTE(tmp);										\
			MCR_ADC(tmp)											\
		}


#define MCR_OUT_BYTE {												\
			REG8 tmp;												\
			GET_PC_BYTE(tmp);										\
			iocore_out((R_Z80A << 8) + tmp, tmp);					\
		}

#define MCR_SUB_BYTE {												\
			BYTE tmp;												\
			GET_PC_BYTE(tmp);										\
			MCR_SUB(tmp)											\
		}

#define	MCR_EXX {													\
			MCR_EX(R_Z80BC, R_Z80BC2)								\
			MCR_EX(R_Z80DE, R_Z80DE2)								\
			MCR_EX(R_Z80HL, R_Z80HL2)								\
		}

#define MCR_IN_BYTE {												\
			REG8 tmp;												\
			GET_PC_BYTE(tmp);										\
			R_Z80A = iocore_inp((R_Z80A << 8) | tmp);				\
		}

#define MCR_SBC_BYTE {												\
			BYTE tmp;												\
			GET_PC_BYTE(tmp);										\
			MCR_SBC(tmp)											\
		}


#define MCR_EX_XSP(reg) {											\
			WORD tmp;												\
			tmp = Z80_RDMEM_W(R_Z80SP);								\
			Z80_WRMEM_W(R_Z80SP, (reg));							\
			(reg) = tmp;											\
		}

#define MCR_AND_BYTE {												\
			BYTE tmp;												\
			GET_PC_BYTE(tmp);										\
			MCR_AND(tmp)											\
		}

#define MCR_LD_W(dst, src) {										\
			(dst) = (src);											\
		}

#define MCR_XOR_BYTE {												\
			BYTE tmp;												\
			GET_PC_BYTE(tmp);										\
			MCR_XOR(tmp)											\
		}


#define MCR_DI {													\
			Z80_IFF |= (1 << IFF_IFLAG);							\
		}

#define MCR_OR_BYTE {												\
			BYTE tmp;												\
			GET_PC_BYTE(tmp);										\
			MCR_OR(tmp)												\
		}

#define MCR_EI {													\
		UINT op;													\
		Z80_IFF &= ~(1 << IFF_IFLAG);								\
		op = Z80_RDMEM(R_Z80PC);									\
		if (op != 0xfb) {											\
			R_Z80PC++;												\
			R_Z80R++;												\
			Z80_COUNT(cycles_main[op]);								\
			z80c_mainop[op]();										\
		}															\
	}

#define MCR_CP_BYTE {												\
			BYTE tmp;												\
			GET_PC_BYTE(tmp);										\
			MCR_CP(tmp)												\
		}


