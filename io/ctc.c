#include	"compiler.h"
#include	"z80core.h"
#include	"pccore.h"
#include	"iocore.h"


void x1_ctc_int(void) {

	CTCCH	*ch;
	UINT	r;
	SINT32	clk0;
	SINT32	clk4;
	SINT32	clk2;
	SINT32	subcnt;
	UINT	i;
	REG8	bit;
	REG8	ctcint_flg;

	ctcint_flg = Z80_ABLEINTERRUPT();

	clk4 = pccore.HSYNC_CLK;
	clk2 = clk4 >> 1;

	if (pccore.ROM_TYPE >= 2) {
		ch = ctc.ch + 0;
		r = 3;
	}
	else {
		ch = ctc.ch + 2;
		r = 1;
	}

	do {
		clk0 = 0;
		bit = 1;
		for (i=0; i<4; i++) {
			if (!(ch->cmd[i] & 0x02)) {
				if (ch->count[i] <= 0) {
					ch->count[i] += ch->countmax[i];
				}
				if (!(ch->cmd[i] & 0x40)) {
					subcnt = clk4;
				}
				else if (i == 3) {
					subcnt = clk0;
				}
				else {
					subcnt = clk2;
				}
				ch->count[i] -= subcnt;
				if (ch->count[i] <= 0) {
					ch->int_flag |= bit;
					if (!i) {
						clk0 = 1;
					}
				}
			}
			bit <<= 1;
		}

		bit = 1;
		for (i=0; i<4; i++) {
			if (ch->int_flag & bit) {
				if (!(ch->cmd[i] & 0x80)) {
					ch->int_flag ^= bit;
				}
				else if (ctcint_flg) {
					ctcint_flg = 0;
					ch->int_flag ^= bit;
					TRACEOUT(("ctc%u int -- %d", 3 - r, i));
					Z80_INT((REG8)(ch->vector + (i << 1)));
				}
			}
			bit <<= 1;
		}
		ch++;
	} while(--r);
}


// ----

static void ctcch_o(CTCCH *ch, UINT port, REG8 value) {

	REG8	scale;
	SINT32	count;

	port &= 3;
	if (ch->cmd[port] & 0x04) {
		scale = 0;
		count = 256;
		ch->basecnt[port] = value;
		if (value) {
			count = (SINT32)value;
		}
		if (!(ch->cmd[port] & 0x40)) {
			if (ch->cmd[port] & 0x20) {
				scale = 8;
			}
			else {
				scale = 4;
			}
		}
		count <<= scale;
		ch->countmax[port] = count;
		ch->count[port] = count;
		ch->cmd[port] &= ~6;
	}
	else if (value & 1) {
		ch->cmd[port] = value;
	}
	else if (!port) {												// ver0.25
		ch->vector = (UINT8)(value & 0xf8);
	}
}

static REG8 ctcch_i(CTCCH *ch, UINT port) {

	REG8	scale;

	port &= 3;
	if (port != 3) {
		return(ch->basecnt[port]);
	}
	else {
		scale = 0;
		if (!(ch->cmd[3] & 0x40)) {
			if (ch->cmd[3] & 0x20) {
				scale = 8;
			}
			else {
				scale = 4;
			}
		}
		return((REG8)(ch->count[3] >> scale));
	}
}


// ----

static CTCCH *getctcch(UINT port) {

	port &= ~3;
	if (port == 0x1fa0) {
		return(ctc.ch + 0);
	}
	if (port == 0x1fa8) {
		return(ctc.ch + 1);
	}
	if ((pccore.SOUND_SW) && (port == 0x0704)) {
		return(ctc.ch + 2);
	}
	return(NULL);
}

void IOOUTCALL ctc_o(UINT port, REG8 value) {

	CTCCH	*ch;

	TRACEOUT(("ctc - %.4x %.2x [%.4x]", port, value, Z80_PC));
	ch = getctcch(port);
	if (ch != NULL) {
		ctcch_o(ch, port, value);
	}
}

REG8 IOINPCALL ctc_i(UINT port) {

	CTCCH	*ch;

	ch = getctcch(port);
	if (ch != NULL) {
		return(ctcch_i(ch, port));
	}
	else {
		return((REG8)0xff);
	}
}


// ----

void ctc_reset(void) {

	UINT	i, j;

	ZeroMemory(&ctc, sizeof(ctc));
	for (i=0; i<3; i++) {
		for (j=0; j<4; j++) {
			ctc.ch[i].cmd[j] = 0x83;
		}
	}
}

