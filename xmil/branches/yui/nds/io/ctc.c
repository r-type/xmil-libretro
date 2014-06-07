
/* #define CTCCOUNTER */
#define CTCFAST

#include	"compiler.h"
#include	"z80core.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"nevent.h"
#include	"ievent.h"


#if defined(TRACE) && defined(CTCCOUNTER)
extern UINT ctccnt;
#endif

static SINT32 minclock(const CTCCH *ch) {

	UINT32	event;
	UINT	i;
	UINT32	clock;

	event = 0x01000000;
	for (i=0; i<3; i++) {
		if ((ch->s.cmd[i] & 0x82) == 0x80) {
			clock = ch->s.count[i];
			event = min(event, clock);
		}
	}
	if ((ch->s.cmd[3] & 0x82) == 0x80) {
		clock = ch->s.count[3];
		if (ch->s.cmd[3] & 0x40) {
			clock = (clock - 1) * ch->s.countmax[0];
			clock += ch->s.count[0];
		}
		event = min(event, clock);
	}
	if (event == 0) {
		event = 1;
	}
	return(event);
}

static REG8 ctcwork(CTCCH *ch) {

	UINT32	baseclock;
	SINT32	stepclock;
	REG8	intr;
	SINT32	pulse3;
	SINT32	pulse;
	SINT32	count;

#if defined(TRACE) && defined(CTCCOUNTER)
	ctccnt++;
#endif

	baseclock = CPU_CLOCK + CPU_BASECLOCK - CPU_REMCLOCK;
	stepclock = baseclock - ch->s.baseclock;
#if defined(FIX_Z80A)			/* 2x2MHz */
	ch->s.baseclock += stepclock & (~1);
	stepclock = stepclock >> 1;
#else
	stepclock /= pccore.multiple;
	ch->s.baseclock += stepclock * pccore.multiple;
#endif

	intr = 0;
	pulse3 = 0;

	/* 0 */
	if (!(ch->s.cmd[0] & 0x02)) {
		pulse = stepclock;
		count = ch->s.count[0];
		count -= pulse;
		if (count <= 0) {
#if defined(CTCFAST)
			count += ch->s.countmax[0];
			if (count <= 0) {
				pulse3 = (0 - count) / ch->s.countmax[0];
				pulse3 += 1;
				count += pulse3 * ch->s.countmax[0];
			}
			pulse3 += 1;
#else
			pulse3 = (0 - count) / ch->s.countmax[0];
			pulse3 += 1;
			count += pulse3 * ch->s.countmax[0];
#endif
			intr |= (ch->s.cmd[0] & 0x80) >> (7 - 0);
		}
		ch->s.count[0] = count;
	}

	/* 3 */
	if (!(ch->s.cmd[3] & 0x02)) {
		if (!(ch->s.cmd[3] & 0x40)) {
			pulse3 = stepclock;
		}
		count = ch->s.count[3];
		count -= pulse3;
		if (count <= 0) {
#if defined(CTCFAST)
			count += ch->s.countmax[3];
			if (count <= 0) {
				count = ch->s.countmax[3] - ((0 - count) % ch->s.countmax[3]);
			}
#else
			count = ch->s.countmax[3] - ((0 - count) % ch->s.countmax[3]);
#endif
			intr |= (ch->s.cmd[3] & 0x80) >> (7 - 3);
#if 0
			TRACEOUT(("<- ch.3 %.8x [%.2x:%.2x %.2x:%.2x]", baseclock,
								ch->s.basecnt[0], ch->s.cmd[0],
								ch->s.basecnt[3], ch->s.cmd[3]));
#endif
		}
		ch->s.count[3] = count;
	}

	/* 1 */
	if (!(ch->s.cmd[1] & 0x02)) {
		pulse = stepclock;
		count = ch->s.count[1];
		count -= pulse;
		if (count <= 0) {
			count = ch->s.countmax[1] - ((0 - count) % ch->s.countmax[1]);
			intr |= (ch->s.cmd[1] & 0x80) >> (7 - 1);
		}
		ch->s.count[1] = count;
	}

	/* 2 */
	if (!(ch->s.cmd[2] & 0x02)) {
		pulse = stepclock;
		count = ch->s.count[2];
		count -= pulse;
		if (count <= 0) {
			count = ch->s.countmax[2] - ((0 - count) % ch->s.countmax[2]);
			intr |= (ch->s.cmd[2] & 0x80) >> (7 - 2);
		}
		ch->s.count[2] = count;
	}
	return(intr);
}

static void ctcstep(CTCCH *ch) {

	REG8	intr;

	intr = ctcwork(ch);
	if (intr) {
		ch->s.intr |= intr;
		ievent_set(IEVENT_CTC0 + ch->s.num);
	}
}

