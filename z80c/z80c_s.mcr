
#define	MCR_Z80INF(reg) {											\
		(reg) = iocore_inp(R_Z80BC);								\
		R_Z80F &= C_FLAG;											\
		R_Z80F |= ZSPtable[(reg)];									\
	}

#define	MCR_Z80OUT(reg) {											\
		iocore_out(R_Z80BC, (reg));									\
	}

#define MCR_SBC_W(reg) {											\
			DWORD tmp;												\
			tmp = (DWORD)R_Z80HL - (DWORD)(reg)						\
										- (DWORD)(R_Z80F & 1);		\
			R_Z80F = N_FLAG;										\
			if (tmp & 0xffff0000) {									\
				R_Z80F |= C_FLAG;									\
			}														\
			if (!(tmp & 0x0000ffff)) {								\
				R_Z80F |= Z_FLAG;									\
			}														\
			else if (tmp & 0x00008000) {							\
				R_Z80F |= S_FLAG;									\
			}														\
			R_Z80F |= ((((R_Z80HL) ^ tmp) & ((R_Z80HL) ^ (reg)))	\
												>> 13) & V_FLAG;	\
			R_Z80F |= ((R_Z80HL ^ tmp ^ (reg)) >> 8) & H_FLAG;		\
			R_Z80HL = (WORD)tmp;									\
		}

#define MCR_NEG {													\
			DWORD	res;											\
			res = 0 - (DWORD)R_Z80A;								\
			R_Z80F = z80szc_flag[res & 0x1ff] | N_FLAG;				\
			R_Z80F |= (res ^ R_Z80A) & H_FLAG;						\
			R_Z80F |= ((res & R_Z80A) >> 5) & V_FLAG;				\
			R_Z80A = (BYTE)res;										\
		}

#define	MCR_RETN {													\
			Z80_IFF &= ~((1 << IFF_NMI) | (1 << IFF_IRQ));			\
			MCR_RET													\
		}

#define MCR_IM(mode) {												\
			R_Z80IM = (mode);										\
		}

#define MCR_ADC_W(reg) {											\
			DWORD tmp;												\
			tmp = (DWORD)R_Z80HL + (DWORD)(reg)						\
										+ (DWORD)(R_Z80F & 1);		\
			R_Z80F = (BYTE)(tmp >> 16);								\
			if (!(tmp & 0x0000ffff)) {								\
				R_Z80F |= Z_FLAG;									\
			}														\
			else if (tmp & 0x00008000) {							\
				R_Z80F |= S_FLAG;									\
			}														\
			R_Z80F |= (((tmp ^ (reg)) & (tmp ^ R_Z80HL))			\
												>> 13) & V_FLAG;	\
			R_Z80F |= ((R_Z80HL ^ tmp ^ (reg)) >> 8) & H_FLAG;		\
			(reg) = (WORD)tmp;										\
		}

#define MCR_RETI {													\
			Z80_IFF &= ~(1 << IFF_IRQ);								\
			MCR_RET													\
		}

#define MCR_LD_RA {													\
			R_Z80R = R_Z80A;										\
			R_Z80R2 = R_Z80A;										\
		}


#define MCR_LD_AI {													\
			R_Z80A = R_Z80I;										\
			R_Z80F &= C_FLAG;										\
			R_Z80F |= z80szc_flag[R_Z80I];							\
			if (!(Z80_IFF & (1 << IFF_IFLAG))) {					\
				R_Z80F |= V_FLAG;									\
			}														\
		}

#define MCR_LD_AR {													\
			R_Z80A = (R_Z80R & 0x7f) | (R_Z80R2 & 0x80);			\
			R_Z80F &= C_FLAG;										\
			R_Z80F |= z80szc_flag[R_Z80A];							\
			if (!(Z80_IFF & (1 << IFF_IFLAG))) {					\
				R_Z80F |= V_FLAG;									\
			}														\
		}


#define MCR_RRD {													\
			BYTE tmp;												\
			tmp = Z80_RDMEM(R_Z80HL);								\
			Z80_WRMEM(R_Z80HL, (tmp >> 4) | (R_Z80A << 4));			\
			R_Z80A = (R_Z80A & 0xf0) | (tmp & 0x0f);				\
			R_Z80F &= C_FLAG;										\
			R_Z80F |= ZSPtable[R_Z80A];								\
		}

