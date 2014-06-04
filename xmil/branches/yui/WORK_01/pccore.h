
typedef struct {
	UINT8	ROM_TYPE;
	UINT8	DIP_SW;
	UINT8	fddequip;

	UINT8	DISPSYNC;
	UINT8	RASTER;
	UINT8	skipline;
	UINT16	skiplight;

#if !defined(DISABLE_SOUND)
	UINT16	samplingrate;
	UINT16	delayms;
#if defined(SUPPORT_TURBOZ) || defined(SUPPORT_OPM)
	UINT8	vol_fm;
#endif
	UINT8	vol_ssg;
	UINT8	MOTOR;
	UINT8	MOTORVOL;
#endif
#if defined(SUPPORT_TURBOZ) || defined(SUPPORT_OPM)
	UINT8	SOUND_SW;
#endif

	UINT8	MOUSE_SW;
	UINT8	KEY_MODE;
	UINT8	BTN_RAPID;
	UINT8	BTN_MODE;
} XMILCFG;

typedef struct {
#if !defined(FIX_Z80A)
	UINT32	baseclock;
	UINT	multiple;
	UINT32	realclock;
#endif
	UINT8	ROM_TYPE;
	UINT8	DIP_SW;
	UINT8	SOUND_SW;
} PCCORE;

typedef struct {
	SINT32	syncclock;
	SINT32	framebaseclock;			/* by 4MHz */
	UINT8	vsync;
	UINT8	drawframe;
	UINT8	soundrenewal;
} CORESTAT;

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

#if !defined(MAINFRAMES_OLD)
void neitem_mainframes(UINT id);
#else
void neitem_disp(UINT id);
void neitem_vsync(UINT id);
#endif

void pccore_initialize(void);
void pccore_reset(void);
void pccore_deinitialize(void);
void pccore_exec(BRESULT draw);

#ifdef __cplusplus
}
#endif

