#include	"compiler.h"
#include	"pccore.h"
#include	"iocore.h"


	IOCORE		iocore;
	CGROM		cgrom;		//	x1k;
	CMT			cmt;
	CRTC		crtc;
	CTC			ctc;
	DMAC		dma;
	FDC			fdc;
	MEMIO		memio;
	PCG			pcg;
	PPI			ppi;
	SIO			sio;
	SNDBOARD	sndboard;
	SUBCPU		subcpu;


// ----

static void IOOUTCALL dummy_out(UINT port, REG8 dat) {

	(void)port;
	(void)dat;
}

static REG8 IOINPCALL dummy_inp(UINT port) {

	(void)port;
	return(0xff);
}


// ----

static void IOOUTCALL port1fxx_o(UINT port, REG8 dat) {

	REG8	lsb;
	REG8	msb6;

	lsb = (UINT8)port;
	if (lsb < 0x80) {
		return;
	}
	msb6 = lsb & (~3);
	if (pccore.ROM_TYPE >= 2) {
		if (lsb < 0x90) {
			dmac_o(port, dat);
			return;
		}
		if ((msb6 == 0xa0) || (msb6 == 0xa8)) {
			ctc_o(port, dat);
			return;
		}
		if (lsb == 0xd0) {
			scrn_o(port, dat);
			return;
		}
		if (lsb == 0xe0) {
			blackctrl_o(port, dat);
			return;
		}
	}
	if (pccore.ROM_TYPE >= 3) {
		if (lsb == 0xb0) {
			extpal_o(port, dat);
			return;
		}
		if (lsb == 0xc0) {
			exttextdisp_o(port, dat);
			return;
		}
		if (lsb == 0xc5) {
			extgrphpal_o(port, dat);
			return;
		}
		if ((lsb >= 0xb9) && (lsb < 0xc0)) {
			exttextpal_o(port, dat);
			return;
		}
	}
	if (msb6 == 0x90) {
		sio_o(port, dat);
		return;
	}
}

static REG8 IOINPCALL port1fxx_i(UINT port) {

	REG8	lsb;
	REG8	msb6;

	lsb = (UINT8)port;
	if (lsb < 0x80) {
		return(0xff);
	}
	msb6 = lsb & (~3);
	if (pccore.ROM_TYPE >= 2) {
		if (lsb < 0x90) {
			return(dmac_i(port));
		}
		if ((msb6 == 0xa0) || (msb6 == 0xa8)) {
			return(ctc_i(port));
		}
		if (lsb == 0xd0) {
			return(scrn_i(port));
		}
		if (lsb >= 0xf0) {
			return(dipsw_i(port));
		}
	}
	if (pccore.ROM_TYPE >= 3) {
		if (lsb == 0xb0) {
			return(extpal_i(port));
		}
		if (lsb == 0xc0) {
			return(exttextdisp_i(port));
		}
		if (lsb == 0xc5) {
			return(extgrphpal_i(port));
		}
		if ((lsb >= 0xb9) && (lsb < 0xc0)) {
			return(exttextpal_i(port));
		}
		if (lsb == 0xe0) {
			return(blackctrl_i(port));
		}
	}
	if (msb6 == 0x90) {
		return(sio_i(port));
	}
	return(0xff);
}


// ----

static const IOINP definp[0x20] = {
			dummy_inp,			dummy_inp,
			dummy_inp,			dummy_inp,
			dummy_inp,			dummy_inp,
			dummy_inp,			dummy_inp,

			dummy_inp,			dummy_inp,
			dummy_inp,			dummy_inp,
			dummy_inp,			dummy_inp,
			cgrom_i,			fdc_i,

			dummy_inp,			dummy_inp,
			dummy_inp,			dummy_inp,
			pcg_i,				pcg_i,
			pcg_i,				pcg_i,

			dummy_inp,			subcpu_i,
			ppi_i,				sndboard_psgsta,
			dummy_inp,			dummy_inp,
			dummy_inp,			port1fxx_i};

static const IOOUT defout[0x20] = {
			dummy_out,			dummy_out,
			dummy_out,			dummy_out,
			dummy_out,			dummy_out,
			dummy_out,			dummy_out,

			dummy_out,			dummy_out,
			dummy_out,			dummy_out,
			dummy_out,			dummy_out,
			cgrom_o,			fdc_o,

			palette_o,			palette_o,
			palette_o,			ply_o,
			dummy_out,			pcg_o,
			pcg_o,				pcg_o,

			crtc_o,				subcpu_o,
			ppi_o,				sndboard_psgdat,
			sndboard_psgreg,	memio_rom,
			memio_ram,			port1fxx_o};



// ----

void iocore_reset(void) {

	ZeroMemory(&iocore, sizeof(iocore));
	CopyMemory(iocore.e.inpfn, definp, sizeof(definp));
	CopyMemory(iocore.e.outfn, defout, sizeof(defout));
#if defined(SUPPORT_BANKMEM)
	if (pccore.ROM_TYPE >= 2) {
		iocore.e.inpfn[0x0b] = memio_bank_i;
		iocore.e.outfn[0x0b] = memio_bank_o;
	}
#endif
	if (pccore.ROM_TYPE >= 3) {
		iocore.e.inpfn[0x10] = palette_i;
		iocore.e.inpfn[0x11] = palette_i;
		iocore.e.inpfn[0x12] = palette_i;
		iocore.e.inpfn[0x13] = ply_i;
	}
	if (pccore.SOUND_SW) {
		iocore.e.inpfn[0x07] = opm_i;
		iocore.e.outfn[0x07] = opm_o;
	}
}


// ----

void IOOUTCALL iocore_out(UINT port, REG8 dat) {

	UINT	msb;

	msb = port >> 8;
	if (iocore.s.mode) {
		gram2_o(port, dat);
	}
	else if (msb >= 0x40) {
		gram_o(port, dat);
	}
	else if (msb >= 0x20) {
		tram_o(port, dat);
	}
	else {
		(*iocore.e.outfn[msb])(port, dat);
	}
}

REG8 IOINPCALL iocore_inp(UINT port) {

	UINT	msb;

	msb = port >> 8;
	iocore.s.mode = 0;
	if (msb >= 0x40) {
		return(gram_i(port));
	}
	else if (msb >= 0x20) {
		return(tram_i(port));
	}
	else {
		return((*iocore.e.inpfn[msb])(port));
	}
}
