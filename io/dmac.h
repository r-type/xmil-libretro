
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
	UINT8	DMA_ENBL;
	UINT8	DMA_REDY;
	UINT8	DMA_MODE;
	UINT8	DMA_CMND;

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
	UINT8	DMA_STOP;
	UINT8	dummydat;

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


#if 0
#define DMAOFST(item)	(BYTE)(&(((DMA_TABLE *)NULL)->item))

#if (defined(NP2_LITTLEENDIAN) || defined(NP2_TESTBIGENDIAN))
#define DMAOFST_L(item)	(BYTE)(&(((DMA_TABLE *)NULL)->item.b[0]))
#define DMAOFST_H(item)	(BYTE)(&(((DMA_TABLE *)NULL)->item.b[1]))
#else
#define DMAOFST_L(item)	(BYTE)(&(((DMA_TABLE *)NULL)->item.b[1]))
#define DMAOFST_H(item)	(BYTE)(&(((DMA_TABLE *)NULL)->item.b[0]))
#endif
#endif


#ifdef __cplusplus
extern "C" {
#endif

void IOOUTCALL dmac_o(UINT port, REG8 value);		// x1_dma_w
REG8 IOINPCALL dmac_i(UINT port);					// x1_dma_r

void dmac_reset(void);

#ifdef __cplusplus
}
#endif

