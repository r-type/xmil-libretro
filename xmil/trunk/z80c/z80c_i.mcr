
#define	GET_I(base)		__CBW(Z80_RDMEM(R_Z80PC++)) + (base)


#define MCR_INC_XI(base) {											\
			WORD adrs;												\
			adrs = GET_I((base));									\
			MCR_INC_MEM(adrs)										\
		}

#define MCR_DEC_XI(base) {											\
			WORD adrs;												\
			adrs = GET_I((base));									\
			MCR_DEC_MEM(adrs)										\
		}

#define MCR_LDXIBYTE(base) {										\
			WORD adrs;												\
			adrs = GET_I((base));									\
			Z80_WRMEM(adrs, Z80_RDMEM(R_Z80PC++));					\
		}

#define MCR_LD_RXI(reg, base) {										\
			WORD adrs;												\
			adrs = GET_I((base));									\
			(reg) = Z80_RDMEM(adrs);								\
		}

#define MCR_LD_XIR(reg, base) {										\
			WORD adrs;												\
			adrs = GET_I((base));									\
			Z80_WRMEM(adrs, (reg));									\
		}

#define MCR_ADD_XI(base) {											\
			WORD adrs;												\
			BYTE tmp;												\
			adrs = GET_I((base));									\
			tmp = Z80_RDMEM(adrs);									\
			MCR_ADD(tmp)											\
		}

#define MCR_ADC_XI(base) {											\
			WORD adrs;												\
			BYTE tmp;												\
			adrs = GET_I((base));									\
			tmp = Z80_RDMEM(adrs);									\
			MCR_ADC(tmp)											\
		}

#define MCR_SUB_XI(base) {											\
			WORD adrs;												\
			BYTE tmp;												\
			adrs = GET_I((base));									\
			tmp = Z80_RDMEM(adrs);									\
			MCR_SUB(tmp)											\
		}

#define MCR_SBC_XI(base) {											\
			WORD adrs;												\
			BYTE tmp;												\
			adrs = GET_I((base));									\
			tmp = Z80_RDMEM(adrs);									\
			MCR_SBC(tmp)											\
		}

#define MCR_AND_XI(base) {											\
			WORD adrs;												\
			BYTE tmp;												\
			adrs = GET_I((base));									\
			tmp = Z80_RDMEM(adrs);									\
			MCR_AND(tmp)											\
		}

#define MCR_XOR_XI(base) {											\
			WORD adrs;												\
			BYTE tmp;												\
			adrs = GET_I((base));									\
			tmp = Z80_RDMEM(adrs);									\
			MCR_XOR(tmp)											\
		}

#define MCR_OR_XI(base) {											\
			WORD adrs;												\
			BYTE tmp;												\
			adrs = GET_I((base));									\
			tmp = Z80_RDMEM(adrs);									\
			MCR_OR(tmp)												\
		}

#define MCR_CP_XI(base) {											\
			WORD adrs;												\
			BYTE tmp;												\
			adrs = GET_I((base));									\
			tmp = Z80_RDMEM(adrs);									\
			MCR_CP(tmp)												\
		}

