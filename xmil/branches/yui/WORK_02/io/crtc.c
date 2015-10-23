#include	"compiler.h"
#include	"z80core.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"nevent.h"
#include	"vram.h"
#include	"palettes.h"
#include	"makescrn.h"


static const UINT8 defrgbp[4] = {0xaa, 0xcc, 0xf0, 0x00};
static const UINT8 defreg[18] = {
				0x37, 0x28, 0x2d, 0x34,
				0x1f, 0x02, 0x19, 0x1c,
				0x00, 0x07, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00,
				0x00, 0x00};

#if defined(SUPPORT_TURBOZ)
static const UINT8 defpaltext[8] = {
				0x00, 0x03, 0x0c, 0x0f, 0x30, 0x33, 0x3c, 0x3f};

static const UINT16 defpalgrph[64] = {
				0x000, 0x00a, 0x0a0, 0x0aa, 0xa00, 0xa0a, 0xaa0, 0xaaa,
				0x005, 0x00f, 0x0a5, 0x0af, 0xa05, 0xa0f, 0xaa5, 0xaaf,
				0x050, 0x05a, 0x0f0, 0x0fa, 0xa50, 0xa5a, 0xaf0, 0xafa,
				0x055, 0x05f, 0x0f5, 0x0ff, 0xa55, 0xa5f, 0xaf5, 0xaff,
				0x500, 0x50a, 0x5a0, 0x5aa, 0xf00, 0xf0a, 0xfa0, 0xfaa,
				0x505, 0x50f, 0x5a5, 0x5af, 0xf05, 0xf0f, 0xfa5, 0xfaf,
				0x550, 0x55a, 0x5f0, 0x5fa, 0xf50, 0xf5a, 0xff0, 0xffa,
				0x555, 0x55f, 0x5f5, 0x5ff, 0xf55, 0xf5f, 0xff5, 0xfff};
#endif


typedef struct {
	UINT32	clock;
	UINT	minx;
	UINT	maxx;
	UINT	miny;
	UINT	maxy;
} GDCCLK;

static const GDCCLK gdcclk[] = {
			{14318180 / 8, 112 - 8, 112 + 8, 200, 300},
			{21052600 / 8, 108 - 6, 108 + 6, 400, 575}};


static void crtc_bankupdate(void) {

	if (!(crtc.s.SCRN_BITS & SCRN_ACCESSVRAM)) {
		crtc.e.gramacc = gram + GRAM_BANK0;
		crtc.e.updatebit = UPDATE_VRAM0;
	}
	else {
		crtc.e.gramacc = gram + GRAM_BANK1;
		crtc.e.updatebit = UPDATE_VRAM1;
	}
}