#define MCR_RLD {													\
			BYTE tmp;												\
			tmp = Z80_RDMEM(R_Z80HL);								\
			Z80_WRMEM(R_Z80HL, (tmp & 0x0f) | (R_Z80A << 4));		\
			R_Z80A = (tmp & 0xf0) | (R_Z80A >> 4);					\
			R_Z80F &= C_FLAG;										\
			R_Z80F |= ZSPtable[R_Z80A];								\
		}


#define MCR_Z80IN0 {												\
			BYTE tmp;												\
			tmp = iocore_inp(R_Z80BC);								\
			R_Z80F &= C_FLAG;										\
			R_Z80F |= ZSPtable[tmp];								\
		}


#define MCR_LDI {													\
			BYTE tmp;												\
			tmp = Z80_RDMEM(R_Z80HL++);								\
			Z80_WRMEM(R_Z80DE++, tmp);								\
			R_Z80F &= 0xe9;											\
			R_Z80BC--;												\
			if (R_Z80BC) {											\
				R_Z80F |= V_FLAG;									\
			}														\
		}

#define MCR_CPI {													\
			BYTE tmp, diff;											\
			tmp = Z80_RDMEM(R_Z80HL++);								\
			R_Z80F &= C_FLAG;										\
			diff = R_Z80A - tmp;									\
			R_Z80F |= (diff ^ R_Z80A ^ tmp) & H_FLAG;				\
			R_Z80F |= z80szc_flag[diff];							\
			R_Z80F |= N_FLAG;										\
			R_Z80BC--;												\
			if (R_Z80BC) {											\
				R_Z80F |= V_FLAG;									\
			}														\
		}

#define MCR_INI {													\
			BYTE tmp;												\
			tmp = iocore_inp(R_Z80BC);								\
			Z80_WRMEM(R_Z80HL++, tmp);								\
			R_Z80F = N_FLAG;										\
			R_Z80B--;												\
			if (!R_Z80B) {											\
				R_Z80F |= Z_FLAG;									\
			}														\
		}

#define MCR_OUTI {													\
			BYTE tmp;												\
			tmp = Z80_RDMEM(R_Z80HL++);								\
			R_Z80F = N_FLAG;										\
			R_Z80B--;												\
			if (!R_Z80B) {											\
				R_Z80F |= Z_FLAG;									\
			}														\
			iocore_out(R_Z80BC, tmp);								\
		}

#define MCR_LDD {													\
			BYTE tmp;												\
			tmp = Z80_RDMEM(R_Z80HL--);								\
			Z80_WRMEM(R_Z80DE--, tmp);								\
			R_Z80F &= 0xe9;											\
			R_Z80BC--;												\
			if (R_Z80BC) {											\
				R_Z80F |= V_FLAG;									\
			}														\
		}

#define MCR_CPD {													\
			BYTE tmp, diff;											\
			tmp = Z80_RDMEM(R_Z80HL--);								\
			R_Z80F &= C_FLAG;										\
			diff = R_Z80A - tmp;									\
			R_Z80F |= (diff ^ R_Z80A ^ tmp) & H_FLAG;				\
			R_Z80F |= z80szc_flag[diff];							\
			R_Z80F |= N_FLAG;										\
			R_Z80BC--;												\
			if (R_Z80BC) {											\
				R_Z80F |= V_FLAG;									\
			}														\
		}

#define MCR_IND {													\
			BYTE tmp;												\
			tmp = iocore_inp(R_Z80BC);								\
			Z80_WRMEM(R_Z80HL--, tmp);								\
			R_Z80F = N_FLAG;										\
			R_Z80B--;												\
			if (!R_Z80B) {											\
				R_Z80F |= Z_FLAG;									\
			}														\
		}

#define MCR_OUTD {													\
			BYTE tmp;												\
			tmp = Z80_RDMEM(R_Z80HL--);								\
			R_Z80F = N_FLAG;										\
			R_Z80B--;												\
			if (!R_Z80B) {											\
				R_Z80F |= Z_FLAG;									\
			}														\
			iocore_out(R_Z80BC, tmp);								\
		}

