#include	"compiler.h"
#include	"z80core.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"vram.h"
#include	"font.h"


static void pcgsync(void) {

	*(UINT32 *)&pcg.r.vsync = 0;
	*(UINT32 *)&pcg.r.roff0 = 0;
}

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

static UINT nowsyncoffset(void) {

	UINT	ret;

	ret = (((v_cnt - crtc.s.CRT_YL) / crtc.s.fnty) + crtc.s.TXT_YL)
											* crtc.s.TXT_XL + crtc.s.TXT_TOP;
	if (pccore.HSYNC_CLK) {
		ret += (h_cnt * crtc.s.TXT_XL) / pccore.HSYNC_CLK;
	}

	if (ret >= 0x0800) {
		ret = 0x07ff;		// オーバーフロー
	}
	return(ret);			// まさに表示しようとしている TXT-RAMアドレス
}


void IOOUTCALL pcg_o(UINT port, REG8 value) {

	UINT	chr;
	UINT	off;
	UINT	l;

	if (crtc.s.SCRN_BITS & SCRN_PCGMODE) {
		off = pcg_offset();
		chr = tram[TRAM_ANK + off];
		if (tram[TRAM_KNJ + off] & 0x90) {
			chr &= 0xfe;
			l = port & 15;
		}
		else {
			l = (port >> 1) & 7;
		}
		switch(port >> 8) {
			case 0x15:
				pcg.d.b[chr][l] = value;
				break;

			case 0x16:
				pcg.d.r[chr][l] = value;
				break;

			case 0x17:
				pcg.d.g[chr][l] = value;
				break;
		}
	}
	else {
		off = nowsyncoffset();						// 990622 puni
		chr = tram[TRAM_ANK + off];
		if (pcg.r.vsync) {
			pcgsync();
		}
		switch(port & 0xff00) {
			case 0x1500:
				pcg.d.b[chr][pcg.r.woff1] = value;
				pcg.r.woff1 = (pcg.r.woff1 + 1) & 7;
				break;

			case 0x1600:
				pcg.d.r[chr][pcg.r.woff2] = value;
				pcg.r.woff2 = (pcg.r.woff2 + 1) & 7;
				break;

			case 0x1700:
				pcg.d.g[chr][pcg.r.woff4] = value;
				pcg.r.woff4 = (pcg.r.woff4 + 1) & 7;
				break;
		}
	}
}

REG8 IOINPCALL pcg_i(UINT port) {

	UINT	off;
	int		l;
	BYTE	chr,knj,val;

	val = 0xff;
	if (crtc.s.SCRN_BITS & SCRN_PCGMODE) {
		l = port & 0x0f;
		if ((port & 0xff00) == 0x1400) {
			off = knj_offset();
			chr = tram[TRAM_ANK + off];
			knj = tram[TRAM_KNJ + off];
			if (knj & 0x80) {
				DWORD p;
				p = ((((DWORD)knj << 8) | chr) & 0x1fff) << 4;
				if (knj & 0x40) {
					val = font_knjx1t[p + l + FONTX1T_LR];
				}
				else {
					val = font_knjx1t[p + l];
				}
			}
			else if (crtc.s.SCRN_BITS & SCRN_CPUFONT) {
				val = font_txt[(chr << 4) + l];
			}
			else {
				val = font_ank[(chr << 3) + (l >> 1)];
			}
		}
		else {
			off = pcg_offset();
			chr = tram[TRAM_ANK + off];
			if (tram[TRAM_KNJ + off] & 0x90) {
				chr &= 0xfe;
			}
			else {
				l >>= 1;
			}
			switch(port & 0xff00) {
				case 0x1500:
					val = pcg.d.b[chr][l];
					break;

				case 0x1600:
					val = pcg.d.r[chr][l];
					break;

				case 0x1700:
					val = pcg.d.g[chr][l];
					break;
			}
		}
	}
	else {
		off = nowsyncoffset();					// 990622 puni
		chr = tram[TRAM_ANK + off];
		if (pcg.r.vsync) {
			pcgsync();
		}
		switch(port >> 8) {
			case 0x14:
				val = font_ank[(chr << 3) + pcg.r.roff0];
				pcg.r.roff0 = (pcg.r.roff0 + 1) & 7;
				break;

			case 0x15:
				val = pcg.d.b[chr][pcg.r.roff1];
				pcg.r.roff1 = (pcg.r.roff1 + 1) & 7;
				break;

			case 0x16:
				val = pcg.d.r[chr][pcg.r.roff2];
				pcg.r.roff2 = (pcg.r.roff2 + 1) & 7;
				break;

			case 0x17:
				val = pcg.d.g[chr][pcg.r.roff4];
				pcg.r.roff4 = (pcg.r.roff4 + 1) & 7;
				break;
		}
	}
	return(val);
}


// ----

void pcg_initialize(void) {

	ZeroMemory(&pcg, sizeof(pcg));
}

void pcg_reset(void) {

	pcgsync();
}

