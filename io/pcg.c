#include	"compiler.h"
#include	"z80core.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"nevent.h"
#include	"vram.h"
#include	"font.h"


static UINT pcg_offset(void) {

	if (tram[TRAM_ATR + 0x07ff] & 0x20) {
		return(0x7ff);
	}
	if (tram[TRAM_ATR + 0x03ff] & 0x20) {
		return(0x3ff);
	}
	if (tram[TRAM_ATR + 0x05ff] & 0x20) {
		return(0x5ff);
	}
	if (tram[TRAM_ATR + 0x01ff] & 0x20) {
		return(0x1ff);
	}
	return(0x7ff);
}

static UINT knj_offset(void) {

	if (!(tram[TRAM_ATR + 0x07ff] & 0x20)) {
		return(0x7ff);
	}
	if (!(tram[TRAM_ATR + 0x03ff] & 0x20)) {
		return(0x3ff);
	}
	if (!(tram[TRAM_ATR + 0x05ff] & 0x20)) {
		return(0x5ff);
	}
	if (!(tram[TRAM_ATR + 0x01ff] & 0x20)) {
		return(0x1ff);
	}
	return(0x7ff);
}

static UINT nowsyncoffset(UINT *line) {

	SINT32	clock;
	UINT	h;
	UINT	v;
	UINT	ret;

	clock = nevent_getwork(NEVENT_FRAMES);
	if (corestat.vsync) {
		clock += corestat.dispclock;
	}
	clock = clock << 8;
	v = clock / crtc.e.rasterclock8;
	h = clock - (v * crtc.e.rasterclock8);
	if (crtc.s.SCRN_BITS & SCRN_24KHZ) {
		v = v >> 1;
	}
	ret = v / crtc.e.fonty;
	*line = (v - (ret * crtc.e.fonty)) & 7;
	ret = (ret * crtc.s.reg[CRTCREG_HDISP]) + crtc.e.pos;
	ret += (h * crtc.s.reg[CRTCREG_HDISP]) / crtc.e.rasterclock8;
	if (ret >= 0x0800) {
		ret = 0x07ff;		// オーバーフロー
	}
	return(ret);			// まさに表示しようとしている TXT-RAMアドレス
}


void IOOUTCALL pcg_o(UINT port, REG8 value) {

	UINT	chr;
	UINT	off;
	UINT	line;

	if (crtc.s.SCRN_BITS & SCRN_PCGMODE) {
		off = pcg_offset();
		chr = tram[TRAM_ANK + off];
		if (tram[TRAM_KNJ + off] & 0x90) {
			chr = chr & (~1);
			line = port & 15;
		}
		else {
			line = (port >> 1) & 7;
		}
	}
	else {
		off = nowsyncoffset(&line);
		chr = tram[TRAM_ANK + off];
	}
	chr += (port & 0x0300) - 0x100;
	pcg.d[(chr << 3) + line] = value;
}

REG8 IOINPCALL pcg_i(UINT port) {

	UINT	upper;
	UINT	line;
	UINT	off;
	UINT	chr;
	UINT	knj;
	UINT	addr;

	upper = port & 0x0300;
	if (crtc.s.SCRN_BITS & SCRN_PCGMODE) {
		line = port & 0x0f;
		if (!upper) {
			off = knj_offset();
			chr = tram[TRAM_ANK + off];
			knj = tram[TRAM_KNJ + off];
			if (knj & 0x80) {
				addr = ((((knj & 0x1f) << 8) + chr) << 4) + line;
				if (knj & 0x40) {
					addr += FONTX1T_LR;
				}
				return(font_knjx1t[addr]);
			}
			else if (crtc.s.SCRN_BITS & SCRN_CPUFONT) {
				return(font_txt[(chr << 4) + line]);
			}
			else {
				line >>= 1;
			}
		}
		else {
			off = pcg_offset();
			chr = tram[TRAM_ANK + off];
			if (tram[TRAM_KNJ + off] & 0x90) {
				chr = chr & (~1);
			}
			else {
				line >>= 1;
			}
		}
	}
	else {
		off = nowsyncoffset(&line);
		chr = tram[TRAM_ANK + off];
	}
	if (!upper) {
		return(font_ank[(chr << 3) + line]);
	}
	else {
		chr += upper - 0x100;
		return(pcg.d[(chr << 3) + line]);
	}
}


// ----

void pcg_initialize(void) {

	ZeroMemory(&pcg, sizeof(pcg));
}

void pcg_reset(void) {
}

