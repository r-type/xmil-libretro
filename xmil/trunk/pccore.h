
typedef struct {
	UINT8	KEY_MODE;
	UINT8	SOUND_SW;
	UINT8	SKIP_LINE;
	UINT8	NOWAIT;
	UINT8	DRAW_SKIP;
	UINT8	ROM_TYPE;
	UINT8	CPU8MHz;
	UINT8	DIP_SW;

	UINT16	samplingrate;
	UINT16	delayms;
	UINT8	MOTOR;
	UINT8	MOTORVOL;

	UINT8	BTN_RAPID;
	UINT8	BTN_MODE;

	UINT8	LINEDEPTH;
	UINT8	BLKLIGHT;

	UINT8	TEXTMODE;
	UINT8	TEXT400L;
	UINT8	LINETEXT;

	UINT8	DISPSYNC;
	UINT8	MOUSE_SW;
} XMILCFG;

typedef struct {
	UINT16	HSYNC_CLK;
	UINT8	ROM_TYPE;
	UINT8	DIP_SW;
	UINT8	SOUND_SW;
} PCCORE;

enum {
	DIPSW_RESOLUTE		= 0x01,
	DIPSW_BOOTMEDIA		= 0x04
};


#ifdef __cplusplus
extern "C" {
#endif

extern const OEMCHAR xmilversion[];

extern	XMILCFG	xmilcfg;
extern	PCCORE	pccore;
extern	BYTE	*RAM0r;
extern	BYTE	*RAM0w;
extern	DWORD	h_cntbase;

extern	BYTE	mMAIN[0x10000];
extern	BYTE	mBIOS[0x8000];
extern	BYTE	mBANK[16][0x8000];

extern	BRESULT	soundrenewal;

#define	h_cnt	Z80_ICOUNT
extern	WORD	v_cnt;

// BYTE reset_x1(BYTE ROM_TYPE, BYTE SOUND_SW, BYTE DIP_SW);

void x1r_exec(void);

void pccore_initialize(void);
void pccore_reset(void);
void pccore_deinitialize(void);

#ifdef __cplusplus
}
#endif

