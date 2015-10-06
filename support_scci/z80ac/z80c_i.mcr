
#define	GET_I(base)		LOW16((base) + mem_read8s(R_Z80PC++))


#define MCR_INC_XI(base) {												\
		UINT adrs;														\
		adrs = GET_I((base));											\
		MCR_INC_MEM(adrs)												\
	}

#define MCR_DEC_XI(base) {												\
		UINT adrs;														\
		adrs = GET_I((base));											\
		MCR_DEC_MEM(adrs)												\
	}

#define MCR_LDXIBYTE(base) {											\
		UINT adrs;														\
		adrs = GET_I((base));											\
		mem_write8(adrs, mem_read8(R_Z80PC++));							\
	}

#define MCR_LD_RXI(reg, base) {											\
		UINT adrs;														\
		adrs = GET_I((base));											\
		(reg) = mem_read8(adrs);										\
	}

#define MCR_LD_XIR(reg, base) {											\
		UINT adrs;														\
		adrs = GET_I((base));											\
		mem_write8(adrs, (reg));										\
	}

#define MCR_ADD_XI(base) {												\
		UINT adrs;														\
		REG8 tmp;														\
		adrs = GET_I((base));											\
		tmp = mem_read8(adrs);											\
		MCR_ADD(tmp)													\
	}

#define MCR_ADC_XI(base) {												\
		UINT adrs;														\
		REG8 tmp;														\
		adrs = GET_I((base));											\
		tmp = mem_read8(adrs);											\
		MCR_ADC(tmp)													\
	}

#define MCR_SUB_XI(base) {												\
		UINT adrs;														\
		REG8 tmp;														\
		adrs = GET_I((base));											\
		tmp = mem_read8(adrs);											\
		MCR_SUB(tmp)													\
	}

#define MCR_SBC_XI(base) {												\
		UINT adrs;														\
		REG8 tmp;														\
		adrs = GET_I((base));											\
		tmp = mem_read8(adrs);											\
		MCR_SBC(tmp)													\
	}

#define MCR_AND_XI(base) {												\
		UINT adrs;														\
		REG8 tmp;														\
		adrs = GET_I((base));											\
		tmp = mem_read8(adrs);											\
		MCR_AND(tmp)													\
	}

#define MCR_XOR_XI(base) {												\
		UINT adrs;														\
		REG8 tmp;														\
		adrs = GET_I((base));											\
		tmp = mem_read8(adrs);											\
		MCR_XOR(tmp)													\
	}

#define MCR_OR_XI(base) {												\
		UINT adrs;														\
		REG8 tmp;														\
		adrs = GET_I((base));											\
		tmp = mem_read8(adrs);											\
		MCR_OR(tmp)														\
	}

#define MCR_CP_XI(base) {												\
		UINT adrs;														\
		REG8 tmp;														\
		adrs = GET_I((base));											\
		tmp = mem_read8(adrs);											\
		MCR_CP(tmp)														\
	}