static void crtc_dispupdate(void) {

	REG8	scrnpage;
	REG8	dispmode;
#if defined(SUPPORT_TURBOZ)
	UINT	updatemask;
	REG8	pal_bank;
	REG8	pal_disp;
#endif

	scrnpage = (crtc.s.SCRN_BITS & SCRN_DISPVRAM)?SCRN_BANK1:SCRN_BANK0;
	if (!crtc.s.width40) {
		scrnpage += SCRN_WIDTH80;
	}
	dispmode = scrnpage;
	dispmode += (crtc.s.SCRN_BITS & SCRN_UNDERLINE) >> 4;
	dispmode += (crtc.s.SCRN_BITS & 7);

#if !defined(SUPPORT_TURBOZ)
	crtc.e.updatemask = (crtc.s.SCRN_BITS & SCRN_TEXTYx2)?0x3ff:0x7ff;
	crtc.e.dispmode = dispmode;
#else
	pal_bank = PAL_NORMAL;
	pal_disp = PAL_NORMAL;
	if ((!(crtc.s.EXTPALMODE & 0x80)) || (crtc.s.SCRN_BITS & SCRN_UNDERLINE)) {
		updatemask = 0x7ff;
		if ((crtc.s.SCRN_BITS & SCRN_24KHZ) && (!crtc.s.width40)) {
			pal_bank = PAL_HIGHRESO;
			pal_disp = PAL_HIGHRESO;
		}
		if (crtc.s.SCRN_BITS & SCRN_TEXTYx2) {
			updatemask = 0x3ff;
		}
	}
	else {												/* Analog mode */
		updatemask = 0x3ff;
		if (!(crtc.s.SCRN_BITS & SCRN_TEXTYx2)) {
			if (crtc.s.SCRN_BITS & SCRN_24KHZ) {
				if (crtc.s.width40) {
					if (crtc.s.SCRN_BITS & SCRN_200LINE) {
														/* width 40,25,0,2 */
						dispmode = scrnpage + SCRN64_320x200;
					}
					else {								/* width 40,25,1,2 */
						dispmode = SCRN64_H320x400;
					}
				}
				else {									/* width 80,25,?,2 */
					updatemask = 0x7ff;
					pal_bank = PAL_HIGHRESO;
					pal_disp = PAL_HIGHRESO;
				}
			}
			else {
				if (crtc.s.width40) {					/* width 40,25,0,1 */
					if (crtc.s.EXTPALMODE & 0x10) {
						if (crtc.s.ZPRY & 0x10) {
							dispmode = SCRN64_L320x200x2 +
									((crtc.s.ZPRY & 8)?SCRN_BANK1:SCRN_BANK0);
							pal_disp = PAL_4096 + PAL_64x2 +
									((crtc.s.ZPRY & 8)?PAL_4096L:PAL_4096H);
						}
						else {
							dispmode = scrnpage + SCRN64_320x200;
							pal_disp = PAL_4096 +
										((crtc.s.SCRN_BITS & SCRN_DISPVRAM)
														?PAL_4096L:PAL_4096H);
						}
						pal_bank = PAL_4096 +
										((crtc.s.SCRN_BITS & SCRN_ACCESSVRAM)
														?PAL_4096L:PAL_4096H);
					}
					else {
						dispmode = SCRN_DRAW4096 + SCRN64_320x200x4096;
						pal_bank = PAL_4096 + PAL_4096FULL;
						pal_disp = PAL_4096 + PAL_4096FULL;
					}
				}
				else {									/* width 80,25,0,1 */
					updatemask = 0x7ff;
					dispmode = SCRN_WIDTH80 + SCRN64_L640x200;
				}
			}
		}
		else {
			if (crtc.s.SCRN_BITS & SCRN_24KHZ) {
				if (crtc.s.width40) {
					if (crtc.s.SCRN_BITS & SCRN_200LINE) {
														/* width 40,12,0,2 */
						dispmode = scrnpage + SCRN64_320x100;
					}
					else {								/* width 40,12,1,2 */
						dispmode = SCRN64_H320x200;
					}
				}
				else {									/* width 80,12,?,2 */
					pal_bank = PAL_HIGHRESO;
					pal_disp = PAL_HIGHRESO;
				}
			}
			else {
				if (crtc.s.width40) {					/* width 40,12,0,1 */
					if (crtc.s.EXTPALMODE & 0x10) {
						if (crtc.s.ZPRY & 0x10) {
							dispmode = SCRN64_320x100x2 +
									((crtc.s.ZPRY & 8)?SCRN_BANK1:SCRN_BANK0);
							pal_disp = PAL_4096 + PAL_64x2 +
									((crtc.s.ZPRY & 8)?PAL_4096L:PAL_4096H);
						}
						else {
							dispmode = scrnpage + SCRN64_320x100;
							pal_disp = PAL_4096 +
										((crtc.s.SCRN_BITS & SCRN_DISPVRAM)
														?PAL_4096L:PAL_4096H);
						}
						pal_bank = PAL_4096 +
										((crtc.s.SCRN_BITS & SCRN_ACCESSVRAM)
														?PAL_4096L:PAL_4096H);
					}
					else {
						dispmode = SCRN_DRAW4096 + SCRN64_320x100x4096;
						pal_bank = PAL_4096 + PAL_4096FULL;
						pal_disp = PAL_4096 + PAL_4096FULL;
					}
				}
				else {									/* width 80,12,0,1 */
					dispmode = SCRN_WIDTH80 + SCRN64_L640x100;
				}
			}
		}
	}
	crtc.e.updatemask = updatemask;
	crtc.e.dispmode = dispmode;
	crtc.e.pal_bank = pal_bank;
	crtc.e.pal_disp = pal_disp;
#endif
}

