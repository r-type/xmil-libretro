#include	"compiler.h"
#include	"scrnmng.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"vram.h"
#include	"palettes.h"
#include	"makescrn.h"


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

static const CRTCSTAT crtcdefault = {
				{0xaa, 0xcc, 0xf0, 0x00},	// rgbp
				0,							// SCRN_BITS
				0,							// CRTC_NUM

				7,							// _FNT_YL
				40,							// TXT_XL
				25,							// TXT_YL
				28,							// TXT_YS

//				200,						// CRT_YL
//				232,						// CRT_VS
//				266,						// CRT_VL

				31,							// TXT_VL
				10,							// TXT_VLA

				0,							// TXT_TOP
//				8,							// fnty

				0,							// lastpal

				0,							// BLACKPAL
				0,							// EXTPALMODE
				0,							// EXTGRPHPAL
				0,							// ZPRY
	};


void vrambank_patch(void) {

	UINT	updatemask;
	UINT8	dispmode;
	UINT8	pal_bank;
	UINT8	pal_disp;

	if (crtc.s.SCRN_BITS & 0x10) {
		crtc.e.gram = gram + GRAM_BANK1;
		crtc.e.updatebit = UPDATE_VRAM1;
	}
	else {
		crtc.e.gram = gram + GRAM_BANK0;
		crtc.e.updatebit = UPDATE_VRAM0;
	}
	dispmode = (crtc.s.SCRN_BITS & SCRN_DISPVRAM)?SCRN_BANK1:SCRN_BANK0;
	pal_bank = pal_disp = PAL_NORMAL;

	if ((!(crtc.s.EXTPALMODE & 0x80)) || (crtc.s.SCRN_BITS & SCRN_UNDERLINE)) {
		updatemask = 0x7ff;
		if ((crtc.s.SCRN_BITS & SCRN_24KHZ) && (crtc.s.TXT_XL == 80)) {
			pal_bank = pal_disp = PAL_HIGHRESO;
		}
		if (crtc.s.SCRN_BITS & SCRN_TEXTYx2) {
			updatemask = 0x3ff;
		}
	}
	else {													// Analog mode
		updatemask = 0x3ff;
		if (!(crtc.s.SCRN_BITS & SCRN_TEXTYx2)) {
			if (crtc.s.SCRN_BITS & SCRN_24KHZ) {
				if (crtc.s.TXT_XL == 40) {
					if (crtc.s.SCRN_BITS & SCRN_200LINE) {	// width 40,25,0,2
						dispmode |= SCRN64_320x200;
					}
					else {									// width 40,25,1,2
						dispmode = SCRN64_H320x400;
					}
				}
				else {										// width 80,25,?,2
					updatemask = 0x7ff;
					pal_bank = pal_disp = PAL_HIGHRESO;
				}
			}
			else {
				if (crtc.s.TXT_XL == 40) {					// width 40,25,0,1
					if (crtc.s.EXTPALMODE & 0x10) {
						if (crtc.s.ZPRY & 0x10) {
							dispmode = SCRN64_L320x200x2 |
								((crtc.s.ZPRY & 8) ? SCRN_BANK1 : SCRN_BANK0);
							pal_disp = PAL_4096 | PAL_64x2 |
								((crtc.s.ZPRY & 8) ? PAL_4096L : PAL_4096H);
						}
						else {
							dispmode |= SCRN64_320x200;
							pal_disp = PAL_4096 |
									((crtc.s.SCRN_BITS & SCRN_DISPVRAM)
													? PAL_4096L : PAL_4096H);
						}
						pal_bank = PAL_4096 |
									((crtc.s.SCRN_BITS & SCRN_ACCESSVRAM)
													? PAL_4096L : PAL_4096H);
					}
					else {
						dispmode = SCRN64_320x200x4096 | SCRN_DRAW4096;
						pal_bank = pal_disp = PAL_4096 | PAL_4096FULL;
					}
				}
				else {										// width 80,25,0,1
					updatemask = 0x7ff;
					dispmode = SCRN64_L640x200;
				}
			}
		}
		else {
			if (crtc.s.SCRN_BITS & SCRN_24KHZ) {
				if (crtc.s.TXT_XL == 40) {
					if (crtc.s.SCRN_BITS & SCRN_200LINE) {	// width 40,12,0,2
						dispmode |= SCRN64_320x100;
					}
					else {									// width 40,12,1,2
						dispmode = SCRN64_H320x200;
					}
				}
				else {										// width 80,12,?,2
					pal_bank = pal_disp = PAL_HIGHRESO;
				}
			}
			else {
				if (crtc.s.TXT_XL == 40) {					// width 40,12,0,1
					if (crtc.s.EXTPALMODE & 0x10) {
						if (crtc.s.ZPRY & 0x10) {
							dispmode = SCRN64_320x100x2 |
								((crtc.s.ZPRY & 8) ? SCRN_BANK1 : SCRN_BANK0);
							pal_disp = PAL_4096 | PAL_64x2 |
								((crtc.s.ZPRY & 8) ? PAL_4096L : PAL_4096H);
						}
						else {
							dispmode |= SCRN64_320x100;
							pal_disp = PAL_4096 |
									((crtc.s.SCRN_BITS & SCRN_DISPVRAM)
													? PAL_4096L : PAL_4096H);
						}
						pal_bank = PAL_4096 |
									((crtc.s.SCRN_BITS & SCRN_ACCESSVRAM)
													? PAL_4096L : PAL_4096H);
					}
					else {
						dispmode = SCRN64_320x100x4096 | SCRN_DRAW4096;
						pal_bank = pal_disp = PAL_4096 | PAL_4096FULL;
					}
				}
				else {										// width 80,12,0,1
					dispmode = SCRN64_L640x100;
				}
			}
		}
	}
	crtc.e.updatemask = updatemask;
	crtc.e.dispmode = dispmode;
	crtc.e.pal_bank = pal_bank;
	crtc.e.pal_disp = pal_disp;
}

