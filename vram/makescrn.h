
#define	TRAM_MAX	0x800

#define	UPDATEVRAM0BIT	6
#define	UPDATEVRAM1BIT	7
#define	UPDATE_TRAM		(1 << 5)
#define	UPDATE_VRAM0	(1 << UPDATEVRAM0BIT)
#define	UPDATE_VRAM1	(1 << UPDATEVRAM1BIT)
#define	UPDATE_VRAM		(UPDATE_VRAM0 | UPDATE_VRAM1)
#define	UPDATE_TVRAM	(UPDATE_VRAM0 | UPDATE_VRAM1 | UPDATE_TRAM)

#define	ANK_24KHz		1
#define	KNJ_24KHz		2

enum {
	MAKESCRN_PAL		= 0x01,
	MAKESCRN_PLY		= 0x02,
	MAKESCRN_MODE		= 0x04
};

typedef struct {
	UINT8	scrnflash;
	UINT8	remakeattr;							// doubleatrchange
	UINT8	palandply;
	UINT8	existblink;							// blinkflag


//	UINT8	*disp1;
//	UINT8	dispflag;
//	UINT8	blinktest;
//	UINT8	blinktime;

	UINT	fontycnt;
	UINT	fontcy;
	UINT	charcy;

	UINT	surfcx;
	UINT	surfcy;
	UINT	surfrx;
	UINT	surfstep;
	UINT	vramsize;

} MAKESCRN;


#ifdef __cplusplus
extern "C" {
#endif

extern	MAKESCRN	makescrn;

extern	BYTE	updatetmp[0x800+0x101];
extern	BYTE	scrnallflash;
extern	DWORD	drawtime;


void init_draw(void);
void scrnupdate(void);

#ifdef __cplusplus
}
#endif

