#include	"compiler.h"
#include	"z80core.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"nevent.h"
#include	"ievent.h"


static REG8 ctcwork(CTCCH *ch) {

	UINT32	baseclock;
	SINT32	stepclock;
	REG8	intr;
	SINT32	pulse3;
	SINT32	pulse;
	SINT32	count;

	baseclock = CPU_CLOCK + CPU_BASECLOCK - CPU_REMCLOCK;
	stepclock = baseclock - ch->baseclock;
	stepclock /= pccore.multiple;
	ch->baseclock += stepclock * pccore.multiple;

	intr = 0;
	pulse3 = 0;

	// 0
	if (!(ch->cmd[0] & 0x02)) {
		pulse = stepclock;					// 2MHz
		if (!(ch->cmd[0] & 0x40)) {
			pulse = pulse * 2;				// 4MHz
		}
		count = ch->count[0];
		count -= pulse;
		if (count <= 0) {
			pulse3 = (0 - count) / ch->countmax[0];
			pulse3 += 1;
			count += pulse3 * ch->countmax[0];
			intr |= (ch->cmd[0] & 0x80) >> (7 - 0);
		}
		ch->count[0] = count;
	}

	// 3
	if (!(ch->cmd[3] & 0x02)) {
		if (!(ch->cmd[3] & 0x40)) {
			pulse3 = stepclock * 2;				// 4MHz
		}
		count = ch->count[3];
		count -= pulse3;
		if (count <= 0) {
			count = ch->countmax[3] - ((0 - count) % ch->countmax[3]);
			intr |= (ch->cmd[3] & 0x80) >> (7 - 3);
		}
		ch->count[3] = count;
	}

	// 1
	if (!(ch->cmd[1] & 0x02)) {
		pulse = stepclock;					// 2MHz
		if (!(ch->cmd[1] & 0x40)) {
			pulse = pulse * 2;				// 4MHz
		}
		count = ch->count[1];
		count -= pulse;
		if (count <= 0) {
			count = ch->countmax[1] - ((0 - count) % ch->countmax[1]);
			intr |= (ch->cmd[1] & 0x80) >> (7 - 1);
		}
		ch->count[1] = count;
	}

	// 2
	if (!(ch->cmd[2] & 0x02)) {
		pulse = stepclock;					// 2MHz
		if (!(ch->cmd[2] & 0x40)) {
			pulse = pulse * 2;				// 4MHz
		}
		count = ch->count[2];
		count -= pulse;
		if (count <= 0) {
			count = ch->countmax[2] - ((0 - count) % ch->countmax[2]);
			intr |= (ch->cmd[2] & 0x80) >> (7 - 2);
		}
		ch->count[2] = count;
	}
	return(intr);
}

static void ctcstep(CTCCH *ch) {

	REG8	intr;

	intr = ctcwork(ch);
	if (intr) {
		ch->intr |= intr;
		TRACEOUT(("-> ievent_set"));
		ievent_set(IEVENT_CTC0 + ch->num);
	}
}

static void ctcnextevent(CTCCH *ch) {

	UINT32	event;
	UINT	i;
	UINT32	clock;

	if (ch->intr) {
		return;
	}
	event = 0x04000000;
	for (i=0; i<3; i++) {
		if ((ch->cmd[i] & 0x82) == 0x80) {
			clock = ch->count[i];
			if (ch->cmd[i] & 0x40) {
				clock = clock * 2;
			}
			TRACEOUT(("ch %d -> %d :%.2x:%.8x", i, clock, ch->cmd[i], ch->count[i]));
			event = min(event, clock);
		}
	}
	if ((ch->cmd[3] & 0x82) == 0x80) {
		clock = ch->count[3];
		if (ch->cmd[3] & 0x40) {
			clock = (clock - 1) * ch->countmax[0];
			clock += ch->count[0];
			if (ch->cmd[0] & 0x40) {
				clock = clock * 2;
			}
		}
		TRACEOUT(("ch %d -> %d", 3, clock));
		event = min(event, clock);
	}
	event /= 2;
	event *= pccore.multiple;
	nevent_set(NEVENT_CTC0 + ch->num, event, neitem_ctc, NEVENT_ABSOLUTE);
	TRACEOUT(("ctc -> %d (%x)", event, event));
}

void neitem_ctc(UINT id) {

	CTCCH	*ch;
	REG8	intr;

	ch = ctc.ch + (id - NEVENT_CTC0);
	intr = ctcwork(ch);
	if (intr) {
		ch->intr |= intr;
		TRACEOUT(("-> ievent_set"));
		ievent_set(IEVENT_CTC0 + ch->num);
	}
	else {
		ctcnextevent(ch);
	}
}

BRESULT ieitem_ctc(UINT id) {

	CTCCH	*ch;
	REG8	intr;
	BRESULT	r;
	UINT	i;
	REG8	bit;

	ch = ctc.ch + (id - IEVENT_CTC0);
	intr = ctcwork(ch);
	intr |= ch->intr;
	TRACEOUT(("ieitem_ctc %d - %.2x", id - IEVENT_CTC0, intr));
	r = FALSE;
	if (intr) {
		for (i=0, bit=1; i<4; i++, bit<<=1) {
			if (intr & bit) {
#if 1			// アークスのタイミング→あとで修正
				if (0)
#elif 0
				if (((ch->count[i] * 17) >> 4) < ch->countmax[i])
#else
				if (ch->count[i] != ch->countmax[i])
#endif
				{
					intr ^= bit;
				}
				else if (!(ch->cmd[i] & 0x80)) {
					intr ^= bit;
				}
				else if (!r) {
					r = TRUE;
					intr ^= bit;
					TRACEOUT(("z80int %d", i));
					Z80_INT((REG8)(ch->vector + (i << 1)));
				}
			}
		}
	}
	TRACEOUT(("--> %.2x", intr));
	ch->intr = intr;
	if (intr) {
		TRACEOUT(("-> ievent_set"));
		ievent_set(IEVENT_CTC0 + ch->num);
	}
	else {
		ctcnextevent(ch);
	}
	return(r);
}




#if 0
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
//					TRACEOUT(("ctc%u int -- %d", 3 - r, i));
					Z80_INT((REG8)(ch->vector + (i << 1)));
				}
			}
			bit <<= 1;
		}
		ch++;
	} while(--r);
}
#endif


// ----

static void ctcch_o(CTCCH *ch, UINT port, REG8 value) {

	SINT32	count;
	REG8	scale;

	port &= 3;
	if (ch->cmd[port] & 0x04) {
		ctcstep(ch);
		ch->basecnt[port] = value;
		count = 256;
		if (value) {
			count = (SINT32)value;
		}
		scale = 0;
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
		ctcnextevent(ch);
	}
	else if (value & 1) {
		ctcstep(ch);
		ch->cmd[port] = value;
		ctcnextevent(ch);
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
		ctcstep(ch);
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
	if (port == 0x0704) {
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
		return(0xff);
	}
}


// ----

void ctc_reset(void) {

	UINT	i;
	UINT	j;

	ZeroMemory(&ctc, sizeof(ctc));
	for (i=0; i<3; i++) {
		ctc.ch[i].num = (UINT8)i;
		for (j=0; j<4; j++) {
			ctc.ch[i].cmd[j] = 0x03;
			ctc.ch[i].count[j] = 256 << 8;
			ctc.ch[i].countmax[j] = 256 << 8;
		}
	}
}