#define MCR_LDIR {													\
			BYTE tmp;												\
			tmp = Z80_RDMEM(R_Z80HL++);								\
			Z80_WRMEM(R_Z80DE++, tmp);								\
			R_Z80F &= 0xe9;											\
			R_Z80BC--;												\
			if (R_Z80BC) {											\
				R_Z80F |= V_FLAG;									\
				Z80_COUNT(5);										\
				R_Z80PC -= 2;										\
			}														\
		}

#define MCR_CPIR {													\
			BYTE tmp, diff;											\
			tmp = Z80_RDMEM(R_Z80HL++);								\
			R_Z80F &= C_FLAG;										\
			diff = R_Z80A - tmp;									\
			R_Z80F |= (diff ^ R_Z80A ^ tmp) & H_FLAG;				\
			R_Z80F |= z80szc_flag[diff];							\
			if (!diff) {											\
				R_Z80F |= N_FLAG;									\
				if (--R_Z80BC) {									\
					R_Z80F |= V_FLAG;								\
				}													\
			}														\
			else {													\
				if (--R_Z80BC) {									\
					R_Z80F |= V_FLAG;								\
					Z80_COUNT(5);									\
					R_Z80PC -= 2;									\
				}													\
			}														\
		}

#define MCR_INIR {													\
			BYTE tmp;												\
			tmp = iocore_inp(R_Z80BC);								\
			Z80_WRMEM(R_Z80HL++, tmp);								\
			if (--R_Z80B) {											\
				R_Z80F = N_FLAG;									\
				Z80_COUNT(5);										\
				R_Z80PC -= 2;										\
			}														\
			else {													\
				R_Z80F = (N_FLAG | Z_FLAG);							\
			}														\
		}

#define MCR_OTIR {													\
			BYTE tmp;												\
			tmp = Z80_RDMEM(R_Z80HL++);								\
			if (--R_Z80B) {											\
				R_Z80F = N_FLAG;									\
				Z80_COUNT(5);										\
				R_Z80PC -= 2;										\
			}														\
			else {													\
				R_Z80F = (N_FLAG | Z_FLAG);							\
			}														\
			iocore_out(R_Z80BC, tmp);								\
		}

#define MCR_LDDR {													\
			BYTE tmp;												\
			tmp = Z80_RDMEM(R_Z80HL--);								\
			Z80_WRMEM(R_Z80DE--, tmp);								\
			R_Z80F &= 0xe9;											\
			R_Z80BC--;												\
			if (R_Z80BC) {											\
				R_Z80F |= V_FLAG;									\
				Z80_COUNT(5);										\
				R_Z80PC -= 2;										\
			}														\
		}

#define MCR_CPDR {													\
			BYTE tmp, diff;											\
			tmp = Z80_RDMEM(R_Z80HL--);								\
			R_Z80F &= C_FLAG;										\
			diff = R_Z80A - tmp;									\
			R_Z80F |= (diff ^ R_Z80A ^ tmp) & H_FLAG;				\
			R_Z80F |= z80szc_flag[diff];							\
			if (!diff) {											\
				R_Z80F |= N_FLAG;									\
				if (--R_Z80BC) {									\
					R_Z80F |= V_FLAG;								\
				}													\
			}														\
			else {													\
				if (--R_Z80BC) {									\
					R_Z80F |= V_FLAG;								\
					Z80_COUNT(5);									\
					R_Z80PC -= 2;									\
				}													\
			}														\
		}

#define MCR_INDR {													\
		REG8 tmp;													\
		tmp = iocore_inp(R_Z80BC);									\
		Z80_WRMEM(R_Z80HL--, tmp);									\
		if (--R_Z80B) {												\
			R_Z80F = N_FLAG;										\
			Z80_COUNT(5);											\
			R_Z80PC -= 2;											\
		}															\
		else {														\
			R_Z80F = (N_FLAG | Z_FLAG);								\
		}															\
	}

#define MCR_OTDR {													\
		REG8 tmp;													\
		tmp = Z80_RDMEM(R_Z80HL--);									\
		if (--R_Z80B) {												\
			R_Z80F = N_FLAG;										\
			Z80_COUNT(5);											\
			R_Z80PC -= 2;											\
		}															\
		else {														\
			R_Z80F = (N_FLAG | Z_FLAG);								\
		}															\
		iocore_out(R_Z80BC, tmp);									\
	}

