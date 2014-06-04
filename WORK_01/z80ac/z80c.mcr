/* -----------------------------------------------------------------------
 *
 * Z80C : Z80 Engine - GENERIC
 *
 *                              Copyright by Studio Milmake 1999-2000,2004
 *
 *------------------------------------------------------------------------ */

/* #define	PCCOUNTER */

#if defined(TRACE) && defined(PCCOUNTER)
extern	UINT	pccnt;
extern	UINT	pccnt2;
extern	UINT	pccnt3;
extern	UINT	lastpc;
#endif

#define Z80_COUNT(clock)												\
	do {																\
		CPU_REMCLOCK -= (clock);										\
	} while (/*CONSTCOND*/ 0)


#if defined(TRACE) && defined(PCCOUNTER)
#define	GET_PC_BYTE(b)													\
	do {																\
		if ((lastpc ^ R_Z80PC) & 0x8000) {								\
			TRACEOUT(("%.4x->%.4x", lastpc, R_Z80PC));					\
			lastpc = R_Z80PC;											\
			pccnt2++;													\
		}																\
		pccnt++;														\
		(b) = mem_read8(R_Z80PC++);										\
	} while (/*CONSTCOND*/ 0)

#define	GET_PC_WORD(w)													\
	do {																\
		pccnt3++;														\
		(w) = mem_read16(R_Z80PC);										\
		R_Z80PC += 2;													\
	} while (/*CONSTCOND*/ 0)
#else
#define	GET_PC_BYTE(b)													\
	do {																\
		(b) = mem_read8(R_Z80PC++);										\
	} while (/*CONSTCOND*/ 0)

#define	GET_PC_WORD(w)													\
	do {																\
		(w) = mem_read16(R_Z80PC);										\
		R_Z80PC += 2;													\
	} while (/*CONSTCOND*/ 0)
#endif


#define MCR_EX1(r1, r2)													\
	do {																\
		REG16 tmp;														\
		tmp = (r1);														\
		(r1) = (r2);													\
		(r2) = tmp;														\
	} while (/*CONSTCOND*/ 0)

#define JRBYFLAG(flg)													\
	do {																\
		if ((flg)) {													\
			SINT ofst;													\
			ofst = mem_read8s(R_Z80PC++);								\
			R_Z80PC += ofst;											\
			Z80_COUNT(5);												\
		}																\
		else {															\
			R_Z80PC++;													\
		}																\
	} while (/*CONSTCOND*/ 0)



#define	LDW_w(reg) {													\
		(reg) = mem_read16(R_Z80PC);									\
		R_Z80PC += 2;													\
	}

#define	LDx_B(dst, src) {												\
		mem_write8((dst), (src));										\
	}

#define	MCR_INC_W(reg) {												\
		(reg)++;														\
	}

#define	MCR_INC(reg) {													\
		R_Z80F &= C_FLAG;												\
		R_Z80F |= z80inc_flag2[(reg)];									\
		(reg)++;														\
	}

#define	MCR_DEC(reg) {													\
		R_Z80F &= C_FLAG;												\
		R_Z80F |= z80dec_flag2[(reg)];									\
		(reg)--;														\
	}

#define	LDB_b(reg) {													\
		(reg) = mem_read8(R_Z80PC++);									\
	}

#define	MCR_RLCA {														\
		REG8 tmp;														\
		tmp = (UINT8)(R_Z80A >> 7);										\
		R_Z80A = (R_Z80A << 1) | tmp;									\
		R_Z80F &= 0xec;													\
		R_Z80F |= tmp;													\
	}

#define MCR_EX(reg1, reg2) {											\
		MCR_EX1(reg1, reg2);											\
	}

#define MCR_ADD_W(dst, src) {											\
		UINT32 tmp;														\
		R_Z80F &= (S_FLAG | Z_FLAG | V_FLAG);							\
		tmp = (dst) + (src);											\
		R_Z80F |= (UINT8)(tmp >> 16);									\
		R_Z80F |= ((tmp ^ (dst) ^ (src)) >> 8) & H_FLAG;				\
		(dst) = (UINT16)tmp;											\
	}

