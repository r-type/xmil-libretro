
typedef struct {
	UINT8	drv;
	UINT8	cmd;
	short	skip;
	short	off;
	short	crc_off;
	UINT8	crc_dat[8];

	UINT8	type;
	UINT8	treg;
	UINT8	sct_bak;
	UINT8	data;
	SINT8	step;
	UINT8	motor;

	UINT8	c;
	UINT8	h;
	UINT8	r;
	UINT8	n;

	UINT32	busyclock;
	UINT32	busystart;
	BYTE	media;
	BYTE	rreg;
} FDC;


#ifdef __cplusplus
extern "C" {
#endif

extern	UINT8	fdcdummyread;

void IOOUTCALL fdc_o(UINT port, REG8 value);		// x1_fdc_w
REG8 IOINPCALL fdc_i(UINT port);					// x1_fdc_r

void fdc_reset(void);

#ifdef __cplusplus
}
#endif

