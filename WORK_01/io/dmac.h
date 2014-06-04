
#define	DMAS_STOIC

#if defined(BYTESEX_LITTLE)
typedef union {
	struct {
		UINT8	flag;
		UINT8	padding;
		UINT8	addrl;
		UINT8	addrh;
	}	b;
	struct {
		UINT8	flag;
		UINT8	padding;
		UINT16	addr;
	}	w;
} DMACNT;

typedef union {
	struct {
		UINT8	ll;
		UINT8	lh;
		UINT8	nl;
		UINT8	nh;
	}	b;
	struct {
		UINT16	l;
		UINT16	n;
	}	w;
} DMALENG;

typedef union {
	struct {
		UINT8	al;
		UINT8	ah;
		UINT8	bl;
		UINT8	bh;
	}	b;
	struct {
		UINT16	a;
		UINT16	b;
	}	w;
} DMAADDR;
#else
typedef union {
	struct {
		UINT8	flag;
		UINT8	padding;
		UINT8	addrh;
		UINT8	addrl;
	}	b;
	struct {
		UINT8	flag;
		UINT8	padding;
		UINT16	addr;
	}	w;
} DMACNT;

typedef union {
	struct {
		UINT8	lh;
		UINT8	ll;
		UINT8	nh;
		UINT8	nl;
	}	b;
	struct {
		UINT16	l;
		UINT16	n;
	}	w;
} DMALENG;
typedef union {
	struct {
		UINT8	ah;
		UINT8	al;
		UINT8	bh;
		UINT8	bl;
	}	b;
	struct {
		UINT16	a;
		UINT16	b;
	}	w;
} DMAADDR;
#endif


#if defined(DMAS_STOIC)

enum {
	DMAF_WORKING		= 0x01,
	DMAF_INCREMENT		= 0x08,
	DMAF_MACH			= 0x10,
	DMAF_ENDB			= 0x20
};

typedef struct {
	UINT8	flag;
	UINT8	WR0;
	UINT8	mode;				/* DMA_MODE */
	UINT8	INT_ENBL;

	UINT8	WR4;
	UINT8	WR5;
	UINT8	MASK_BYT;
	UINT8	MACH_BYT;

	DMACNT	cnt_a;
	DMACNT	cnt_b;

	DMALENG	leng;
	DMAADDR	addr;

	UINT8	enable;				/* DMA_ENBL */
	UINT8	ready;				/* DMA_REDY */
	UINT8	RR_MSK;
	UINT8	RR;

	UINT8	INT_FLG;
	UINT8	INT_PLS;
	UINT8	INT_VCT;
	UINT8	dummydat;

	UINT	wcnt;
	UINT	wptr;
	UINT	rcnt;
	UINT	rptr;
	UINT8	wtbl[8];
	UINT8	rtbl[8];
} DMAC;

#else

typedef struct {
	UINT8	working;
	UINT8	increment;

	UINT8	mode;				/* DMA_MODE */
	UINT8	__cmd;				/* DMA_CMND */

	UINT8	enable;				/* DMA_ENBL */
	UINT8	ready;				/* DMA_REDY */

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

	UINT8	WR0;
#if 0
	UINT8	WR1;
	UINT8	WR2;
	UINT8	WR3;
#endif	/* 0 */
	UINT8	WR4;
	UINT8	WR5;
	UINT8	dummydat;
#if 0
	UINT8	padding;
#endif	/* 0 */

	DMACNT	cnt_a;
	DMACNT	cnt_b;

	DMALENG	leng;
	DMAADDR	addr;

	UINT	wcnt;
	UINT	wptr;
	UINT	rcnt;
	UINT	rptr;
	UINT8	wtbl[8];
	UINT8	rtbl[8];
} DMAC;

#endif


#ifdef __cplusplus
extern "C" {
#endif

void dmac_sendready(BRESULT ready);
BRESULT ieitem_dmac(UINT id);

void IOOUTCALL dmac_o(UINT port, REG8 dat);
REG8 IOINPCALL dmac_i(UINT port);

void dmac_reset(void);

#ifdef __cplusplus
}
#endif

