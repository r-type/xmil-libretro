
#if defined(BYTESEX_LITTLE)
typedef union {
	struct {
		UINT8	l;
		UINT8	h;
	}		b;
	UINT16	w;
} DMAPAIR;
#else
typedef union {
	struct {
		UINT8	h;
		UINT8	l;
	}		b;
	UINT16	w;
} DMAPAIR;
#endif

typedef struct {
	UINT8	working;
	UINT8	increment;

	UINT8	enable;				// DMA_ENBL
	UINT8	ready;				// DMA_REDY
	UINT8	mode;				// DMA_MODE
	UINT8	cmd;				// DMA_CMND

	UINT8	INT_ENBL;
	UINT8	INT_FLG;
	UINT8	INT_PLS;
	UINT8	INT_VCT;

	UINT8	MASK_BYT;
	UINT8	MACH_BYT;
	UINT8	MACH_FLG;
	UINT8	ENDB_FLG;

	UINT8	RR_MSK;
	UINT8	RR;

	UINT8	dummydat;
	UINT8	padding[3];

	DMAPAIR	ADR_A;
	DMAPAIR	ADR_B;
	DMAPAIR	BYT_L;

	DMAPAIR	CNT_A;
	DMAPAIR	CNT_B;
	DMAPAIR	BYT_N;

	UINT8	WR[8];

	UINT	WR_CNT;
	UINT	WR_OFF;
	UINT	RR_CNT;
	UINT	RR_OFF;
	UINT8	WR_TBL[16];
	UINT8	RR_TBL[16];
} DMAC;


#ifdef __cplusplus
extern "C" {
#endif

void dmac_sendready(BRESULT ready);
BRESULT ieitem_dmac(UINT id);

void IOOUTCALL dmac_o(UINT port, REG8 dat);			// x1_dma_w
REG8 IOINPCALL dmac_i(UINT port);					// x1_dma_r

void dmac_reset(void);

#ifdef __cplusplus
}
#endif

