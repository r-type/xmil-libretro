
// ---- TRAM

enum {
	TRAMATR_COLOR	= 0x07,
	TRAMATR_REVERSE	= 0x08,
	TRAMATR_BLINK	= 0x10,
	TRAMATR_PCG		= 0x20,
	TRAMATR_Yx2		= 0x40,
	TRAMATR_Xx2		= 0x80,
	TRAMKNJ_ULINE	= 0x20
};

enum {
	UPDATE_TRAM		= (1 << 5),
	UPDATE_VRAM0	= (1 << 6),
	UPDATE_VRAM1	= (1 << 7),
	UPDATE_VRAM		= UPDATE_VRAM0 | UPDATE_VRAM1,
	UPDATE_TVRAM	= UPDATE_VRAM0 | UPDATE_VRAM1 | UPDATE_TRAM
};

typedef struct {
	UINT8	atr;
	UINT8	ank;
	UINT8	knj;
	UINT8	_udt;
} TRAM;


// ---- GRAM

enum {
	GRAM_BANK0		= 0x00000,
	GRAM_BANK1		= 0x10000,

	GRAM_BANK0L		= 0x00000,
	GRAM_BANK0H		= 0x00400,
	GRAM_BANK1L		= 0x10000,
	GRAM_BANK1H		= 0x10400,

	GRAM_HALFSTEP	= 0x00400,

	GRAM_LINESTEP	= 0x0800,
	GRAM_B			= 0x0000,
	GRAM_R			= 0x4000,
	GRAM_G			= 0x8000,
	GRAM_RGBMASK	= 0xc000
};

#define	PORT2GRAM(port)		((port) - 0x4000)
#define	PORT2GRAM2(port)	((port) & (~0xc000))
#define	TRAM2GRAM(addr)		(addr)

#ifdef __cplusplus
extern "C" {
#endif

extern	TRAM	tram[0x800];
extern	UINT8	gram[];
extern	UINT8	vramupd[0x800];

#ifdef __cplusplus
}
#endif

#define	TRAM_ANK(addr)		tram[addr].ank
#define	TRAM_KNJ(addr)		tram[addr].knj
#define	TRAM_ATR(addr)		tram[addr].atr
#define	TRAMUPDATE(addr)	vramupd[addr]

