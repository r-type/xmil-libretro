
// #define	SUPPORT_MOTORRISEUP

enum {
	FDCDIR_NONE			= 0x00,
	FDCDIR_IN			= 0x01,
	FDCDIR_OUT			= 0x02,
	FDCDIR_TAO			= 0x03
};

enum {
	FDC_BUFFERS		= 0x3000
};

enum {
	FDCMOTOR_STOP		= 0,
	FDCMOTOR_STARTING	= 1,
	FDCMOTOR_READY		= 2,
	FDCMOTOR_STOPING	= 3
};

typedef struct {
	UINT8	cmd;
	UINT8	type;
	UINT8	data;
	SINT8	step;
	UINT8	drv;
	UINT8	motor;
	UINT8	creg;
	UINT8	rreg;

	UINT8	c;
	UINT8	h;
	UINT8	r;
	UINT8	n;

	UINT8	ctbl[4];

	UINT8	media;
	UINT8	stat;

	UINT8	bufmedia;
	UINT8	bufunit;
	UINT	buftrack;
	UINT8	bufsc;
	UINT8	bufdir;
	UINT8	bufwrite;
	UINT8	bufmark;
	UINT	bufsize;
	UINT	bufpos;

	UINT8	busy;
	UINT8	hole;
	UINT8	curtime;
	UINT8	equip;
	UINT	crcnum;

	SINT32	loopclock;

#if defined(SUPPORT_MOTORRISEUP)
	UINT8	motorevent[4];
	SINT32	motorclock[4];
#endif

	UINT8	buffer[FDC_BUFFERS];
} FDCSTAT;

typedef struct {
	UINT	dummy;
} FDCEXT;

typedef struct {
	FDCSTAT	s;
} FDC;


#ifdef __cplusplus
extern "C" {
#endif

void neitem_fdcbusy(UINT id);
#if defined(SUPPORT_MOTORRISEUP)
void fdc_callback(void);
#else
#define	fdc_callback()
#endif

void IOOUTCALL fdc_o(UINT port, REG8 value);		// x1_fdc_w
REG8 IOINPCALL fdc_i(UINT port);					// x1_fdc_r

void fdc_reset(void);

#ifdef __cplusplus
}
#endif

