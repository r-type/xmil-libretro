
/* #define SUPPORT_MOTORRISEUP */

enum {
	FDCDIR_NONE			= 0x00,
	FDCDIR_TAO			= 0x01,
	FDCDIR_IN			= 0x02,
	FDCDIR_OUT			= 0x03
};

enum {
	FDCMOTOR_STOP		= 0,
	FDCMOTOR_STARTING	= 1,
	FDCMOTOR_READY		= 2,
	FDCMOTOR_STOPING	= 3
};

typedef struct {
	UINT8	cmd;
	UINT8	ctype;
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

#if !defined(CONST_DISKIMAGE)
	UINT8	wt_mode;
	UINT8	wt_sectors;
	UINT8	wt_padding[2];
	UINT	wt_ptr;
	UINT	wt_datpos;
	UINT	wt_datsize;

	UINT8	buffer[0x3000];
#else
	UINT8	crcbuf[6];
#endif
} FDCSTAT;

typedef struct {
	UINT8	diskstat[4];
#if defined(CONST_DISKIMAGE)
	UINT8	*buffer;
#endif
} FDCEXT;

typedef struct {
	FDCSTAT	s;
	FDCEXT	e;
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

void IOOUTCALL fdc_o(UINT port, REG8 value);
REG8 IOINPCALL fdc_i(UINT port);

void fdc_reset(void);

#ifdef __cplusplus
}
#endif

