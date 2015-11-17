#include "compiler.h"
#include "libnds.h"
#include "sysmng.h"
#include "z80core.h"
#include "resource.h"

#if defined(DISPCLOCK) && (!defined(NDS_SIMULATE))

#define NDSVIDEO_WIDTH		SCREEN_WIDTH
#define NDSVIDEO_HEIGHT		SCREEN_HEIGHT

struct tagNdsOam
{
	UINT16 wAttr0;
	UINT16 wAttr1;
	UINT16 wAttr2;
	UINT16 wAttr3;
};

static const UINT32 s_spd[8*12] =
{
				0x001100, 0x012210, 0x121121, 0x121121,	// 0
				0x121121, 0x121121, 0x012210, 0x001100,
				0x000100, 0x001210, 0x012210, 0x001210,	// 1
				0x001210, 0x001210, 0x001210, 0x000100,
				0x001100, 0x012210, 0x121121, 0x010121,	// 2
				0x001210, 0x012110, 0x122221, 0x011110,
				0x011110, 0x122221, 0x011210, 0x012210,	// 3
				0x011121, 0x121121, 0x012210, 0x001100,
				0x000100, 0x001210, 0x012210, 0x121210,	// 4
				0x121210, 0x122221, 0x011210, 0x000100,
				0x011110, 0x122221, 0x121110, 0x012210,	// 5
				0x011121, 0x121121, 0x012210, 0x001100,
				0x000100, 0x001210, 0x012100, 0x122210,	// 6
				0x121121, 0x121121, 0x012210, 0x001100,
				0x011110, 0x122221, 0x011121, 0x000121,	// 7
				0x001210, 0x001210, 0x012100, 0x001000,
				0x001100, 0x012210, 0x121121, 0x012210,	// 8
				0x121121, 0x121121, 0x012210, 0x001100,
				0x001100, 0x012210, 0x121121, 0x121121,	// 9
				0x012221, 0x001121, 0x012210, 0x001100,
				0x000000, 0x000000, 0x000000, 0x000000,	// .
				0x000000, 0x010000, 0x121000, 0x010000, 
				0x010000, 0x121010, 0x010121, 0x001210,	// %
				0x012100, 0x121010, 0x010121, 0x000010,
};

static struct
{
	UINT32 uTick;
	UINT32 uClock;
//	UINT32 uDraws;
} s_workclock;

void sysmng_initialize()
{
	volatile tagNdsOam *pOam = reinterpret_cast<tagNdsOam *>(OAM);

	UINT i;
	for (i=0; i<128; i++)
	{
		pOam[i].wAttr0 = ATTR0_DISABLED;
		pOam[i].wAttr1 = 0;
		pOam[i].wAttr2 = 0;
	}

	volatile UINT32 *p = reinterpret_cast<UINT32 *>(SPRITE_GFX);
	for (i=0; i<8*12; i++)
	{
		p[i] = s_spd[i];
	}

	SPRITE_PALETTE[0] = (0 << 10) + (0 << 5) + (0 << 0);
	SPRITE_PALETTE[1] = (0 << 10) + (0 << 5) + (1 << 0);
	SPRITE_PALETTE[2] = (0 << 10) + (0 << 5) + (31 << 0);

	pOam[0].wAttr0 = ATTR0_DISABLED;
	pOam[0].wAttr1 = ATTR1_SIZE_8 | ATTR1_FLIP_X | (NDSVIDEO_WIDTH - 30);
//	pOam[0].wAttr2 = ATTR2_PALETTE(0) | ATTR2_PRIORITY(0) | 0;

	pOam[1].wAttr0 = ATTR0_SQUARE | ATTR0_COLOR_16 | ATTR0_TYPE_NORMAL | ATTR0_NORMAL | (NDSVIDEO_HEIGHT - 8);
	pOam[1].wAttr1 = ATTR1_SIZE_8 | ATTR1_FLIP_X | (NDSVIDEO_WIDTH - 25);
//	pOam[1].wAttr2 = ATTR2_PALETTE(0) | ATTR2_PRIORITY(0) | 0;

	pOam[2].wAttr0 = ATTR0_SQUARE | ATTR0_COLOR_16 | ATTR0_TYPE_NORMAL | ATTR0_NORMAL | (NDSVIDEO_HEIGHT - 8);
	pOam[2].wAttr1 = ATTR1_SIZE_8 | ATTR1_FLIP_X | (NDSVIDEO_WIDTH - 20);
	pOam[2].wAttr2 = ATTR2_PALETTE(0) | ATTR2_PRIORITY(0) | 10;

	pOam[3].wAttr0 = ATTR0_SQUARE | ATTR0_COLOR_16 | ATTR0_TYPE_NORMAL | ATTR0_NORMAL | (NDSVIDEO_HEIGHT - 8);
	pOam[3].wAttr1 = ATTR1_SIZE_8 | ATTR1_FLIP_X | (NDSVIDEO_WIDTH - 18);
//	pOam[3].wAttr2 = ATTR2_PALETTE(0) | ATTR2_PRIORITY(0) | 0;

	pOam[4].wAttr0 = ATTR0_SQUARE | ATTR0_COLOR_16 | ATTR0_TYPE_NORMAL | ATTR0_NORMAL | (NDSVIDEO_HEIGHT - 8);
	pOam[4].wAttr1 = ATTR1_SIZE_8 | ATTR1_FLIP_X | (NDSVIDEO_WIDTH - 13);
//	pOam[4].wAttr2 = ATTR2_PALETTE(0) | ATTR2_PRIORITY(0) | 0;

	pOam[5].wAttr0 = ATTR0_SQUARE | ATTR0_COLOR_16 | ATTR0_TYPE_NORMAL | ATTR0_NORMAL | (NDSVIDEO_HEIGHT - 8);
	pOam[5].wAttr1 = ATTR1_SIZE_8 | ATTR1_FLIP_X | (NDSVIDEO_WIDTH - 8);
//	pOam[5].wAttr2 = ATTR2_PALETTE(0) | ATTR2_PRIORITY(0) | 0;
}

