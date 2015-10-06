
#define RGB15(r,g,b)		((r) | ((g) << 5) | ((b) << 10))

#define VRAM_OFFSET(n)		((n)<<3)

#define SCREEN_HEIGHT		192
#define SCREEN_WIDTH		256

enum VRAM_A_TYPE
{
	VRAM_A_LCD						= 0,
	VRAM_A_MAIN_BG					= 1,
	VRAM_A_MAIN_BG_0x06000000		= 1 | VRAM_OFFSET(0),
	VRAM_A_MAIN_BG_0x06020000		= 1 | VRAM_OFFSET(1),
	VRAM_A_MAIN_BG_0x06040000		= 1 | VRAM_OFFSET(2),
	VRAM_A_MAIN_BG_0x06060000		= 1 | VRAM_OFFSET(3),
	VRAM_A_MAIN_SPRITE				= 2,
	VRAM_A_MAIN_SPRITE_0x06400000	= 2,
	VRAM_A_MAIN_SPRITE_0x06420000	= 2 | VRAM_OFFSET(1),
	VRAM_A_TEXTURE					= 3,
	VRAM_A_TEXTURE_SLOT0			= 3 | VRAM_OFFSET(0),
	VRAM_A_TEXTURE_SLOT1			= 3 | VRAM_OFFSET(1),
	VRAM_A_TEXTURE_SLOT2			= 3 | VRAM_OFFSET(2),
	VRAM_A_TEXTURE_SLOT3			= 3 | VRAM_OFFSET(3)
};

enum VRAM_B_TYPE
{
	VRAM_B_LCD						= 0,
	VRAM_B_MAIN_BG					= 1 | VRAM_OFFSET(1),
	VRAM_B_MAIN_BG_0x06000000		= 1 | VRAM_OFFSET(0),
	VRAM_B_MAIN_BG_0x06020000		= 1 | VRAM_OFFSET(1),
	VRAM_B_MAIN_BG_0x06040000		= 1 | VRAM_OFFSET(2),
	VRAM_B_MAIN_BG_0x06060000		= 1 | VRAM_OFFSET(3),
	VRAM_B_MAIN_SPRITE				= 2 | VRAM_OFFSET(1),
	VRAM_B_MAIN_SPRITE_0x06400000	= 2,
	VRAM_B_MAIN_SPRITE_0x06420000	= 2 | VRAM_OFFSET(1),
	VRAM_B_TEXTURE					= 3 | VRAM_OFFSET(1),
	VRAM_B_TEXTURE_SLOT0			= 3 | VRAM_OFFSET(0),
	VRAM_B_TEXTURE_SLOT1			= 3 | VRAM_OFFSET(1),
	VRAM_B_TEXTURE_SLOT2			= 3 | VRAM_OFFSET(2),
	VRAM_B_TEXTURE_SLOT3			= 3 | VRAM_OFFSET(3)
};

enum VRAM_C_TYPE
{
	VRAM_C_LCD = 0,
	VRAM_C_MAIN_BG  = 1 | VRAM_OFFSET(2),
	VRAM_C_MAIN_BG_0x06000000	= 1 | VRAM_OFFSET(0),
	VRAM_C_MAIN_BG_0x06020000	= 1 | VRAM_OFFSET(1),
	VRAM_C_MAIN_BG_0x06040000	= 1 | VRAM_OFFSET(2),
	VRAM_C_MAIN_BG_0x06060000	= 1 | VRAM_OFFSET(3),
	VRAM_C_ARM7	= 2,
	VRAM_C_ARM7_0x06000000 = 2,
	VRAM_C_ARM7_0x06020000 = 2 | VRAM_OFFSET(1),
	VRAM_C_SUB_BG	= 4,
	VRAM_C_SUB_BG_0x06200000	= 4 | VRAM_OFFSET(0),
	VRAM_C_SUB_BG_0x06220000	= 4 | VRAM_OFFSET(1),
	VRAM_C_SUB_BG_0x06240000	= 4 | VRAM_OFFSET(2),
	VRAM_C_SUB_BG_0x06260000	= 4 | VRAM_OFFSET(3),
	VRAM_C_TEXTURE	= 3 | VRAM_OFFSET(2),
	VRAM_C_TEXTURE_SLOT0	= 3 | VRAM_OFFSET(0),
	VRAM_C_TEXTURE_SLOT1	= 3 | VRAM_OFFSET(1),
	VRAM_C_TEXTURE_SLOT2	= 3 | VRAM_OFFSET(2),
	VRAM_C_TEXTURE_SLOT3	= 3 | VRAM_OFFSET(3)
};

