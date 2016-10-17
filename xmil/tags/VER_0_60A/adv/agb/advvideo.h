
enum {
	ADVV_WIDTH		= 240,
	ADVV_HEIGHT		= 160
};

enum {
	ADVV_MODE0			= 0x0000,
	ADVV_MODE1			= 0x0001,
	ADVV_MODE2			= 0x0002,
	ADVV_MODE3			= 0x0003,
	ADVV_MODE4			= 0x0004,
	ADVV_MODE5			= 0x0005,
	ADVV_PLANE			= 0x0010,
	ADVV_MAPLINEAR  	= 0x0040,
	ADVV_BG0			= 0x0100,
	ADVV_BG1			= 0x0200,
	ADVV_BG2			= 0x0400,
	ADVV_BG3			= 0x0800,
	ADVV_OBJ			= 0x1000,
	ADVV_WIN0			= 0x2000,
	ADVV_WIN1			= 0x4000,
	ADVV_OBJWIN			= 0x8000
};

enum {
	ADVV_BGPRYSFT		= 0,
	ADVV_BGCHRBASESFT	= 2,
	ADVV_BGMOSIACSFT	= 6,
	ADVV_BGCOLORSFT		= 7,
	ADVV_BGSCRNBASESFT	= 8,
	ADVV_BGOVERFLOWSFT	= 13,
	ADVV_BGSCRNSIZE		= 14
};

enum {
	ADVV_EABG0		= 0x0001,
	ADVV_EABG1		= 0x0002,
	ADVV_EABG2		= 0x0004,
	ADVV_EABG3		= 0x0008,
	ADVV_EAOBJ		= 0x0010,
	ADVV_EABD		= 0x0020,
	ADVV_EALPHA		= 0x0040,
	ADVV_EWHITE		= 0x0080,
	ADVV_EBLACK		= 0x00c0,
	ADVV_EBBG0		= 0x0100,
	ADVV_EBBG1		= 0x0200,
	ADVV_EBBG2		= 0x0400,
	ADVV_EBBG3		= 0x0800,
	ADVV_EVASFT		= 0,
	ADVV_EVBSFT		= 8
};

#define	REG_DISPCNT			((volatile UINT16 *)0x04000000)
#define	REG_VCOUNT			((volatile UINT16 *)0x04000006)
#define	REG_BG0CNT			((volatile UINT16 *)0x04000008)
#define	REG_BG1CNT			((volatile UINT16 *)0x0400000a)
#define	REG_BG2CNT			((volatile UINT16 *)0x0400000c)
#define	REG_BG3CNT			((volatile UINT16 *)0x0400000e)
#define	REG_BG0HOFS			((volatile UINT16 *)0x04000010)
#define	REG_BG0VOFS			((volatile UINT16 *)0x04000012)
#define	REG_BG1HOFS			((volatile UINT16 *)0x04000014)
#define	REG_BG1VOFS			((volatile UINT16 *)0x04000016)
#define	REG_BG2HOFS			((volatile UINT16 *)0x04000018)
#define	REG_BG2VOFS			((volatile UINT16 *)0x0400001a)
#define	REG_BG3HOFS			((volatile UINT16 *)0x0400001c)
#define	REG_BG3VOFS			((volatile UINT16 *)0x0400001e)
#define	REG_BLDCNT			((volatile UINT16 *)0x04000050)
#define	REG_BLDALPHA		((volatile UINT16 *)0x04000052)
#define	REG_BLDY			((volatile UINT16 *)0x04000054)

#define	ADVVIDEOPAL			((UINT16 *)0x05000000)
#define	ADVVIDEOBUFFER		((UINT8 *) 0x06000000)


// ----

#define	advv_setmode(mode)												\
	do {																\
		*REG_DISPCNT = (mode);											\
	} while (/*CONSTCOND*/ 0)

#define	advv_palupdate()
#define	advv_bufferupdate()

