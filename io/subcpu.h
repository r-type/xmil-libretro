
typedef struct {
	UINT8	timer[8][6];
	UINT8	work[4];
	UINT8	vect;
	UINT8	tvctrl;
	UINT8	zero;
	UINT8	padding;
	UINT8	datcnt;
	UINT8	datptr;
	UINT8	cmdcnt;
	UINT8	cmdptr;
	UINT	keycount;
	UINT	keycountrep;
	UINT8	mode;
	UINT8	keydata;
	UINT8	padding2[2];
} SCPUS;

typedef struct {
	SINT32	intrclock;
} SCPUE;

typedef struct {
	SCPUS	s;
	SCPUE	e;
} SUBCPU;

#ifdef __cplusplus
extern "C" {
#endif

void neitem_scpu(UINT id);
BRESULT ieitem_scpu(UINT id);
void subcpu_sendkey(void);

void IOOUTCALL subcpu_o(UINT port, REG8 dat);
REG8 IOINPCALL subcpu_i(UINT port);

void subcpu_reset(void);

#ifdef __cplusplus
}
#endif