enum VRAM_D_TYPE
{
	VRAM_D_LCD = 0,
	VRAM_D_MAIN_BG  = 1 | VRAM_OFFSET(3),
	VRAM_D_MAIN_BG_0x06000000  = 1 | VRAM_OFFSET(0),
	VRAM_D_MAIN_BG_0x06020000  = 1 | VRAM_OFFSET(1),
	VRAM_D_MAIN_BG_0x06040000  = 1 | VRAM_OFFSET(2),
	VRAM_D_MAIN_BG_0x06060000  = 1 | VRAM_OFFSET(3),
	VRAM_D_ARM7 = 2 | VRAM_OFFSET(1),
	VRAM_D_ARM7_0x06000000 = 2,
	VRAM_D_ARM7_0x06020000 = 2 | VRAM_OFFSET(1),
	VRAM_D_SUB_SPRITE  = 4,
	VRAM_D_TEXTURE = 3 | VRAM_OFFSET(3),
	VRAM_D_TEXTURE_SLOT0 = 3 | VRAM_OFFSET(0),
	VRAM_D_TEXTURE_SLOT1 = 3 | VRAM_OFFSET(1),
	VRAM_D_TEXTURE_SLOT2 = 3 | VRAM_OFFSET(2),
	VRAM_D_TEXTURE_SLOT3 = 3 | VRAM_OFFSET(3)
};

enum
{
	MODE_0_2D		= 0x10000,
	MODE_1_2D		= 0x10001,
	MODE_2_2D		= 0x10002,
	MODE_3_2D		= 0x10003,
	MODE_4_2D		= 0x10004,
	MODE_5_2D		= 0x10005
};

enum
{
	DISPLAY_BG0_ACTIVE		= (1 << 8),
	DISPLAY_BG1_ACTIVE		= (1 << 9),
	DISPLAY_BG2_ACTIVE		= (1 << 10),
	DISPLAY_BG3_ACTIVE		= (1 << 11),
	DISPLAY_SPR_ACTIVE		= (1 << 12),
	DISPLAY_WIN0_ON			= (1 << 13),
	DISPLAY_WIN1_ON			= (1 << 14),
	DISPLAY_SPR_WIN_ON		= (1 << 15)
};

#define BG_MAP_BASE(base)  ((base) << 8)

enum
{
	BG_16_COLOR		= (0 << 7),
	BG_256_COLOR	= (1 << 7),

	BG_RS_16x16		= (0 << 14),
	BG_RS_32x32		= (1 << 14),
	BG_RS_64x64		= (2 << 14),
	BG_RS_128x128	= (3 << 14),
};

enum
{
	BG_BMP8_128x128		= (BG_RS_16x16 | BG_256_COLOR),
	BG_BMP8_256x256		= (BG_RS_32x32 | BG_256_COLOR),
	BG_BMP8_512x256		= (BG_RS_64x64 | BG_256_COLOR),
	BG_BMP8_512x512		= (BG_RS_128x128 | BG_256_COLOR),
	BG_BMP8_1024x512	= (1 << 14),
	BG_BMP8_512x1024	= 0,

	BG_BMP16_128x12		= (BG_RS_16x16 | BG_256_COLOR | (1 << 2)),
	BG_BMP16_256x256	= (BG_RS_32x32 | BG_256_COLOR | (1 << 2)),
	BG_BMP16_512x256	= (BG_RS_64x64 | BG_256_COLOR | (1 << 2)),
	BG_BMP16_512x512	= (BG_RS_128x128 | BG_256_COLOR | (1 << 2)),
};

extern CRegScreen16 g_bgpalettesub;
extern CRegVram16 g_vram[0x80000];
extern CRegPalette16 g_pal[256];
extern CRegPalette16 g_palsub[256];

struct tagNdsBgReg
{
	CRegScreen16 cr;
	CRegScreen16 xdx;
	CRegScreen16 xdy;
	CRegScreen16 ydx;
	CRegScreen16 ydy;
	CRegScreen32 cx;
	CRegScreen32 cy;
};
extern tagNdsBgReg g_bg[4];
extern tagNdsBgReg g_subbg[4];


#define BG3_CR				g_bg[3].cr
#define BG3_XDX				g_bg[3].xdx
#define	BG3_XDY				g_bg[3].xdy
#define BG3_YDX				g_bg[3].ydx
#define BG3_YDY				g_bg[3].ydy
#define BG3_CX				g_bg[3].cx
#define BG3_CY				g_bg[3].cy

#define SUB_BG0_CR			g_subbg[0].cr

#define BG_GFX				g_vram
#define BG_PALETTE			g_pal
#define BG_PALETTE_SUB		g_palsub

void videoSetMode(uint32 mode);
void videoSetModeSub(uint32 mode);
uint32 vramSetMainBanks(VRAM_A_TYPE a, VRAM_B_TYPE b, VRAM_C_TYPE c, VRAM_D_TYPE d);

#define CHAR_BASE_BLOCK(n)			(((n)*0x4000)+ 0x06000000)
#define CHAR_BASE_BLOCK_SUB(n)		(((n)*0x4000)+ 0x06200000)
#define SCREEN_BASE_BLOCK(n)		(((n)*0x800) + 0x06000000)
#define SCREEN_BASE_BLOCK_SUB(n)	(((n)*0x800) + 0x06200000)


#define ndsvideo_vramnotify()		do { CNdsRegUpdate::getInstance().set(RT_VRAM); } while(0)


// ---- for win

bool ndsvideo_initialize(HWND hWnd);
void ndsvideo_deinitialize();
void ndsvideo_update();
void ndsvideo_bufferupdate();