#define	LDB_x(b, w) {													\
		(b) = mem_read8((w));											\
	}

#define MCR_DEC_W(reg) {												\
		(reg)--;														\
	}

#define MCR_RRCA {														\
		REG8 tmp;														\
		tmp = (UINT8)(R_Z80A & 1);										\
		R_Z80F &= 0xec;													\
		R_Z80F |= tmp;													\
		R_Z80A = (R_Z80A >> 1) | (tmp << 7);							\
	}


#define MCR_DJNZ {														\
		R_Z80B--;														\
		JRBYFLAG(R_Z80B);												\
	}

#define MCR_RLA {														\
		REG8 tmp;														\
		tmp = (UINT8)(R_Z80A >> 7);										\
		R_Z80A = (R_Z80A << 1) | (R_Z80F & 1);							\
		R_Z80F &= 0xec;													\
		R_Z80F |= tmp;													\
	}

#define MCR_JR {														\
		SINT ofst;														\
		ofst = mem_read8s(R_Z80PC++);									\
		R_Z80PC += ofst;												\
		Z80_COUNT(5);													\
	}

#define MCR_RRA {														\
		REG8 tmp;														\
		tmp = (UINT8)(R_Z80A & 1);										\
		R_Z80A = (R_Z80A >> 1) | (R_Z80F << 7);							\
		R_Z80F &= 0xec;													\
		R_Z80F |= tmp;													\
	}


#define	MCR_JRNFLG(flg) {												\
		JRBYFLAG(!(R_Z80F & (flg)));									\
	}

#define LDx_W(reg) {													\
		UINT adrs;														\
		GET_PC_WORD(adrs);												\
		mem_write16(adrs, (reg));										\
	}

#define	MCR_DDA {														\
		SINT dst;														\
		REG8 flg;														\
		SINT alow;														\
		dst = R_Z80A;													\
		alow = R_Z80A & 0x0f;											\
		flg = R_Z80F & N_FLAG;											\
		if (!flg) {														\
			if (alow >= 10) {											\
				flg |= H_FLAG;											\
				dst += 6;												\
			}															\
			else if (R_Z80F & H_FLAG) {									\
				dst += 6;												\
			}															\
			if ((dst >= 0xa0) || (R_Z80F & C_FLAG)) {					\
				flg |= C_FLAG;											\
				dst += 0x60;											\
			}															\
		}																\
		else {															\
			if ((dst > 0x99) || (R_Z80F & C_FLAG)) {					\
				dst -= 0x60;											\
				flg |= C_FLAG;											\
			}															\
			if ((alow > 9) || (R_Z80F & H_FLAG)) {						\
				if (alow < 6) {											\
					flg |= H_FLAG;										\
				}														\
				dst -= 6;												\
				if ((dst < 0) && (!(R_Z80F & H_FLAG))) {				\
					flg |= C_FLAG;										\
				}														\
			}															\
		}																\
		R_Z80A = (UINT8)dst;											\
		R_Z80F = flg | z80szp_flag[dst & 0xff];							\
	}

#define	MCR_JRFLG(flg) {												\
		JRBYFLAG(R_Z80F & (flg));										\
	}

#define MCR_ADDx2(reg) {												\
			R_Z80F &= (S_FLAG | Z_FLAG | V_FLAG);						\
			R_Z80F |= (UINT8)((reg) >> 15);								\
			(reg) <<= 1;												\
			R_Z80F |= ((reg) >> 8) & H_FLAG;							\
		}

#define LDW_x(reg) {													\
		UINT adrs;														\
		GET_PC_WORD(adrs);												\
		(reg) = mem_read16(adrs);										\
	}

#define	MCR_CPL {														\
		R_Z80A = (UINT8)(~R_Z80A);										\
		R_Z80F |= (H_FLAG | N_FLAG);									\
	}


#define LDB_x_a {														\
		UINT adrs;														\
		GET_PC_WORD(adrs);												\
		mem_write8(adrs, R_Z80A);										\
	}