static void crtc_clkupdate(void) {

	UINT		clksync;
const GDCCLK	*clk;
	UINT		h;
	UINT		fonty;
	UINT		v;

	clksync = (crtc.s.SCRN_BITS & SCRN_24KHZ);
	clk = gdcclk + clksync;
	h = crtc.s.reg[CRTCREG_HSIZE] + 1;
	if (crtc.s.width40) {
		h = h * 2;
	}
	if (h < clk->minx) {
		h = clk->minx;
	}
	else if (h > clk->maxx) {
		h = clk->maxx;
	}
	crtc.e.rasterclock8 = (4000000 * h) / (clk->clock >> 8);

	fonty = crtc.s.reg[CRTCREG_CHRCY] & 0x1f;
	fonty >>= clksync;
	fonty += 1;
	crtc.e.fonty = fonty;

	v = (crtc.s.reg[CRTCREG_CHRCY] & 0x1f) + 1;
	v = v * ((crtc.s.reg[CRTCREG_VSIZE] & 0x7f) + 1);
	v = v + (crtc.s.reg[CRTCREG_VSIZEA] & 0x1f);
	if (v < clk->miny) {
		v = clk->miny;
	}
	else if (v > clk->maxy) {
		v = clk->maxy;
	}
	crtc.e.frameclock = ((4000000 * h) / clk->clock) * v;
}

static void crtc_timingupdate(void) {

	UINT32	fontclock;
	UINT	yl;
	SINT32	vsyncstart;

	/* とりあえず…ね */
	crtc.e.pos = crtc.s.reg[CRTCREG_POSL]
									+ ((crtc.s.reg[CRTCREG_POSH] & 7) << 8);

	crtc.e.rasterdisp8 = (crtc.e.rasterclock8 * 40) / 56;

	fontclock = (crtc.s.reg[CRTCREG_CHRCY] & 0x1f) + 1;
	fontclock = (fontclock * crtc.e.rasterclock8) >> 8;

	/* YsIIIが yl==0で disp信号見る…なんで？ */
	yl = (crtc.s.reg[CRTCREG_VDISP] & 0x7f);
	crtc.e.yl = yl;
	iocore.e.dispclock = fontclock * max(yl, 1);
	vsyncstart = fontclock * ((crtc.s.reg[CRTCREG_VSYNC] & 0x7f) + 1);
	iocore.e.vsyncstart = vsyncstart;
#if !defined(MAINFRAMES_OLD)
	iocore.e.vsyncend = vsyncstart + (((crtc.s.reg[CRTCREG_PULSE] >> 4)
												* crtc.e.rasterclock8) >> 8);
	neitem_mainframes(NEVENT_FRAMES);
#else
	iocore.e.vpulseclock = ((crtc.s.reg[CRTCREG_PULSE] >> 4)
												* crtc.e.rasterclock8) >> 8;
#endif
}


/* CRTC */

void IOOUTCALL crtc_o(UINT port, REG8 value) {

	port &= 0xff;
	if (port == 0) {
		crtc.s.regnum = value;
	}
	else if (port == 1) {
		if (crtc.s.regnum < CRTCREG_MAX) {
			if (crtc.s.reg[crtc.s.regnum] != value) {
				crtc.s.reg[crtc.s.regnum] = value;
				crtc_clkupdate();
				crtc_timingupdate();
				crtc.e.remakeattr = 1;
				crtc.e.scrnallflash = 1;
			}
		}
	}
}


/* スクリーンモード */

void IOOUTCALL scrn_o(UINT port, REG8 value) {

	REG8	modify;

	modify = crtc.s.SCRN_BITS ^ value;
	crtc.s.SCRN_BITS = value;
	if (modify & SCRN_ACCESSVRAM) {
		crtc_bankupdate();
	}
	if (modify & SCRN_DISPCHANGE) {
		crtc.e.scrnallflash = 1;
		crtc_dispupdate();
		crtc_clkupdate();
		crtc_timingupdate();
		crtc.e.palandply = 1;
	}
	(void)port;
}

REG8 IOINPCALL scrn_i(UINT port) {

	(void)port;
	return(crtc.s.SCRN_BITS);
}


/* プライオリティ */

void IOOUTCALL ply_o(UINT port, REG8 value) {

	if (crtc.s.rgbp[CRTC_PLY] != value) {
		crtc.s.rgbp[CRTC_PLY] = value;
		crtc.e.palandply = 1;
#if defined(SUPPORT_PALEVENT)
		if ((!corestat.vsync) && (palevent.events < SUPPORT_PALEVENT)) {
			PAL1EVENT *e = palevent.event + palevent.events;
			palevent.events++;
			e->rgbp = CRTC_PLY;
			e->value = value;
			e->clock = nevent_getwork(NEVENT_FRAMES);
		}
#endif
	}
	(void)port;
}