static void crtc_updt(void) {

	UINT	fonty;

	fonty = crtc.s._FNT_YL;
	if (crtc.s.SCRN_BITS & SCRN_24KHZ) {
		fonty >>= 1;
	}
	fonty += 1;
	crtc.e.fonty = fonty;
	crtc.e.yl = (crtc.s.TXT_YL & 0x7f);

	crtc.e.dl = fonty * crtc.e.yl;
	crtc.e.vs = fonty * ((crtc.s.TXT_YS & 0x7f) + 1);
	crtc.e.vl = fonty * ((crtc.s.TXT_VL & 0x7f) + 1) + (crtc.s.TXT_VLA & 0x1f);
}


// ---- CRTC

void IOOUTCALL crtc_o(UINT port, REG8 value) {

	port &= 0xff;
	if (port == 0) {
		crtc.s.CRTC_NUM = value;
	}
	else if (port == 1) {
		switch(crtc.s.CRTC_NUM) {
			case 0x01:
				if (value <= 40) {
					crtc.s.TXT_XL = 40;
				}
				else {
					crtc.s.TXT_XL = 80;
				}
				vrambank_patch();
				break;

			case 0x04:
				crtc.s.TXT_VL = value;
				break;

			case 0x05:
				crtc.s.TXT_VLA = value;
				break;

			case 0x06:
				crtc.s.TXT_YL = value;
				break;

			case 0x07:
				crtc.s.TXT_YS = value;
				break;

			case 0x09:
				crtc.s._FNT_YL = value;
				break;

			case 0x0c:
				crtc.s.TXT_TOP &= 0xff;
				crtc.s.TXT_TOP |= (value & 7) << 8;
				makescrn.remakeattr = 1;
				break;

			case 0x0d:
				crtc.s.TXT_TOP &= 0x700;
				crtc.s.TXT_TOP |= value;
				makescrn.remakeattr = 1;
				break;

			default:
				return;
		}
//		crtc.s.GRP_XL = crtc.s.TXT_XL << 3;
//		crtc.s.GRP_YL = 200;
		crtc_updt();
		scrnallflash = 1;								/* 990220 puni */
	}
}


// ---- スクリーンモード

void IOOUTCALL scrn_o(UINT port, REG8 value) {

	REG8	modify;

	modify = crtc.s.SCRN_BITS ^ value;
	crtc.s.SCRN_BITS = value;
	if (modify & SCRN_DISPCHANGE) {
//		pal_reset();					// なんで？
		scrnallflash = 1;
		makescrn.palandply = 1;
		crtc_updt();
	}
	vrambank_patch();
	(void)port;
}

