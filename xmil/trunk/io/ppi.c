#include	"compiler.h"
#include	"z80core.h"
#include	"pccore.h"
#include	"iocore.h"


/* 8255 PPI～ */

static REG8 getportb(void) {

	REG8	ret;
	REG8	ppib;
#if defined(MAINFRAMES_OLD)
	SINT32	clock;
#endif

#if 0
	if (subcpu.IBF) {
		subcpu.IBF = 0;
		ret |= 0x40;						/* 1:SUB-CPU BUSY */
	}
	if (subcpu.OBF) {
		ret |= 0x20;						/* 1:SUB-CPU Data empty */
	}
#endif	/* 0 */

	ppib = iocore.s.ppib;
	ret = ppib;

	/* ret |= cmt_test(); */				/* THUNDER BALL */

	iocore.s.ppib = (UINT8)((ppib & (~0x40)) | 0x01);

	/* ret |= cmt_read(); */

#if defined(MAINFRAMES_OLD)
	clock = CPU_CLOCKCOUNT - iocore.e.framestartclock;
	if (clock < iocore.e.dispclock) {
		ret |= 0x80;						/* 1:DISP */
	}

	/* 実機の動きを見ると　どうも 読み込んだらリセットされるようだ？ */
	/* 有効範囲が絞れるならそうすべき(VSYNCを取りこぼすソフトがある) */
	clock -= iocore.e.vsyncstart;
	if ((clock >= 0) && (clock < iocore.e.vpulseclock)) {
		ret |= 0x04;						/* 1:V-SYNC */
	}
#endif
	return(ret);
}

static void setportc(REG8 dat) {

	REG8	modify;

	modify = ppi.portc ^ dat;
	ppi.portc = dat;
	/* cmt_write((REG8)(dat & 1)); */
	if ((modify & 0x20) && (!(dat & 0x20))) {
		iocore.s.mode = 1;
		/* TRACEOUT(("iocore.s.mode = 1")); */
	}
	if (modify & 0x40) {
		crtc_setwidth((REG8)(dat & 0x40));
	}
}


/* ---- */

void IOOUTCALL ppi_o(UINT port, REG8 value) {

	REG8	bit;

	switch(port & 0x0f) {
		case 0:
			ppi.porta = value;
			return;

		case 1:
			ppi.portb = value;
			return;

		case 2:
			setportc(value);
			break;

		case 3:
			if (value & 0x80) {
				ppi.mode = value;
				return;
			}
			else {
				bit = 1 << ((value >> 1) & 7);
				if (value & 0x01) {
					setportc((REG8)(ppi.portc | bit));
				}
				else {
					setportc((REG8)(ppi.portc & (~bit)));
				}
			}
			break;
	}
}

REG8 IOINPCALL ppi_i(UINT port) {

	switch(port & 0x0f) {
		case 0:
#if 0
			if (!(ppi.mode & 0x10)) {
				return(ppi.porta);
			}
#endif	/* 0 */
			return(ppi.porta);

		case 1:
			if (!(ppi.mode & 0x02)) {
				return(ppi.portb);
			}
			return(getportb());

		case 2:
			/* mode? */
			return(ppi.portc);

		case 3:
			return(ppi.mode);
	}
	return(0xff);
}


/* initialize & reset */

void ppi_initialize(void) {

	ppi.porta = 0x00;
	ppi.portb = 0xff;
	ppi.portc = 0xff;
	ppi.mode = 0x82;
}

void ppi_reset(void) {

	ppi.porta = 0x00;
	ppi.portc |= 0x40;
	ppi.mode = 0x82;
}