static void ctcnextevent(CTCCH *ch) {

	UINT32	event;

	if (ch->s.intr) {
		return;
	}
#if defined(FIX_Z80A)
	event = minclock(ch) * 2;
#else
	event = minclock(ch) * pccore.multiple;
#endif
	nevent_set(NEVENT_CTC0 + ch->s.num, event, neitem_ctc, NEVENT_ABSOLUTE);
}

void neitem_ctc(UINT id) {

	CTCCH	*ch;
	REG8	intr;

	ch = ctc.ch + (id - NEVENT_CTC0);
	intr = ctcwork(ch);
	if (intr) {
		ch->s.intr |= intr;
		ievent_set(IEVENT_CTC0 + ch->s.num);
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
	intr |= ch->s.intr;
	r = FALSE;
	if (intr) {
#if 1
		for (i=0, bit=1; i<4; i++, bit<<=1)
#else	/* 1 */
		for (i=4, bit=8; i--; bit>>=1)
#endif	/* 1 */
		{
			if (intr & bit) {
				if (!(ch->s.cmd[i] & 0x80)) {
					intr ^= bit;
				}
				else if (!r) {
					r = TRUE;
					intr ^= bit;
					ch->s.irq = (UINT8)i;
					/* TRACEOUT(("ctc int %d %d [%.2x]", ch->s.num, i, ch->s.cmd[i])); */
					Z80_INTERRUPT((REG8)(ch->s.vector + (i << 1)));
				}
			}
		}
	}
	ch->s.intr = intr;
	if (intr) {
		ievent_set(IEVENT_CTC0 + ch->s.num);
	}
	else {
		ctcnextevent(ch);
	}
	return(r);
}

void ieeoi_ctc(UINT id) {

	CTCCH	*ch;
	REG8	intr;
	UINT	curirq;

	ch = ctc.ch + (id - IEVENT_CTC0);
	intr = ctcwork(ch) | ch->s.intr;
	curirq = ch->s.irq;
	if (intr & (1 << curirq)) {			/* 割り込み中に割り込んだ… */
		/* カウンタが０でなければ割り込みを消す */
		if ((ch->s.countmax[curirq] - ch->s.count[curirq])
													>= ch->s.range[curirq]) {
			intr ^= (1 << curirq);
		}
	}
	ch->s.intr = intr;
	if (intr) {
		ievent_set(id);
	}
	else {
		ctcnextevent(ch);
	}
}


/* ---- */

static void ctcch_o(CTCCH *ch, UINT port, REG8 value) {

	SINT32	count;
	REG8	scale;

	port &= 3;
	if (ch->s.cmd[port] & 0x04) {
		ctcstep(ch);
		ch->s.basecnt[port] = value;
		count = ((value - 1) & 0xff) + 1;
		scale = 0;
		if (!(ch->s.cmd[port] & 0x40)) {
			if (ch->s.cmd[port] & 0x20) {
				scale = 8 - 1;
			}
			else {
				scale = 4 - 1;
			}
		}
		ch->s.scale[port] = scale;
		ch->s.countmax[port] = count << scale;
		ch->s.count[port] = count << scale;
		ch->s.range[port] = 1 << scale;
		ch->s.cmd[port] &= ~6;
		ctcnextevent(ch);
	}
	else if (value & 1) {
		ctcstep(ch);
		ch->s.cmd[port] = value;
		ctcnextevent(ch);
	}
	else if (!port) {												/* ver0.25 */
		ch->s.vector = (UINT8)(value & 0xf8);
	}
}

static REG8 ctcch_i(CTCCH *ch, UINT port) {

	port &= 3;
	if (port != 3) {
		return(ch->s.basecnt[port]);
	}
	else {
		ctcstep(ch);
		return((REG8)(ch->s.count[3] >> ch->s.scale[3]));
	}
}


/* ---- */

static CTCCH *getctcch(UINT port) {

	port &= ~3;
	if (port == 0x1fa0) {
		return(ctc.ch + CTC_TURBO1);
	}
	if (port == 0x1fa8) {
		return(ctc.ch + CTC_TURBO2);
	}
	if (port == 0x0704) {
		return(ctc.ch + CTC_OPM);
	}
	return(NULL);
}

void IOOUTCALL ctc_o(UINT port, REG8 value) {

	CTCCH	*ch;

	/* TRACEOUT(("ctc - %.4x %.2x [%.4x]", port, value, Z80_PC)); */
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


/* ---- */

void ctc_reset(void) {

	UINT	i;
	UINT	j;

	ZeroMemory(&ctc, sizeof(ctc));
	for (i=0; i<3; i++) {
		ctc.ch[i].s.num = (UINT8)i;
		for (j=0; j<4; j++) {
			ctc.ch[i].s.cmd[j] = 0x23;
			ctc.ch[i].s.scale[j] = 7;
			ctc.ch[i].s.count[j] = 256 << 7;
			ctc.ch[i].s.countmax[j] = 256 << 7;
		}
	}
}

