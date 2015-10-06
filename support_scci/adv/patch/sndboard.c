#include	"compiler.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"advpsg.h"


	_ADVPSG	advpsg;


void IOOUTCALL sndboard_psgreg(UINT port, REG8 dat) {

	advpsg.addr = dat;
	(void)port;
}

void IOOUTCALL sndboard_psgdat(UINT port, REG8 dat) {

	advpsg_setreg(&advpsg, advpsg.addr, dat);
	(void)port;
}

REG8 IOINPCALL sndboard_psgsta(UINT port) {

	if (advpsg.addr < 0x10) {
		// ’¼Ú‚æ‚Þ‚æ‚Þ
		return(((UINT8 *)&advpsg.reg)[advpsg.addr]);
//		return(advpsg_getreg(&advpsg, advpsg.addr));
	}
	else {
		(void)port;
		return(0xff);
	}
}


// ----

void sndboard_reset(void) {

	advpsg_reset(&advpsg);
}