#define MCR_INC_MEM(adrs) {												\
		REG8 tmp;														\
		tmp = mem_read8((adrs));										\
		R_Z80F &= C_FLAG;												\
		R_Z80F |= z80inc_flag2[tmp];									\
		mem_write8((adrs), (REG8)(tmp + 1));							\
	}

#define	MCR_DEC_MEM(adrs) {												\
		REG8 tmp;														\
		tmp = mem_read8((adrs));										\
		R_Z80F &= C_FLAG;												\
		R_Z80F |= z80dec_flag2[tmp];									\
		mem_write8((adrs), (REG8)(tmp - 1));							\
	}

#define LDB_xhl_b {														\
		REG8 tmp;														\
		GET_PC_BYTE(tmp);												\
		mem_write8(R_Z80HL, tmp);										\
	}

#define MCR_SCF {														\
		R_Z80F &= ~(H_FLAG | N_FLAG);									\
		R_Z80F |= C_FLAG;												\
	}

#define LDB_a_x {														\
		UINT adrs;														\
		GET_PC_WORD(adrs);												\
		R_Z80A = mem_read8(adrs);										\
	}

#define MCR_CCF {														\
		R_Z80F &= ~(H_FLAG | N_FLAG);									\
		if (R_Z80F & C_FLAG) {											\
			R_Z80F |= H_FLAG;											\
		}																\
		R_Z80F ^= C_FLAG;												\
	}

#define MCR_LD(d, s) {													\
		(d) = (s);														\
	}

#define MCR_HALT {														\
		R_Z80PC--;														\
		Z80_IFF |= (1 << IFF_HALT);										\
		CPU_REMCLOCK = 0;												\
	}


#define	MCR_ADD(b) {													\
		UINT res;														\
		res = R_Z80A + (b);												\
		R_Z80F = z80szc_flag[res & 0x1ff];								\
		R_Z80F |= (res ^ R_Z80A ^ (b)) & H_FLAG;						\
		R_Z80F |= (((res ^ R_Z80A) & (res ^ (b))) >> 5) & V_FLAG;		\
		R_Z80A = (UINT8)res;											\
	}

#define	MCR_ADD_XHL {													\
		REG8 tmp;														\
		tmp = mem_read8(R_Z80HL);										\
		MCR_ADD(tmp)													\
	}

#define	MCR_ADC(b) {													\
		UINT res;														\
		res = R_Z80A + (b) + (R_Z80F & 1);								\
		R_Z80F = z80szc_flag[res & 0x1ff];								\
		R_Z80F |= (res ^ R_Z80A ^ (b)) & H_FLAG;						\
		R_Z80F |= (((res ^ R_Z80A) & (res ^ (b))) >> 5) & V_FLAG;		\
		R_Z80A = (UINT8)res;											\
	}

#define	MCR_ADC_XHL {													\
		REG8 tmp;														\
		tmp = mem_read8(R_Z80HL);										\
		MCR_ADC(tmp)													\
	}


#define	MCR_SUB(b) {													\
		UINT res;														\
		res = R_Z80A - (b);												\
		R_Z80F = z80szc_flag[res & 0x1ff] | N_FLAG;						\
		R_Z80F |= (res ^ R_Z80A ^ (b)) & H_FLAG;						\
		R_Z80F |= (((R_Z80A ^ res) & (R_Z80A ^ (b))) >> 5) & V_FLAG;	\
		R_Z80A = (UINT8)res;											\
	}

#define	MCR_SUB_XHL {													\
		REG8 tmp;														\
		tmp = mem_read8(R_Z80HL);										\
		MCR_SUB(tmp)													\
	}

#define	MCR_SBC(b) {													\
		UINT res;														\
		res = R_Z80A - (b) - (R_Z80F & 1);								\
		R_Z80F = z80szc_flag[res & 0x1ff] | N_FLAG;						\
		R_Z80F |= (res ^ R_Z80A ^ (b)) & H_FLAG;						\
		R_Z80F |= (((R_Z80A ^ res) & (R_Z80A ^ (b))) >> 5) & V_FLAG;	\
		R_Z80A = (UINT8)res;											\
	}

