
#ifdef __cplusplus
extern "C" {
#endif

enum {
	CRTC_PALB		= 0,
	CRTC_PALR		= 1,
	CRTC_PALG		= 2,
	CRTC_PLY		= 3,
	CRTC_BLACK		= 4,
	CRTC_RGBPMAX	= 5
};

#define	PAL_NORMAL		0x00
#define PAL_HIGHRESO	0x01

#define PAL_4096H		0x00
#define PAL_4096L		0x01

#define	PAL_4096BANK	0x01
#define PAL_64x2		0x02
#define PAL_4096FULL	0x04
#define	PAL_4096		0x10


#if !defined(SUPPORT_TURBOZ)
enum {
	DISPMODE_24KHZ			= 0x01,
	DISPMODE_200L			= 0x02,
	DISPMODE_TEXTYx2		= 0x04,
	DISPMODE_UNDERLINE		= 0x08,

	DISPMODE_MASKMODE		= 0x0f,
	DISPMODE_WIDTH80		= 0x40,
	DISPMODE_BANK1			= 0x80
};
#else
enum {
	DISPMODE_24KHZ			= 0x01,
	DISPMODE_200L			= 0x02,
	DISPMODE_TEXTYx2		= 0x04,
	DISPMODE_UNDERLINE		= 0x08,

	DISPMODE_SCRN64			= 0x10,
	DISPMODE64_320x200		= 0x10,
	DISPMODE64_L320x200x2	= 0x11,
	DISPMODE64_L640x200		= 0x12,
	DISPMODE64_H320x400		= 0x13,
	DISPMODE64_320x200x4096	= 0x14,
	DISPMODE64_320x100		= 0x15,
	DISPMODE64_320x100x2	= 0x16,
	DISPMODE64_L640x100		= 0x17,
	DISPMODE64_H320x200		= 0x18,
	DISPMODE64_320x100x4096	= 0x19,

	DISPMODE_MASKMODE		= 0x1f,
	DISPMODE_4096			= 0x20,
	DISPMODE_WIDTH80		= 0x40,
	DISPMODE_BANK1			= 0x80
};
#endif


enum {
	SCRN_BANK0				= 0x00,
	SCRN_BANK1				= 0x80,
	SCRN_WIDTH80			= 0x40,
	SCRN_DRAW4096			= 0x20,

	SCRN64_320x200			= 0x10,
	SCRN64_L320x200x2		= 0x11,
	SCRN64_L640x200			= 0x12,
	SCRN64_H320x400			= 0x13,
	SCRN64_320x200x4096		= 0x14,
	SCRN64_320x100			= 0x15,
	SCRN64_320x100x2		= 0x16,
	SCRN64_L640x100			= 0x17,
	SCRN64_H320x200			= 0x18,
	SCRN64_320x100x4096		= 0x19,

	SCRN64_MASK				= 0x1f,
	SCRN64_ENABLE			= 0x10
};


/*	SCRN_24KHZ		0x01	- 0:15KHz		1:24KHz
 *	SCRN_200LINE	0x02	- 0:400line		1:200line
 *	SCRN_TEXTYx2	0x04	- 0:ノーマル	1:縦に拡大
 *	SCRN_DISPVRAM	0x08	- 0:表			1:裏
 *	SCRN_ACCESSVRAM	0x10	- 0:表			1:裏
 *	SCRN_PCGMODE	0x20	- 0:互換		1:高速
 *	SCRN_CPUFONT	0x40	- 0:8ラスタ		1:16ラスタ
 *	SCRN_UNDERLINE	0x80	- 0:なし		1:あり
 */

enum {
	SCRN_24KHZ		= 0x01,
	SCRN_200LINE	= 0x02,
	SCRN_TEXTYx2	= 0x04,
	SCRN_DISPVRAM	= 0x08,
	SCRN_ACCESSVRAM	= 0x10,
	SCRN_PCGMODE	= 0x20,
	SCRN_CPUFONT	= 0x40,
	SCRN_UNDERLINE	= 0x80,

	SCRN_DISPCHANGE	= SCRN_24KHZ + SCRN_200LINE + SCRN_TEXTYx2 + SCRN_DISPVRAM + SCRN_UNDERLINE
};

enum {
	CRTCREG_HSIZE		= 0,
	CRTCREG_HDISP		= 1,
	CRTCREG_HSYNC		= 2,
	CRTCREG_PULSE		= 3,
	CRTCREG_VSIZE		= 4,
	CRTCREG_VSIZEA		= 5,
	CRTCREG_VDISP		= 6,
	CRTCREG_VSYNC		= 7,
	CRTCREG_CHRCY		= 9,
	CRTCREG_POSH		= 12,
	CRTCREG_POSL		= 13,

	CRTCREG_MAX			= 18
};


typedef struct {
	UINT8	SCRN_BITS;
	UINT8	width40;
	UINT8	regnum;
	UINT8	padding;
	UINT8	rgbp[6];
	UINT8	reg[CRTCREG_MAX];
#if defined(SUPPORT_TURBOZ)
	UINT8	EXTPALMODE;
	UINT8	EXTGRPHPAL;
	UINT8	ZPRY;
	UINT8	lastpal;
#endif
} CRTCSTAT;

typedef struct {
	UINT8	scrnflash;
	UINT8	scrnallflash;
	UINT8	remakeattr;							/* doubleatrchange */
	UINT8	palandply;

	UINT8	*gramacc;							/* curvram */
#if (!defined(MEMOPTIMIZE)) || (MEMOPTIMIZE < 100)
	UINT	updatemask;							/* updatemsk */
	UINT8	updatebit;							/* curupdt */
#else
	UINT8	updatebit;							/* curupdt */
	UINT8	_padding1;
	UINT16	updatemask;							/* updatemsk */
	UINT8	_padding2;
#endif
	UINT8	dispmode;
	UINT8	existblink;							/* blinkflag */
	UINT8	blinktime;

#if defined(SUPPORT_TURBOZ)
	UINT8	pal_bank;
	UINT8	pal_disp;
	UINT8	pal_padding[2];
#endif

	SINT32	rasterclock8;
	SINT32	rasterdisp8;
	SINT32	frameclock;
	UINT	fonty;
	UINT	yl;
	UINT	pos;
} CRTCEXT;

#if defined(SUPPORT_TURBOZ)
typedef struct {
	UINT8	text[8];
	UINT16	grph[2][64];
	UINT16	grph4096[4096];
} CRTCPAL;
#endif

typedef struct {
	CRTCEXT		e;
	CRTCSTAT	s;
#if defined(SUPPORT_TURBOZ)
	CRTCPAL		p;
#endif
} CRTC;


/* functions */

void crtc_setwidth(REG8 width40);
void crtc_update(void);

void IOOUTCALL crtc_o(UINT port, REG8 value);

void IOOUTCALL scrn_o(UINT port, REG8 value);
REG8 IOINPCALL scrn_i(UINT port);

void IOOUTCALL ply_o(UINT port, REG8 value);
void IOOUTCALL palette_o(UINT port, REG8 value);

void IOOUTCALL blackctrl_o(UINT port, REG8 value);
REG8 IOINPCALL blackctrl_i(UINT port);


#if defined(SUPPORT_TURBOZ)
REG8 IOINPCALL ply_i(UINT port);
REG8 IOINPCALL palette_i(UINT port);

void IOOUTCALL extpal_o(UINT port, REG8 value);
REG8 IOINPCALL extpal_i(UINT port);
void IOOUTCALL extgrphpal_o(UINT port, REG8 value);
REG8 IOINPCALL extgrphpal_i(UINT port);
void IOOUTCALL exttextpal_o(UINT port, REG8 value);
REG8 IOINPCALL exttextpal_i(UINT port);
void IOOUTCALL exttextdisp_o(UINT port, REG8 value);
REG8 IOINPCALL exttextdisp_i(UINT port);
#endif


#if defined(SUPPORT_TURBOZ)
void crtc_initialize(void);
#else
#define	crtc_initialize()
#endif
void crtc_reset(void);
void crtc_forcesetwidth(REG8 width);

#ifdef __cplusplus
}
#endif

