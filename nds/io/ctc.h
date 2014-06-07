
enum {
	CTC_OPM			= 0,
	CTC_TURBO1		= 1,
	CTC_TURBO2		= 2
};


typedef struct {
	UINT32	baseclock;
	UINT8	vector;
	UINT8	intr;
	UINT8	irq;
	UINT8	num;
	UINT8	cmd[4];
	UINT8	scale[4];
	UINT8	basecnt[4];
	SINT32	count[4];
	SINT32	countmax[4];
	SINT32	range[4];
} CTCCHST;

typedef struct {
	UINT	neventid;
	UINT32	ieventbit;
} CTCCHEX;

typedef struct {
	CTCCHST	s;
	CTCCHEX	e;
} CTCCH;

typedef struct {
	CTCCH	ch[3];
} CTC;


#ifdef __cplusplus
extern "C" {
#endif

void neitem_ctc(UINT id);
BRESULT ieitem_ctc(UINT id);
void ieeoi_ctc(UINT id);

void IOOUTCALL ctc_o(UINT port, REG8 dat);
REG8 IOINPCALL ctc_i(UINT port);

void ctc_reset(void);

#ifdef __cplusplus
}
#endif

