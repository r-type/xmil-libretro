#include "compiler.h"
#include "libnds.h"
#include "pccore.h"
#include "iocore.h"
#include "vram.h"
#include "palettes.h"
#include "makescrn.h"
#include "makesub.h"
#include "vsyncff.h"


#define	SUPPORT_WIDTH80


	SCRNPOS		scrnpos;
	MAKESCRN	makescrn;


static void fillupdatetmp(void)
{
	for (UINT i=0; i<0x800; i++)
	{
		TRAMUPDATE(i) |= UPDATE_TVRAM;
	}
}

static void flashupdatetmp(void)
{
	BRESULT	y2;
	REG16	atr;
	REG16	udtbase;
	REG16	udt;

	if (!makescrn.vramsize)
	{
		return;
	}
	UINT posl = crtc.e.pos;
	UINT y = crtc.e.yl;
	do
	{
		UINT x;
		for (x=0; x<crtc.s.reg[CRTCREG_HDISP]; x++)
		{
			if (!(TRAM_ATR(LOW11(posl + x)) & TRAMATR_Yx2))
			{
				break;
			}
		}
		y2 = (x < crtc.s.reg[CRTCREG_HDISP])?FALSE:TRUE;
		udtbase = (x < crtc.s.reg[CRTCREG_HDISP])?0x0000:0x0404;
		UINT r = (crtc.s.reg[CRTCREG_HDISP] + 1) >> 1;
		do
		{
			UINT posr = LOW11(posl + 1);
			atr = (TRAM_ATR(posl) << 8) | TRAM_ATR(posr);
			udt = udtbase;
			if (!y2)
			{
				if (atr & (TRAMATR_Yx2 << 8))
				{
					udt |= (UPDATE_TRAM | 1) << 8;		// ¶’×‚êc”{Šp
				}
				else
				{
					y2 = TRUE;
				}
			}
			if (!y2)
			{
				if (atr & (TRAMATR_Yx2 << 0))
				{
					udt |= (UPDATE_TRAM | 1) << 0;		// ‰E’×‚êc”{Šp
				}
				else
				{
					y2 = TRUE;
				}
			}
			if (atr & (TRAMATR_Xx2 << 8))				// ¶‘¤”{Šp?
			{
				udt |= 0x0812;
			}
			if (atr & (TRAMATR_Xx2 << 0))				// ‰E‘¤”{Šp?
			{
				udt |= 0x0008;
			}
			if ((TRAMUPDATE(posl) ^ (udt >> 8)) & 0x1f)
			{
				TRAMUPDATE(posl) = (UINT8)((udt >> 8) | UPDATE_TRAM);
			}
			if ((TRAMUPDATE(posr) ^ (udt >> 0)) & 0x1f)
			{
				TRAMUPDATE(posr) = (UINT8)((udt >> 0) | UPDATE_TRAM);
			}
			posl = LOW11(posl + 2);
		} while(--r);
		if (crtc.s.reg[CRTCREG_HDISP] & 1)
		{
			posl = LOW11(posl - 1);
		}
	} while(--y);
}

