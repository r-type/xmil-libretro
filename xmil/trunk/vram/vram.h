
enum {
	GRAM_SIZE		= 0x10000,
	GRAM_BANK0		= 0x00000,
	GRAM_BANK1		= 0x10000,

	GRAM_BANK0L		= 0x00000,
	GRAM_BANK0H		= 0x08000,
	GRAM_BANK1L		= 0x10000,
	GRAM_BANK1H		= 0x18000,

	GRAM_HALFSTEP	= 0x08000,

	PLANE_B		= 8,
	PLANE_R		= 16,
	PLANE_G		= 24
};

enum {
	TRAM_ANK		= 0x0000,
	TRAM_ATR		= 0x0800,
	TRAM_KNJ		= 0x1000,

	X1ATR_COLOR		= 0x07,
	X1ATR_REVERSE	= 0x08,
	X1ATR_BLINK		= 0x10,
	X1ATR_PCG		= 0x20,
	X1ATR_Yx2		= 0x40,
	X1ATR_Xx2		= 0x80,
	X1KNJ_ULINE		= 0x20
};


#ifdef __cplusplus
extern "C" {
#endif

extern	UINT8	GRP_RAM[0x20000];
extern	UINT8	tram[0x01800];

#ifdef __cplusplus
}
#endif

