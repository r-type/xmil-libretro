#include "compiler.h"
#include "libnds.h"
#include "xmil9.h"
#if defined(_WIN32) && defined(NDS_SIMULATE)
#include "dosio.h"
#endif
#include "sysmng.h"
#include "z80core.h"
#include "pccore.h"
#include "iocore.h"
#include "joymng.h"
#include "timing.h"
#include "makescrn.h"
#include "fddfile.h"
#include "font.h"
#include "sound/nds9psg.h"
#include "vram.h"
#include "vsyncff.h"
#include "resource.h"
#include "softkbd9.h"
#if defined(SUPPORT_ROMEO2)
#include "ds_fpga.h"
#include "nds_fpga_raw.h"
#endif	// defined(SUPPORT_ROMEO2)


#if 0 && defined(TRACE)
	XMILOSCFG	xmiloscfg = {1, 1};
#else
	XMILOSCFG	xmiloscfg = {0, 0};
#endif

#define	MAX_FRAMESKIP		4


#if defined(NDS_SIMULATE)
static bool diskimgset1(REG8 cDrv, LPCTSTR lpcszFile)
{
	FILEH fh = FILEH_INVALID;
	UINT8 *pWork = 0;
	do
	{
		fh = file_open_rb_c(lpcszFile);
		if (fh == FILEH_INVALID)
		{
			break;
		}
		const UINT uSize = file_getsize(fh);
		if (uSize == 0)
		{
			break;
		}
		pWork = new UINT8 [uSize];
		if (!pWork)
		{
			break;
		}
		if (file_read(fh, pWork, uSize) != uSize)
		{
			break;
		}
		file_close(fh);
		fddfile_set(cDrv, FTYPE_D88, pWork, uSize);
		return true;
	} while(0 /*CONSTCOND*/);

	if (pWork)
	{
		delete[] pWork;
	}
	if (fh != FILEH_INVALID)
	{
		file_close(fh);
	}
	return false;
}

static void diskimgset()
{
	diskimgset1(0, _T("disk1.d88"));
	diskimgset1(1, _T("disk2.d88"));
}

#else	// defined(NDS_SIMULATE)

static bool diskimgset1(REG8 cDrv, const unsigned char *lpcsData, UINT uSize)
{
	if ((lpcsData) && (uSize))
	{
		fddfile_set(cDrv, FTYPE_D88, lpcsData, uSize);
		return true;
	}
	return false;
}

static void diskimgset()
{
	diskimgset1(0, __extromimage.sDisk1, __extromimage.uDisk1Size);
	diskimgset1(1, __extromimage.sDisk2, __extromimage.uDisk2Size);
}
#endif	// defined(NDS_SIMULATE)



// ---- bitmap
#if defined(SUPPORT_ROMEO2)
static void withRomeo2()
{
	const unsigned char *p = reinterpret_cast<const unsigned char *>(g_sDemoBmp);
	p = p + 16 + (192 * 256);

	unsigned char *q = reinterpret_cast<unsigned char *>(BG_GFX_SUB);
	q = q + 188 + (64 * 256);

	for (int y=0; y<8; y++)
	{
		for (int x=0; x<68; x+=2)
		{
			*(reinterpret_cast<uint16 *>(q + x)) = *(reinterpret_cast<const uint16 *>(p + x));
		}
		p = p + 256;
		q = q + 256;
	}
}
#endif	// defined(SUPPORT_ROMEO2)


// ----

struct tagXmilMain
{
	UINT uFrameCount;
	UINT uWaitCount;
	UINT uFrameMax;
};
typedef struct tagXmilMain		XMILMAIN;
typedef struct tagXmilMain		*PXMILMAIN;


static void framereset(XMILMAIN &xmm, UINT uCount)
{
	xmm.uFrameCount = 0;
	sysmng_dispclock();
	sysmng_fddsync(uCount);
}

static void processwait(XMILMAIN &xmm, UINT uCount)
{
	if (timing_getcount() >= uCount)
	{
		timing_setcount(0);
		framereset(xmm, uCount);
	}
	else
	{
		Sleep(1);
	}
}

static void exec1frame(XMILMAIN &xmm)
{
	joymng_setflags();
	pccore_exec(xmm.uFrameCount == 0);
	xmm.uFrameCount++;
	nds9psg_sync(&nds9psg);
	softkbd9_sync();
}