/* パレット */

void IOOUTCALL palette_o(UINT port, REG8 value) {

	UINT	num;
#if defined(SUPPORT_TURBOZ)
	REG8	sft;
	UINT	pal;
#endif

#if defined(SUPPORT_TURBOZ)
	crtc.s.lastpal = (value & 0xf0);
	if (!(crtc.s.EXTPALMODE & 0x80)) {
#endif
		num = (port >> 8) & 3;
		if (crtc.s.rgbp[num] != value) {
			crtc.s.rgbp[num] = value;
			crtc.e.palandply = 1;
#if defined(SUPPORT_PALEVENT)
			if ((!corestat.vsync) && (palevent.events < SUPPORT_PALEVENT)) {
				PAL1EVENT *e = palevent.event + palevent.events;
				palevent.events++;
				e->rgbp = (UINT8)num;
				e->value = value;
				e->clock = nevent_getwork(NEVENT_FRAMES);
			}
#endif
		}
#if defined(SUPPORT_TURBOZ)
	}
	else {
		if ((crtc.s.EXTGRPHPAL & 0x88) != 0x80) {
			return;
		}
		sft = (port >> (8 - 2)) & (3 << 2);
		if (crtc.e.pal_bank & PAL_4096) {				/* use 4096palettes */
			num = ((port & 0xff) << 4) + (crtc.s.lastpal >> 4);
			if (!(crtc.e.pal_bank & PAL_4096FULL)) {
				num &= 0xccc;
				if (crtc.e.pal_bank & PAL_4096BANK) {
					num >>= 2;
				}
			}
			pal = crtc.p.grph4096[num];
			pal &= ~((0x0f) << sft);
			pal |= (value & 0x0f) << sft;
			crtc.p.grph4096[num] = pal;
			pal_setgrph4096(num);
		}
		else {
			num = ((port & 0x80) >> 5) | ((port & 0x08) >> 2) |
												((value & 0x80) >> 7);
			if (crtc.e.dispmode & SCRN64_ENABLE) {
				num += ((port & 0x40) >> 1) | ((port & 0x04) << 2) |
													((value & 0x40) >> 3);
			}
			else {
				num |= (num << 3);
			}
			pal = crtc.p.grph[crtc.e.pal_bank][num];
			pal &= ~((0x0f) << sft);
			pal |= (value & 0x0f) << sft;
			crtc.p.grph[crtc.e.pal_bank][num] = pal;
			pal_setgrph(crtc.e.pal_bank, (REG8)num);
		}
	}
#endif
}

void IOOUTCALL blackctrl_o(UINT port, REG8 value) {

	if (crtc.s.rgbp[CRTC_BLACK] != value) {
		crtc.s.rgbp[CRTC_BLACK] = value;
		crtc.e.palandply = 1;
#if defined(SUPPORT_PALEVENT)
		if ((!corestat.vsync) && (palevent.events < SUPPORT_PALEVENT)) {
			PAL1EVENT *e = palevent.event + palevent.events;
			palevent.events++;
			e->rgbp = CRTC_BLACK;
			e->value = value;
			e->clock = nevent_getwork(NEVENT_FRAMES);
		}
#endif
	}
	(void)port;
}

REG8 IOINPCALL blackctrl_i(UINT port) {

	(void)port;
	return(crtc.s.rgbp[CRTC_BLACK]);
}


/* turboZ */

#if defined(SUPPORT_TURBOZ)
REG8 IOINPCALL ply_i(UINT port) {

	(void)port;
	return(crtc.s.rgbp[CRTC_PLY]);
}

