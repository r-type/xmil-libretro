#include	"compiler.h"
#include	"joymng.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"sndctrl.h"
#include	"keystat.h"


void IOOUTCALL sndboard_opmreg(UINT port, REG8 value) {

//	TRACEOUT(("%.4x, %.2x", port, value));
	sndboard.opmreg = (UINT8)value;
}

void IOOUTCALL sndboard_opmdat(UINT port, REG8 value) {

//	TRACEOUT(("%.4x, %.2x", port, value));
	opmgen_setreg(sndboard.opmreg, value);
}

REG8 IOINPCALL sndboard_opmsta(UINT port) {

	if (pccore.SOUND_SW) {
		return(0x00);								// É[ÉäÉAÅ[ÉhëŒçÙ
	}
	return(0xff);
}


void IOOUTCALL sndboard_psgreg(UINT port, REG8 value) {

//	TRACEOUT(("OUT %.4x, %.2x", port, value));
	sndboard.psgreg = value;
}

void IOOUTCALL sndboard_psgdat(UINT port, REG8 value) {

//	TRACEOUT(("OUT %.4x, %.2x", port, value));
	if (sndboard.psgreg < 0x0e) {
		psggen_setreg(&psggen, sndboard.psgreg, value);
	}
}

REG8 IOINPCALL sndboard_psgsta(UINT port) {

	REG8	ret;

	if (sndboard.psgreg < 0x0e) {
		return(psggen_getreg(&psggen, sndboard.psgreg));
	}
	else if (sndboard.psgreg < 0x10) {
		ret = 0xff;
		if (sndboard.psgreg == (xmilcfg.KEY_MODE + 0x0d)) {
			ret &= keystat_getjoy();
		}
		if (sndboard.psgreg == 0x0e) {
			ret &= joymng_getstat();
		}
		if (xmilcfg.BTN_RAPID) {
			sndboard.rapids ^= 0x60;
			ret |= sndboard.rapids;
		}
		if (xmilcfg.BTN_MODE) {
			ret = (ret & (~0x60)) | ((ret & 0x40) >> 1) | ((ret & 0x20) << 1);
		}
		return(ret);
	}
	else {
		return(0xff);
	}
}


// ----

void sndboard_reset(void) {

	sndboard.opmreg = 0;
	sndboard.psgreg = 0;
	sndctrl_reset();
}