static BRESULT updateblink(void)
{
	UINT uPos = makescrn.vramtop;
	makescrn.blinktest ^= 0x10;
	REG8 cUpdate = 0;
	UINT r = makescrn.vramsize;
	while(r)
	{
		r--;
		if (TRAM_ATR(uPos) & 0x10)
		{
			TRAMUPDATE(uPos) |= UPDATE_TRAM;
			cUpdate = UPDATE_TRAM;
		}
		uPos = LOW11(uPos + 1);
	}
	if (cUpdate)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


// ----

typedef void (LONG_CALL * MAKEFN)(UINT uPos);

static void LONG_CALL width_dummy(UINT uPos) { }

static const MAKEFN screenmake[] = {
				width40x25_200l,	width_dummy,
				width40x25_200l,	width40x25_200l,
				width40x12_200l,	width_dummy,
				width40x12_200l,	width40x12_200l,
				width_dummy,		width_dummy,
				width_dummy,		width_dummy,
				width_dummy,		width_dummy,
				width_dummy,		width_dummy,

				width80x25_200l,	width_dummy,
				width80x25_200l,	width80x25_200l,
				width80x12_200l,	width_dummy,
				width80x12_200l,	width80x12_200l,
				width_dummy,		width_dummy,
				width_dummy,		width_dummy,
				width_dummy,		width_dummy,
				width_dummy,		width_dummy,
};


static void changemodes(void)
{
	REG8 cDispMode = crtc.e.dispmode;
	makescrn.dispmode = cDispMode;
	makescrn.drawmode = cDispMode & DISPMODE_MASKMODE;
	makemix_mixstep((BRESULT)(cDispMode & 4));
#if defined(SUPPORT_WIDTH80)
	if (cDispMode & DISPMODE_WIDTH80)
	{
		makescrn.drawmode += 16;
		vsyncff_turn(1);
	}
	else
	{
		vsyncff_turn(0);
	}
#endif
	if (!(cDispMode & DISPMODE_BANK1))
	{
		makescrn.disp1 = gram + GRAM_BANK0;
		makescrn.disp2 = gram + GRAM_BANK1;
		makescrn.dispflag = UPDATE_TRAM + UPDATE_VRAM0;
	}
	else
	{
		makescrn.disp1 = gram + GRAM_BANK1;
		makescrn.disp2 = gram + GRAM_BANK0;
		makescrn.dispflag = UPDATE_TRAM + UPDATE_VRAM1;
	}
}

static void changecrtc(void)
{
	makescrn.vramtop = crtc.e.pos;

	UINT uScrnXMax;
	UINT uCharCx;
#if defined(SUPPORT_WIDTH80)
	if (makescrn.dispmode & DISPMODE_WIDTH80)
	{
		uScrnXMax = MAKESCRN_VIDEOWIDTH >> 2;
		uCharCx = 4;
	}
	else
	{
		uScrnXMax = MAKESCRN_VIDEOWIDTH >> 3;
		uCharCx = 8;
	}
#else
	uScrnXMax = MAKESCRN_VIDEOWIDTH >> 3;
	uCharCx = 8;
#endif
	UINT uScrnYMax = MAKESCRN_VIDEOHEIGHT;
	UINT uTextXl = crtc.s.reg[CRTCREG_HDISP];
	UINT uSurfCx = min(uScrnXMax, uTextXl);
	const UINT uPosX = uTextXl - uSurfCx;
#if 1
	scrnpos.x = uPosX / 2;
#else
	if (scrnpos.x > uPosX)
	{
		scrnpos.x = uPosX;
	}
#endif

	UINT uFontCy = crtc.e.fonty;
	UINT uUnderLines = (makescrn.dispmode & DISPMODE_UNDERLINE)?2:0;
	if (uFontCy > uUnderLines)
	{
		uFontCy -= uUnderLines;
	}
	else
	{
		uFontCy = 0;
	}
	const REG8 y2 = (makescrn.dispmode & DISPMODE_TEXTYx2)?1:0;
	uFontCy >>= y2;
	if (!uFontCy)
	{
		uFontCy = 1;
	}
	if (uFontCy > 8)
	{
		uFontCy = 8;
	}
	UINT uCharCy = uFontCy + uUnderLines;
	makescrn.fontcy = uFontCy;
	makescrn.charcy = uCharCy;
	uCharCy <<= y2;
	UINT uSurfCy = uScrnYMax / uCharCy;
	if (uSurfCy > crtc.e.yl)
	{
		uSurfCy = crtc.e.yl;
	}
	const UINT uPosY = crtc.e.yl - uSurfCy;
	if (scrnpos.y > uPosY)
	{
		scrnpos.y = uPosY;
	}

	makescrn.surfcx = uSurfCx;
	makescrn.surfrx = uTextXl - uSurfCx;
	makescrn.surfcy = uSurfCy;
	makescrn.surfstep = (MAKESCRN_VIDEOYALIGN * uCharCy) - (uSurfCx * uCharCx);
	makescrn.vramsize = min(0x800, uSurfCy * uTextXl);
}

void scrnupdate(void)
{
	REG8	flag;
	REG8	existblink;
	UINT	fontycnt;
	UINT	pos;
	UINT	y;
	REG8	udtmp;

	if (!corestat.drawframe)
	{
		return;
	}
	corestat.drawframe = 0;

	flag = 0;
	if (crtc.e.scrnflash)
	{
		crtc.e.scrnflash = 0;
		flag |= SCRNUPD_FLASH;
	}
	if (crtc.e.scrnallflash)
	{
		crtc.e.scrnallflash = 0;
		flag |= SCRNUPD_ALLFLASH;
	}
	if (crtc.e.palandply)
	{
		crtc.e.palandply = 0;
		flag |= SCRNUPD_PALANDPLY;
	}

	if (makescrn.dispmode != crtc.e.dispmode)
	{
		TRACEOUT(("change mode!"));
		changemodes();
		flag |= SCRNUPD_ALLFLASH | SCRNUPD_PALANDPLY;
	}
	if (flag & SCRNUPD_ALLFLASH)
	{
		changecrtc();
		fillupdatetmp();
	}
	if (crtc.e.remakeattr)
	{
		crtc.e.remakeattr = 0;
		flashupdatetmp();
	}
	if (flag & SCRNUPD_PALANDPLY)
	{
		pal_update();
	}

	if (crtc.e.blinktime)
	{
		crtc.e.blinktime--;
	}
	else
	{
		crtc.e.blinktime = 30 - 1;
		if (crtc.e.existblink)
		{
			existblink = updateblink();
			crtc.e.existblink = existblink;
			flag |= existblink;
		}
	}

	if (flag & (SCRNUPD_FLASH | SCRNUPD_ALLFLASH))
	{
		if (makescrn.vramsize)
		{
			fontycnt = 0;
			pos = makescrn.vramtop;
			y = scrnpos.y;
			if (y)
			{
				do
				{
					udtmp = TRAMUPDATE(pos);
					if (udtmp & 4)
					{
						fontycnt += makescrn.charcy;
					}
					else
					{
						fontycnt = makescrn.charcy * 2;
					}
					pos = LOW11(pos + crtc.s.reg[CRTCREG_HDISP]);
				} while(--y);
				fontycnt = fontycnt & 15;
			}
			makescrn.fontycnt = fontycnt;
			pos += scrnpos.x;
			screenmake[makescrn.drawmode](pos);
			ndsvideo_vramnotify();
		}
	}
}


void makescrn_initialize(void)
{
	CopyMemory(makescrn.patx2, patx2, 0x100);
	vsyncff_init();
}

void makescrn_reset(void)
{
	changemodes();
	changecrtc();
	flashupdatetmp();
}