REG8 IOINPCALL palette_i(UINT port) {

	UINT	num;
	UINT	pal;
	REG8	sft;

	if ((crtc.s.EXTPALMODE & 0x80) && ((crtc.s.EXTGRPHPAL & 0x88) == 0x88)) {
		if (crtc.e.pal_bank & PAL_4096) {				/* use 4096palettes */
			num = ((port & 0xff) << 4) + (crtc.s.lastpal >> 4);
			if (!(crtc.e.pal_bank & PAL_4096FULL)) {
				num &= 0xccc;
				if (crtc.e.pal_bank & PAL_4096BANK) {
					num >>= 2;
				}
			}
			pal = crtc.p.grph4096[num];
		}
		else {
			num = ((port & 0x80) >> 5) | ((port & 0x08) >> 2) |
											((crtc.s.lastpal & 0x80) >> 7);
			if (crtc.e.dispmode & SCRN64_ENABLE) {
				num += ((port & 0x40) >> 1) | ((port & 0x04) << 2) |
											((crtc.s.lastpal & 0x40) >> 3);
			}
			else {
				num |= (num << 3);
			}
			pal = crtc.p.grph[crtc.e.pal_bank][num];
		}
		sft = (port >> (8 - 2)) & (3 << 2);
		return((REG8)(crtc.s.lastpal + ((pal >> sft) & 0xf)));
	}
	else {
		return(0xff);
	}
}


void IOOUTCALL extpal_o(UINT port, REG8 value) {

	crtc.s.EXTPALMODE = value;
	crtc_dispupdate();
	(void)port;
}

REG8 IOINPCALL extpal_i(UINT port) {

	(void)port;
	return(crtc.s.EXTPALMODE);
}

void IOOUTCALL extgrphpal_o(UINT port, REG8 value) {

	if (crtc.s.EXTPALMODE & 0x80) {
		crtc.s.EXTGRPHPAL = value;
	}
	(void)port;
}

REG8 IOINPCALL extgrphpal_i(UINT port) {

	if (crtc.s.EXTPALMODE & 0x80) {
		return(crtc.s.EXTGRPHPAL);
	}
	(void)port;
	return(0xff);
}

void IOOUTCALL exttextpal_o(UINT port, REG8 value) {

	if (crtc.s.EXTPALMODE & 0x80) {
		crtc.p.text[port & 7] = value;
		pal_settext((REG8)(port & 7));
	}
}

REG8 IOINPCALL exttextpal_i(UINT port) {

	if (crtc.s.EXTPALMODE & 0x80) {
		return(crtc.p.text[port & 7]);
	}
	return(0xff);
}

void IOOUTCALL exttextdisp_o(UINT port, REG8 value) {

	crtc.s.ZPRY = value;
	crtc_dispupdate();
	(void)port;
}

REG8 IOINPCALL exttextdisp_i(UINT port) {

	if (crtc.s.EXTPALMODE & 0x80) {
		return(crtc.s.ZPRY);
	}
	(void)port;
	return(0xff);
}
#endif


/* extension */

void crtc_update(void) {

	crtc_bankupdate();
	crtc_dispupdate();
	crtc_clkupdate();
	crtc_timingupdate();
	crtc.e.palandply = 1;
	crtc.e.scrnallflash = 1;
}

void crtc_setwidth(REG8 width40) {

	crtc.s.width40 = width40;
	crtc_update();
}


/* init/reset */

#if defined(SUPPORT_TURBOZ)
static void resetpal(void) {

	CopyMemory(crtc.p.text, defpaltext, sizeof(defpaltext));
	CopyMemory(crtc.p.grph[0], defpalgrph, sizeof(defpalgrph));
	CopyMemory(crtc.p.grph[1], defpalgrph, sizeof(defpalgrph));
}
#endif

#if defined(SUPPORT_TURBOZ)
void crtc_initialize(void) {

	UINT	p;

	resetpal();
	for (p=0; p<4096; p++) {
		crtc.p.grph4096[p] = p;
	}
}
#endif

void crtc_reset(void) {

	ZeroMemory(&crtc, sizeof(crtc));
	CopyMemory(crtc.s.rgbp, defrgbp, 4);
	CopyMemory(crtc.s.reg, defreg, 18);
	crtc.s.width40 = 1;
#if defined(SUPPORT_TURBOZ)
	if (pccore.ROM_TYPE < 3) {
		resetpal();
	}
#endif
#if 0
	/* IPLが勝手に切り替える筈である */
	if ((pccore.ROM_TYPE >= 2) && (!(pccore.DIP_SW & 1))) {
		crtc.s.SCRN_BITS = SCRN_200LINE;
		crtc.s.reg[CRTCREG_CHRCY] = 15;
	}
#endif	/* 0 */

	pal_reset();
	crtc.e.palandply = 1;
	crtc_update();
}

void crtc_forcesetwidth(REG8 width) {

	crtc.s.reg[CRTCREG_HDISP] = (UINT8)width;
	crtc_dispupdate();
	crtc.e.scrnallflash = 1;
}