int nds9main()
{
	XMILMAIN	xmilmain;

	if ((sizeof(z80core) != 0x40) ||
		(sizeof(dma) != 0x40) ||
		(sizeof(iocore) != 0x214) ||
		(sizeof(ppi) != 0x4) ||
		(sizeof(crtc) != 0x44) ||
		(sizeof(nds9psg) != 0x18) ||
		(sizeof(subcpu) != 0x4c) ||
		(sizeof(ctc) != 0xe4) ||
		(sizeof(cgrom) != 0xc) ||
		(sizeof(fdc) != 0x40))
	{
		while(1) { Sleep(1000); }
	}

	TRACEINIT();

//	xmilopen();

	irqInit();
	irqSet(IRQ_VBLANK, vsyncff_int);
	irqSet(IRQ_TIMER3, ndscore_timer3int);

	ndscore_timer3init();
	vsyncff_init();


	irqEnable(IRQ_VBLANK | IRQ_TIMER3);

#if defined(DISPCLOCK) && (!defined(NDS_SIMULATE)) 
#define VRAMBPARAM	VRAM_B_MAIN_SPRITE_0x06400000
#else	// defined(DISPCLOCK) && (!defined(NDS_SIMULATE))
#define VRAMBPARAM	VRAM_B_LCD
#endif	// defined(DISPCLOCK) && (!defined(NDS_SIMULATE))
#if defined(TRACE)
#define VRAMCPARAM	VRAM_C_SUB_BG
#else	// defined(TRACE)
#define VRAMCPARAM	VRAM_C_SUB_BG_0x06200000
#endif	// defined(TRACE)

	vramSetMainBanks(VRAM_A_MAIN_BG_0x06000000, VRAMBPARAM, VRAMCPARAM, VRAM_D_LCD);

#if defined(DISPCLOCK) && (!defined(NDS_SIMULATE))
	videoSetMode(MODE_5_2D | DISPLAY_SPR_ACTIVE | DISPLAY_BG3_ACTIVE | DISPLAY_SPR_1D);
#else	// defined(DISPCLOCK) && (!defined(NDS_SIMULATE))
	videoSetMode(MODE_5_2D | DISPLAY_BG3_ACTIVE);
#endif	// defined(DISPCLOCK) && (!defined(NDS_SIMULATE))

#if MAKESCRN_VRAMFF
	BG3_CR = BG_BMP8_512x256;
#else	// MAKESCRN_VRAMFF
	BG3_CR = BG_BMP8_256x256;
#endif	// MAKESCRN_VRAMFF
	BG3_XDX = 1 << 8;
	BG3_XDY = 0;
	BG3_YDX = 0;
	BG3_YDY = 1 << 8;
	BG3_CX = 0;
	BG3_CY = 0 << 8;

#if defined(TRACE)
	videoSetModeSub(MODE_0_2D | DISPLAY_BG0_ACTIVE);
	SUB_BG0_CR = BG_MAP_BASE(31);
	BG_PALETTE_SUB[255] = RGB15(31,31,31);
	consoleInitDefault((u16*)SCREEN_BASE_BLOCK_SUB(31), (u16*)CHAR_BASE_BLOCK_SUB(0), 16);
#else	// defined(TRACE)
	videoSetModeSub(MODE_5_2D | DISPLAY_BG3_ACTIVE);
	SUB_BG3_CR = BG_BMP8_256x256;
	SUB_BG3_XDX = 1 << 8;
	SUB_BG3_XDY = 0;
	SUB_BG3_YDX = 0;
	SUB_BG3_YDY = 1 << 8;
	SUB_BG3_CX = 0;
	SUB_BG3_CY = 0 << 8;
	dmaCopy(g_sDemoBmp, BG_GFX_SUB, 256*192);
	dmaCopy(g_sDemoPal, BG_PALETTE_SUB, 256*2);
#endif	// defined(TRACE)

#if defined(SUPPORT_ROMEO2)
	ds_fpga_bus_init();
	const int nFpgaError = ds_fpga_configration(g_nds_fpga_raw, g_nds_fpga_raw_size);
	if (nFpgaError == FPGA_NO_ERR)
	{
		/* Enable OPM Sound */
		xmilcfg.SOUND_SW = 1;
		withRomeo2();
	}
#endif	// defined(SUPPORT_ROMEO2)

	softkbd9_initialize();

	extrom_initialize();

	sysmng_initialize();

	pccore_initialize();
	pccore_reset();

	diskimgset();

	xmilmain.uFrameCount = 0;
	xmilmain.uWaitCount = 0;
	xmilmain.uFrameMax = 1;

	sysmng_clockreset();

	CopyMemory(font_ank, __extromimage.ank, 0x800);

#if 1
	while(ndssys_task())
	{
		if (xmiloscfg.NOWAIT)
		{
			exec1frame(xmilmain);
			if (xmiloscfg.DRAW_SKIP)
			{
				// nowait frame skip
				if (xmilmain.uFrameCount >= xmiloscfg.DRAW_SKIP)
				{
					processwait(xmilmain, 0);
				}
			}
			else
			{
				// nowait auto skip
				if (timing_getcount())
				{
					processwait(xmilmain, 0);
				}
			}
		}
		else if (xmiloscfg.DRAW_SKIP)
		{
			// frame skip
			if (xmilmain.uFrameCount < xmiloscfg.DRAW_SKIP)
			{
				exec1frame(xmilmain);
			}
			else
			{
				processwait(xmilmain, xmiloscfg.DRAW_SKIP);
			}
		}
		else
		{
			// auto skip
			if (!xmilmain.uWaitCount)
			{
				exec1frame(xmilmain);
				const UINT uCount = timing_getcount();
				if (xmilmain.uFrameCount > uCount)
				{
					xmilmain.uWaitCount = xmilmain.uFrameCount;
					if (xmilmain.uFrameMax > 1)
					{
						xmilmain.uFrameMax--;
					}
				}
				else if (xmilmain.uFrameCount >= xmilmain.uFrameMax)
				{
					if (xmilmain.uFrameMax < MAX_FRAMESKIP)
					{
						xmilmain.uFrameMax++;
					}
					if (uCount >= MAX_FRAMESKIP)
					{
						timing_reset();
					}
					else
					{
						timing_setcount(uCount - xmilmain.uFrameCount);
					}
					framereset(xmilmain, 0);
				}
			}
			else
			{
				processwait(xmilmain, xmilmain.uWaitCount);
				xmilmain.uWaitCount = xmilmain.uFrameCount;
			}
		}
	}
#else
	while(ndssys_task())
	{
		if (xmilmain.uFrameCount < 2)
		{
			exec1frame(xmilmain);
		}
		else
		{
			processwait(xmilmain, 2);
		}
	}
#endif

	pccore_deinitialize();

	TRACETERM();

	return 0;
}

