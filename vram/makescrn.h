
enum {
	MAKESCRN_320x200S	= 0,
	MAKESCRN_640x200S	= 1,
	MAKESCRN_320x200H	= 2,
	MAKESCRN_640x200H	= 3,
	MAKESCRN_320x400	= 4,
	MAKESCRN_640x400	= 5,
	MAKESCRN_4096		= 6
};


typedef struct {
	UINT8	scrnflash;
	UINT8	_scrnallflash;
	UINT8	remakeattr;							// doubleatrchange
	UINT8	palandply;

	UINT8	*disp1;
	UINT8	*disp2;
	UINT8	dispflag;
	UINT8	existblink;							// blinkflag
	UINT8	blinktest;
	UINT8	blinktime;

	UINT8	dispmode;
	UINT8	drawmode;
	UINT8	nextdraw;
	UINT8	_padding;

	UINT	vramtop;
	UINT	fontycnt;
	UINT	fontcy;
	UINT	charcy;

	UINT	surfcx;
	UINT	surfcy;
	UINT	surfrx;
	UINT	surfsy;
	UINT	surfstep;
	UINT	vramsize;
} MAKESCRN;


#ifdef __cplusplus
extern "C" {
#endif

extern	MAKESCRN	makescrn;

extern	UINT8	scrnallflash;
extern	UINT	drawtime;


void scrnupdate(void);
void makescrn_initialize(void);
void makescrn_reset(void);

#ifdef __cplusplus
}
#endif

