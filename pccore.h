
typedef struct {
	UINT8	ROM_TYPE;
	UINT8	DIP_SW;
	UINT8	fddequip;

	UINT8	DISPSYNC;
	UINT8	RASTER;
	UINT8	skipline;
	UINT16	skiplight;

	UINT16	samplingrate;
	UINT16	delayms;
	UINT8	SOUND_SW;
	UINT8	MOTOR;
	UINT8	MOTORVOL;

	UINT8	MOUSE_SW;
	UINT8	KEY_MODE;
	UINT8	BTN_RAPID;
	UINT8	BTN_MODE;
} XMILCFG;

typedef struct {
	UINT32	baseclock;
	UINT	multiple;
	UINT32	realclock;

	UINT8	ROM_TYPE;
	UINT8	DIP_SW;
	UINT8	SOUND_SW;
} PCCORE;

typedef struct {
	SINT32	dispclock;
	SINT32	syncclock;
	UINT8	vsync;
	UINT8	drawframe;
	UINT8	soundrenewal;
} CORESTAT;

enum {
	RASTER_CLOCK		= 250
};

enum {
	DIPSW_RESOLUTE		= 0x01,
	DIPSW_BOOTMEDIA		= 0x04
};


#ifdef __cplusplus
extern "C" {
#endif

extern const OEMCHAR xmilversion[];

extern	XMILCFG		xmilcfg;
extern	PCCORE		pccore;
extern	CORESTAT	corestat;
extern	BYTE		*RAM0r;
extern	BYTE		*RAM0w;

extern	UINT8	mMAIN[0x10000];
extern	UINT8	mBIOS[0x8000];
#if defined(SUPPORT_BANKMEM)
extern	UINT8	mBANK[16][0x8000];
#endif

void neitem_disp(UINT id);
void neitem_vsync(UINT id);

void pccore_initialize(void);
void pccore_reset(void);
void pccore_deinitialize(void);
void pccore_exec(BRESULT draw);

#ifdef __cplusplus
}
#endif

