#include	"compiler.h"
#include	"sysmng.h"
#include	"z80core.h"


	UINT	sys_updates;


#if defined(DISPCLOCK) && (!defined(ADV_SIMULATE))

typedef struct {
	UINT16	attr0;
	UINT16	attr1;
	UINT16	attr2;
	UINT16	attr3;
} ADVOAM;

#define	ADVVIDEOOAM	((volatile ADVOAM *)0x07000000)

static const UINT32 spd[8*12] = {
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
				0x012100, 0x121010, 0x010121, 0x000010};

static struct {
	UINT32	tick;
	UINT32	clock;
//	UINT32	draws;
} workclock;

void sysmng_initialize(void) {

	UINT	i;
	UINT32	*p;

	sys_updates = 0;
	for (i=0; i<256; i++) {
		ADVVIDEOOAM[i].attr0 = 160;
		ADVVIDEOOAM[i].attr1 = 240;
		ADVVIDEOOAM[i].attr2 = 512;
		ADVVIDEOOAM[i].attr3 = 0;
	}
	p = (UINT32 *)(ADVVIDEOBUFFER + 0x14000);
	for (i=0; i<8*12; i++) {
		p[i] = spd[i];
	}
	ADVVIDEOPAL[0x100] = (0 << 10) + (0 << 5) + (0 << 0);
	ADVVIDEOPAL[0x101] = (0 << 10) + (0 << 5) + (1 << 0);
	ADVVIDEOPAL[0x102] = (0 << 10) + (0 << 5) + (31 << 0);

	ADVVIDEOOAM[0].attr0 = 160 - 8;
	ADVVIDEOOAM[0].attr1 = 0x1000 + 240 - 25;
	ADVVIDEOOAM[1].attr0 = 160 - 8;
	ADVVIDEOOAM[1].attr1 = 0x1000 + 240 - 20;
	ADVVIDEOOAM[1].attr2 = 512 + 10;
	ADVVIDEOOAM[2].attr0 = 160 - 8;
	ADVVIDEOOAM[2].attr1 = 0x1000 + 240 - 18;
	ADVVIDEOOAM[3].attr0 = 160 - 8;
	ADVVIDEOOAM[3].attr1 = 0x1000 + 240 - 13;
	ADVVIDEOOAM[4].attr0 = 160 - 8;
	ADVVIDEOOAM[4].attr1 = 0x1000 + 240 - 8;
}

void sysmng_clockreset(void) {

	workclock.tick = GETTICK();
	workclock.clock = CPU_CLOCK;
//	workclock.draws = drawtime;				// drawcount;
}

void sysmng_dispclock(void) {

	SINT32	tick;
	SINT32	khz;

	tick = GETTICK() - workclock.tick;
	if (tick < 1000) {
		return;
	}
	workclock.tick += tick;
//	workclock.fps = ((drawtime - workclock.draws) * 10000) / tick;
//	workclock.draws = drawtime;
	khz = (CPU_CLOCK - workclock.clock) / tick;
	workclock.clock = CPU_CLOCK;
	ADVVIDEOOAM[4].attr2 = 512 + (khz % 10);
	khz /= 10;
	ADVVIDEOOAM[3].attr2 = 512 + (khz % 10);
	khz /= 10;
	ADVVIDEOOAM[2].attr2 = 512 + (khz % 10);
	khz /= 10;
	ADVVIDEOOAM[0].attr2 = 512 + khz;
}

#endif

