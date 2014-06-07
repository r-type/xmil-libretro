
enum {
	START_PAL	= 0x0a
};

enum {
	RGB24_B	= 2,
	RGB24_G	= 1,
	RGB24_R	= 0
};

typedef struct {
	UINT8	*ptr;
	int		xalign;
	int		yalign;
	int		width;
	int		height;
	UINT	bpp;
	int		extend;
} SCRNSURF;

enum {
	SCRNMODE_FULLSCREEN		= 0x01,
	SCRNMODE_HIGHCOLOR		= 0x02,
	SCRNMODE_COREHIGHCOLOR	= 0x04,
	SCRNMODE_ROTATE			= 0x10,
	SCRNMODE_ROTATEDIR		= 0x20,
	SCRNMODE_ROTATELEFT		= (SCRNMODE_ROTATE + 0),
	SCRNMODE_ROTATERIGHT	= (SCRNMODE_ROTATE + SCRNMODE_ROTATEDIR),
	SCRNMODE_ROTATEMASK		= 0x30
};

enum {
	SCRNFLAG_FULLSCREEN		= 0x01,
	SCRNFLAG_HAVEEXTEND		= 0x02,
	SCRNFLAG_ENABLE			= 0x80
};


#ifdef __cplusplus
extern "C" {
#endif

void scrnmng_setwidth(int posx, int width);
void scrnmng_setextend(int extend);
void scrnmng_setheight(int posy, int height);
#define scrnmng_setcolormode(f)		(SUCCESS)
const SCRNSURF *scrnmng_surflock(void);
//void scrnmng_surfunlock(const SCRNSURF *surf);
#define scrnmng_surfunlock(f)
#define scrnmng_update()

#define	scrnmng_isfullscreen()		(0)
#define	scrnmng_haveextend()		(0)
#define	scrnmng_getbpp()			(16)
#define	scrnmng_allflash()
#define	scrnmng_palchanged()
RGB16 scrnmng_makepal16(RGB32 pal32);

#ifdef __cplusplus
}
#endif


// ---- for palm

void scrnmng_initialize(void);
BRESULT scrnmng_create(REG8 scrnmode);
void scrnmng_destroy(void);


#define	USE_PALS		0xc0
#define	START_EXT		(START_PAL + USE_PALS)
#define	EXT_PALS		0x28
#define	TOTAL_PALS		(USE_PALS + EXT_PALS)