#define	MCR_SBC_XHL {													\
		REG8 tmp;														\
		tmp = mem_read8(R_Z80HL);										\
		MCR_SBC(tmp)													\
	}


#define	MCR_AND(b) {													\
		R_Z80A &= (b);													\
		R_Z80F = z80szp_flag[R_Z80A];									\
	}

#define	MCR_AND_XHL {													\
		REG8 tmp;														\
		tmp = mem_read8(R_Z80HL);										\
		MCR_AND(tmp)													\
	}

#define	MCR_XOR(b) {													\
		R_Z80A ^= (b);													\
		R_Z80F = z80szp_flag[R_Z80A];									\
	}

#define	MCR_XOR_XHL {													\
		REG8 tmp;														\
		tmp = mem_read8(R_Z80HL);										\
		MCR_XOR(tmp)													\
	}


#define	MCR_OR(b) {														\
		R_Z80A |= (b);													\
		R_Z80F = z80szp_flag[R_Z80A];									\
	}

#define	MCR_OR_XHL {													\
		REG8 tmp;														\
		tmp = mem_read8(R_Z80HL);										\
		MCR_OR(tmp)														\
	}

#define	MCR_CP(b) {														\
		UINT res;														\
		res = R_Z80A - (b);												\
		R_Z80F = z80szc_flag[res & 0x1ff] | N_FLAG;						\
		R_Z80F |= (res ^ R_Z80A ^ (b)) & H_FLAG;						\
		R_Z80F |= (((R_Z80A ^ res) & (R_Z80A ^ (b))) >> 5) & V_FLAG;	\
	}

#define	MCR_CP_XHL {													\
		REG8 tmp;														\
		tmp = mem_read8(R_Z80HL);										\
		MCR_CP(tmp)														\
	}


#define MCR_RETNFLG(flg) {												\
		if (!(R_Z80F & (flg))) {										\
			R_Z80PC = mem_read16(R_Z80SP);								\
			R_Z80SP += 2;												\
			Z80_COUNT(6);												\
		}																\
	}

#define MCR_POP(reg) {													\
		(reg) = mem_read16(R_Z80SP);									\
		R_Z80SP += 2;													\
	}

#define MCR_JPNFLG(flg) {												\
		if (!(R_Z80F & (flg))) {										\
			R_Z80PC = mem_read16(R_Z80PC);								\
		}																\
		else {															\
			R_Z80PC += 2;												\
		}																\
	}

#define	MCR_JP {														\
		R_Z80PC = mem_read16(R_Z80PC);									\
	}

#define MCR_CALLNFLG(flg) {												\
		if (!(R_Z80F & (flg))) {										\
			REG16 tmp;													\
			GET_PC_WORD(tmp);											\
			R_Z80SP -= 2;												\
			mem_write16(R_Z80SP, R_Z80PC);								\
			R_Z80PC = tmp;												\
			Z80_COUNT(7);												\
		}																\
		else {															\
			R_Z80PC += 2;												\
		}																\
	}

#define MCR_PUSH(reg) {													\
		R_Z80SP -= 2;													\
		mem_write16(R_Z80SP, (reg));									\
	}

#define MCR_ADD_BYTE {													\
		REG8 tmp;														\
		GET_PC_BYTE(tmp);												\
		MCR_ADD(tmp)													\
	}

#define MCR_RST(vect) {													\
		R_Z80SP -= 2;													\
		mem_write16(R_Z80SP, R_Z80PC);									\
		R_Z80PC = (vect);												\
	}

#define MCR_RETFLG(flg) {												\
		if (R_Z80F & (flg)) {											\
			R_Z80PC = mem_read16(R_Z80SP);								\
			R_Z80SP += 2;												\
			Z80_COUNT(6);												\
		}																\
	}