REG8 IOINPCALL scrn_i(UINT port) {

	(void)port;
	return(crtc.s.SCRN_BITS);
}


// ---- プライオリティ

void IOOUTCALL ply_o(UINT port, REG8 value) {

	if (crtc.s.rgbp[CRTC_PLY] != value) {
		crtc.s.rgbp[CRTC_PLY] = value;
		makescrn.palandply = 1;
	}
	(void)port;
}

REG8 IOINPCALL ply_i(UINT port) {

	(void)port;
	return(crtc.s.rgbp[CRTC_PLY]);
}


// ---- パレット

void IOOUTCALL palette_o(UINT port, REG8 value) {

	REG8	sft;
	UINT	num;
	UINT	pal;

	crtc.s.lastpal = (value & 0xf0);
	if (crtc.s.EXTPALMODE & 0x80) {
		if ((crtc.s.EXTGRPHPAL & 0x88) != 0x80) {
			return;
		}
		sft = (port >> (8 - 2)) & (3 << 2);
		if (crtc.e.pal_bank & PAL_4096) {				// use 4096palettes
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
			if ((crtc.e.dispmode & SCRN64_MASK) == SCRN64_INVALID) {
				num |= (num << 3);
			}
			else {
				num += ((port & 0x40) >> 1) | ((port & 0x04) << 2) |
													((value & 0x40) >> 3);
			}
			pal = crtc.p.grph[crtc.e.pal_bank][num];
			pal &= ~((0x0f) << sft);
			pal |= (value & 0x0f) << sft;
			crtc.p.grph[crtc.e.pal_bank][num] = pal;
			pal_setgrph(crtc.e.pal_bank, (REG8)num);
		}
	}
	else {
		num = (port >> 8) & 3;
		if (crtc.s.rgbp[num] != value) {
			crtc.s.rgbp[num] = value;
			makescrn.palandply = 1;
		}
	}
}

REG8 IOINPCALL palette_i(UINT port) {

	UINT	num;
	UINT	pal;
	REG8	sft;

	if ((crtc.s.EXTPALMODE & 0x80) && ((crtc.s.EXTGRPHPAL & 0x88) == 0x88)) {
		if (crtc.e.pal_bank & PAL_4096) {				// use 4096palettes
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
			if ((crtc.e.dispmode & SCRN64_MASK) == SCRN64_INVALID) {
				num |= (num << 3);
			}
			else {
				num += ((port & 0x40) >> 1) | ((port & 0x04) << 2) |
											((crtc.s.lastpal & 0x40) >> 3);
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


// ---- turboZ

void IOOUTCALL extpal_o(UINT port, REG8 value) {

	crtc.s.EXTPALMODE = value;
	vrambank_patch();
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
	vrambank_patch();
	(void)port;
}

REG8 IOINPCALL exttextdisp_i(UINT port) {

	if (crtc.s.EXTPALMODE & 0x80) {
		return(crtc.s.ZPRY);
	}
	(void)port;
	return(0xff);
}

void IOOUTCALL blackctrl_o(UINT port, REG8 value) {

	crtc.s.BLACKPAL = value;
	makescrn.palandply = 1;
	(void)port;
}

REG8 IOINPCALL blackctrl_i(UINT port) {

	(void)port;
	return(crtc.s.BLACKPAL);
}


// ----

static void resetpal(void) {

	CopyMemory(crtc.p.text, defpaltext, sizeof(defpaltext));
	CopyMemory(crtc.p.grph[0], defpalgrph, sizeof(defpalgrph));
	CopyMemory(crtc.p.grph[1], defpalgrph, sizeof(defpalgrph));
}

void crtc_initialize(void) {

	UINT	p;

	resetpal();
	for (p=0; p<4096; p++) {
		crtc.p.grph4096[p] = p;
	}
}

void crtc_reset(void) {

	crtc.s = crtcdefault;
	if (pccore.ROM_TYPE < 3) {
		resetpal();
	}
	if ((pccore.ROM_TYPE >= 2) && (!(pccore.DIP_SW & 1))) {
		crtc.s.SCRN_BITS = SCRN_200LINE;
		crtc.s._FNT_YL = 15;
	}

	pal_reset();
	crtc_updt();

	vrambank_patch();
	makescrn.palandply = 1;
	scrnallflash = 1;
}

