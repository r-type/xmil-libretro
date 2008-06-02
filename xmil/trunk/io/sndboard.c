#include	"compiler.h"
#include	"joymng.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"keystat.h"
#include	"sound.h"
#include	"sndctrl.h"
#include	"x1f.h"


#if defined(SUPPORT_TURBOZ) || defined(SUPPORT_OPM)
void IOOUTCALL opm_o(UINT port, REG8 dat) {

	REG8	lsb;
	REG8	reg;

	lsb = (UINT8)port;
	if (lsb == 0x00) {					/* 0700 */
		sndboard.opmreg = (UINT8)dat;
	}
	else if (lsb == 0x01) {				/* 0701 */
		reg = sndboard.opmreg;
		sndboard.opmdat[reg] = dat;
		x1f_opm(reg, dat);
#if !defined(DISABLE_SOUND)
		opmgen_setreg(reg, dat);
#endif
	}
	else if ((lsb & (~3)) == 0x04) {	/* 0704-0707 */
		ctc_o(port, dat);
	}
}

REG8 IOINPCALL opm_i(UINT port) {

	REG8	lsb;

	lsb = (UINT8)port;
	if ((lsb & (~1)) == 0x00) {			/* 0700/0701 */
		return(0x00);											/* É[ÉäÉAÅ[Éh */
	}
	else if ((lsb & (~3)) == 0x04) {	/* 0704-0707 */
		return(ctc_i(port));
	}
	else {
		return(0xff);
	}
}
#endif


void IOOUTCALL sndboard_psgreg(UINT port, REG8 dat) {

	sndboard.psgreg = dat;
	(void)port;
}

void IOOUTCALL sndboard_psgdat(UINT port, REG8 dat) {

	REG8	reg;

	reg = sndboard.psgreg;
	if (reg < 0x0e) {
		sndboard.psgdat[reg] = dat;
		x1f_psg(reg, dat);
#if !defined(DISABLE_SOUND)
		psggen_setreg(&psggen, reg, dat);
#endif
	}
	(void)port;
}

REG8 IOINPCALL sndboard_psgsta(UINT port) {

	REG8	ret;

	if (sndboard.psgreg < 0x0e) {
		return(sndboard.psgdat[sndboard.psgreg]);
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
		(void)port;
		return(0xff);
	}
}


/* reset */

#if !defined(DISABLE_SOUND)
void sndboard_update(void) {

	UINT	i;

	for (i=0; i<14; i++) {
		psggen_setreg(&psggen, (REG8)i, sndboard.psgdat[i]);
	}
#if defined(SUPPORT_TURBOZ) || defined(SUPPORT_OPM)
	for (i=0x20; i<0x100; i++) {
		opmgen_setreg((REG8)i, sndboard.opmdat[i]);
	}
#endif
}
#endif

void sndboard_reset(void) {

	ZeroMemory(&sndboard, sizeof(sndboard));
	sndboard.psgdat[0x07] = 0x3f;
	sndctrl_reset();
}