void sysmng_clockreset()
{
	s_workclock.uTick = GETTICK();
	s_workclock.uClock = CPU_CLOCK;
//	s_workclock.uDraws = drawtime;				// drawcount;
}

void sysmng_dispclock()
{
	const SINT32 nTick = GETTICK() - s_workclock.uTick;
	if (nTick < 1000)
	{
		return;
	}
	const UINT uCpuClock = CPU_CLOCK;
	s_workclock.uTick += nTick;
//	SINT32 nFps = ((drawtime - s_workclock.uDraws) * 10000) / nTick;
//	s_workclock.uDraws = drawtime;
	SINT32 nKhz = (uCpuClock - s_workclock.uClock) / nTick;
	s_workclock.uClock = uCpuClock;

	volatile tagNdsOam *pOam = reinterpret_cast<tagNdsOam *>(OAM);
	pOam[5].wAttr2 = ATTR2_PALETTE(0) | ATTR2_PRIORITY(0) | (nKhz % 10);
	nKhz /= 10;
	pOam[4].wAttr2 = ATTR2_PALETTE(0) | ATTR2_PRIORITY(0) | (nKhz % 10);
	nKhz /= 10;
	pOam[3].wAttr2 = ATTR2_PALETTE(0) | ATTR2_PRIORITY(0) | (nKhz % 10);
	nKhz /= 10;
	pOam[1].wAttr2 = ATTR2_PALETTE(0) | ATTR2_PRIORITY(0) | (nKhz % 10);
	nKhz /= 10;
	if (nKhz)
	{
		pOam[0].wAttr0 = ATTR0_SQUARE | ATTR0_COLOR_16 | ATTR0_TYPE_NORMAL | ATTR0_NORMAL | (NDSVIDEO_HEIGHT - 8);
		pOam[0].wAttr2 = ATTR2_PALETTE(0) | ATTR2_PRIORITY(0) | (nKhz % 10);
	}
	else
	{
		pOam[0].wAttr0 = ATTR0_DISABLED;
	}
}

#endif	// defined(DISPCLOCK) && (!defined(NDS_SIMULATE))



#if !defined(TRACE)

static int s_nFddCount[2];

static void updateLamp(UINT uDrv, bool bSet)
{
	int nPosX = 0;
	switch(uDrv)
	{
		case 0:
			nPosX = 12;
			break;

		case 1:
			nPosX = 112;
			break;

		default:
			return;
	}

	const int nSrcX = (bSet) ? 8 : 0;
	const unsigned char *p = reinterpret_cast<const unsigned char *>(g_sDemoBmp);
	unsigned char *q = reinterpret_cast<unsigned char *>(BG_GFX_SUB);
	p = p + nSrcX + (192 * 256);
	q = q + nPosX + (9 * 256);
	for (int y=0; y<3; y++)
	{
		for (int x=0; x<8; x+=2)
		{
			*(reinterpret_cast<uint16 *>(q + x)) = *(reinterpret_cast<const uint16 *>(p + x));
		}
		p = p + 256;
		q = q + 256;
	}
}

void sysmng_fddaccess(UINT8 cDrv)
{
	if (cDrv < 2)
	{
		if (s_nFddCount[cDrv] <= 0)
		{
			updateLamp(cDrv, true);
		}
		s_nFddCount[cDrv] = 20;

	}
}

void sysmng_fddsync(UINT uFrames)
{
	for (int i=0; i<2; i++)
	{
		if (s_nFddCount[i] > 0)
		{
			s_nFddCount[i] -= uFrames;
			if (s_nFddCount[i] <= 0)
			{
				updateLamp(i, false);
			}
		}
	}
}

#endif	// !defined(TRACE)