#define MCR_RET {														\
		R_Z80PC = mem_read16(R_Z80SP);									\
		R_Z80SP += 2;													\
		Z80_COUNT(6);													\
	}

#define MCR_JPFLG(flg) {												\
		if (R_Z80F & (flg)) {											\
			R_Z80PC = mem_read16(R_Z80PC);								\
		}																\
		else {															\
			R_Z80PC += 2;												\
		}																\
	}

#define MCR_CALLFLG(flg) {												\
		if (R_Z80F & (flg)) {											\
			REG16 tmp;													\
			GET_PC_WORD(tmp);											\
			R_Z80SP -= 2;												\
			mem_write16(R_Z80SP, R_Z80PC);								\
			R_Z80PC = tmp;												\
			Z80_COUNT(7);												\
		}																\
		else {															\
			R_Z80PC += 2;												\
		}																\
	}

#define MCR_CALL {														\
		REG16 tmp;														\
		GET_PC_WORD(tmp);												\
		R_Z80SP -= 2;													\
		mem_write16(R_Z80SP, R_Z80PC);									\
		R_Z80PC = tmp;													\
		Z80_COUNT(7);													\
	}

#define MCR_ADC_BYTE {													\
		REG8 tmp;														\
		GET_PC_BYTE(tmp);												\
		MCR_ADC(tmp)													\
	}


#define MCR_OUT_BYTE {													\
		REG8 tmp;														\
		GET_PC_BYTE(tmp);												\
		iocore_out((R_Z80A << 8) + tmp, tmp);							\
	}

#define MCR_SUB_BYTE {													\
		REG8 tmp;														\
		GET_PC_BYTE(tmp);												\
		MCR_SUB(tmp)													\
	}

#define	MCR_EXX {														\
		MCR_EX(R_Z80BC, R_Z80BC2)										\
		MCR_EX(R_Z80DE, R_Z80DE2)										\
		MCR_EX(R_Z80HL, R_Z80HL2)										\
	}

#define MCR_IN_BYTE {													\
		REG8 tmp;														\
		GET_PC_BYTE(tmp);												\
		R_Z80A = iocore_inp((R_Z80A << 8) | tmp);						\
	}

#define MCR_SBC_BYTE {													\
		REG8 tmp;														\
		GET_PC_BYTE(tmp);												\
		MCR_SBC(tmp)													\
	}


#define MCR_EX_XSP(reg) {												\
		REG16 tmp;														\
		tmp = mem_read16(R_Z80SP);										\
		mem_write16(R_Z80SP, (reg));									\
		(reg) = tmp;													\
	}

#define MCR_AND_BYTE {													\
		REG8 tmp;														\
		GET_PC_BYTE(tmp);												\
		MCR_AND(tmp)													\
	}

#define MCR_LD_W(dst, src) {											\
		(dst) = (src);													\
	}

#define MCR_XOR_BYTE {													\
		REG8 tmp;														\
		GET_PC_BYTE(tmp);												\
		MCR_XOR(tmp)													\
	}


#define MCR_DI {														\
		Z80_IFF |= (1 << IFF_IFLAG);									\
	}

#define MCR_OR_BYTE {													\
		REG8 tmp;														\
		GET_PC_BYTE(tmp);												\
		MCR_OR(tmp)														\
	}

#define MCR_EI {														\
		REG8 iff;														\
		SINT32 rem;														\
		iff = Z80_IFF;													\
		if (iff & (1 << IFF_IFLAG)) {									\
			Z80_IFF = (UINT8)(iff & (~(1 << IFF_IFLAG)));				\
			rem = CPU_REMCLOCK - 1;										\
			if ((rem < 0) ||											\
				((!(iff & (1 << IFF_NMI))) && (CPU_REQIRQ != 0))) {		\
				CPU_BASECLOCK -= rem;									\
				CPU_REMCLOCK = 1;										\
			}															\
		}																\
	}

#define MCR_CP_BYTE {													\
		REG8 tmp;														\
		GET_PC_BYTE(tmp);												\
		MCR_CP(tmp)														\
	}


