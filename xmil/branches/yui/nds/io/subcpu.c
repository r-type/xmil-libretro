#include	"compiler.h"
#include	"timemng.h"
#include	"z80core.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"nevent.h"
#include	"ievent.h"
#include	"keystat.h"
#include	"calendar.h"


/*     e3 e4 e5 e6 e7 e8 e9 ea eb ec ed ee ef
 * cmd: 0, 1, 0, 0, 1, 0, 1, 0, 0, 3, 0, 3, 0
 * dat: 3, 0, 0, 2, 0, 1, 0, 1, 1, 0, 3, 0, 3
 */

typedef struct {
	UINT8	pos;
	UINT8	flag;
} SCPUTBL;

enum {
	SCPU_CNTMASK	= 0x07,
	SCPU_INPUT		= 0x08
};

static const SCPUTBL scputbl[0x20] = {
		{offsetof(SUBCPU, s.timer[0]),	6},					/* d0: timer0 set  */
		{offsetof(SUBCPU, s.timer[1]),	6},					/* d1: timer1 set  */
		{offsetof(SUBCPU, s.timer[2]),	6},					/* d2: timer2 set  */
		{offsetof(SUBCPU, s.timer[3]),	6},					/* d3: timer3 set  */
		{offsetof(SUBCPU, s.timer[4]),	6},					/* d4: timer4 set  */
		{offsetof(SUBCPU, s.timer[5]),	6},					/* d5: timer5 set  */
		{offsetof(SUBCPU, s.timer[6]),	6},					/* d6: timer6 set  */
		{offsetof(SUBCPU, s.timer[7]),	6},					/* d7: timer7 set  */
		{offsetof(SUBCPU, s.timer[0]),	6 + SCPU_INPUT},	/* d8: timer0 get  */
		{offsetof(SUBCPU, s.timer[1]),	6 + SCPU_INPUT},	/* d9: timer1 get  */
		{offsetof(SUBCPU, s.timer[2]),	6 + SCPU_INPUT},	/* da: timer2 get  */
		{offsetof(SUBCPU, s.timer[3]),	6 + SCPU_INPUT},	/* db: timer3 get  */
		{offsetof(SUBCPU, s.timer[4]),	6 + SCPU_INPUT},	/* dc: timer4 get  */
		{offsetof(SUBCPU, s.timer[5]),	6 + SCPU_INPUT},	/* dd: timer5 get  */
		{offsetof(SUBCPU, s.timer[6]),	6 + SCPU_INPUT},	/* de: timer6 get  */
		{offsetof(SUBCPU, s.timer[7]),	6 + SCPU_INPUT},	/* df: timer7 get  */
		{offsetof(SUBCPU, s.zero),		0},					/* e0:             */
		{offsetof(SUBCPU, s.zero),		0},					/* e1:             */
		{offsetof(SUBCPU, s.zero),		0},					/* e2:             */
		{offsetof(SUBCPU, s.work),		3 + SCPU_INPUT},	/* e3: game keys   */
		{offsetof(SUBCPU, s.vect),		1},					/* e4: intr vector */
		{offsetof(SUBCPU, s.zero),		0},					/* e5:             */
		{offsetof(SUBCPU, s.work),		2 + SCPU_INPUT},	/* e6: game keys   */
		{offsetof(SUBCPU, s.tvctrl),	1},					/* e7: set TV ctrl */
		{offsetof(SUBCPU, s.tvctrl),	1 + SCPU_INPUT},	/* e8: get TV ctrl */
		{offsetof(SUBCPU, s.work),		1},					/* e9: cmt ctrl    */
		{offsetof(SUBCPU, s.work),		1 + SCPU_INPUT},	/* ea: cmtctrlstat */
		{offsetof(SUBCPU, s.work),		1 + SCPU_INPUT},	/* eb: cmttypestat */
		{offsetof(SUBCPU, s.work),		3},					/* ec: date set    */
		{offsetof(SUBCPU, s.work),		3 + SCPU_INPUT},	/* ed: date set    */
		{offsetof(SUBCPU, s.work),		3},					/* ee: time set    */
		{offsetof(SUBCPU, s.work),		3 + SCPU_INPUT}};	/* ef: time set    */


/* ---- */

void neitem_scpu(UINT id) {

	BRESULT	intr;

	intr = FALSE;
	/* こうすると同時押しが判定できないのでキーバッファを持つべし */
	if (keystat.req_int) {
		keystat.req_int = 0;
		intr = TRUE;
	}
	else if (subcpu.s.keydata) {
		subcpu.s.keycount++;
		if (subcpu.s.keycount >= subcpu.s.keycountrep) {
			subcpu.s.keycount = 0;
			intr = TRUE;
		}
		nevent_set(NEVENT_SUBCPU, subcpu.e.intrclock,
											neitem_scpu, NEVENT_RELATIVE);
	}
	if (intr) {
		ievent_set(IEVENT_SUBCPU);
	}
}

BRESULT ieitem_scpu(UINT id) {

	UINT	key;
	UINT8	keydata;

	if ((subcpu.s.cmdcnt) || (subcpu.s.datcnt)) {
		keystat.req_int = 1;			/* 再送しる */
		subcpu_sendkey();
		return(FALSE);
	}
	if (subcpu.s.vect == 0) {			/* 割り込み不要だったら捨てる */
		return(FALSE);
	}
	key = keystat_getflag();
	keydata = (UINT8)(key >> 8);
	if (subcpu.s.keydata != keydata) {
		subcpu.s.keydata = keydata;
		subcpu.s.keycount = 0;
		subcpu.s.keycountrep = 480;
	}
	else {
		if (keydata == 0) {
			return(FALSE);
		}
		key = key & (~0x20);			/* rep */
		subcpu.s.keycountrep = 48;		/* 0.1sec */
	}
	subcpu.s.work[1] = (UINT8)key;
	subcpu.s.work[0] = keydata;
	subcpu.s.cmdptr = offsetof(SUBCPU, s.zero);
	subcpu.s.datptr = offsetof(SUBCPU, s.work);
	subcpu.s.mode = 0xe6;
	subcpu.s.cmdcnt = 0;
	subcpu.s.datcnt = 2;

	iocore.s.ppib = (UINT8)((iocore.s.ppib & (~0x20)) | 0x40);

	Z80_INTERRUPT(subcpu.s.vect);
	(void)id;
	return(TRUE);
}


/* ---- */

static void subcpusetbuffer(SUBCPU *s) {

	UINT32	key;

	switch(s->s.mode) {
		case 0xe3:
			key = keystat_gete3();
			s->s.work[2] = (UINT8)(key >> 16);
			s->s.work[1] = (UINT8)(key >> 8);
			s->s.work[0] = (UINT8)(key >> 0);
			break;

		case 0xe6:
			key = keystat_getflag();
			s->s.work[1] = (UINT8)(key >> 0);
			s->s.work[0] = (UINT8)(key >> 8);
			break;

		case 0xea:
			s->s.work[0] = cmt_ctrl_stat();
			break;

		case 0xeb:
			s->s.work[0] = cmt_tape_stat();
			break;

		case 0xed:
			calendar_getdate(s->s.work);
			break;

		case 0xef:
			calendar_gettime(s->s.work);
			break;
	}
}

void IOOUTCALL subcpu_o(UINT port, REG8 value) {

	UINT		tblpos;
const SCPUTBL	*p;

	if (iocore.s.ppib & 0x40) {
		return;
	}
	if (!subcpu.s.cmdcnt) {
		subcpu.s.mode = (UINT8)value;
		tblpos = value - 0xd0;
		if (tblpos >= 0x20) {
			tblpos = 0x10;
		}
		p = scputbl + tblpos;
		iocore.s.ppib = (UINT8)(iocore.s.ppib & (~0x60));
		if (p->flag & SCPU_INPUT) {
			subcpu.s.cmdptr = offsetof(SUBCPU, s.zero);
			subcpu.s.datptr = p->pos;
			/* subcpu.s.cmdcnt = 0; */
			subcpu.s.datcnt = p->flag & SCPU_CNTMASK;
			iocore.s.ppib |= 0x40;
			subcpusetbuffer(&subcpu);
		}
		else {
			subcpu.s.cmdptr = p->pos;
			subcpu.s.datptr = offsetof(SUBCPU, s.zero);
			subcpu.s.cmdcnt = p->flag & SCPU_CNTMASK;
			subcpu.s.datcnt = 0;
			iocore.s.ppib |= 0x20;
		}
	}
	else {
		subcpu.s.cmdcnt--;
		((UINT8 *)(&subcpu))[subcpu.s.cmdptr + subcpu.s.cmdcnt] = value;
		if (subcpu.s.cmdcnt == 0) {
			switch(subcpu.s.mode) {
				case 0xe9:
					cmt_ctrl(subcpu.s.work[0]);
					break;

				case 0xec:
					calendar_setdate(subcpu.s.work);
					break;

				case 0xee:
					calendar_settime(subcpu.s.work);
					break;
			}
		}
	}
	(void)port;
}

REG8 IOINPCALL subcpu_i(UINT port) {

	if (subcpu.s.datcnt) {
		subcpu.s.datcnt--;
	}
	else {									/* D-SIDE で通るように… */
		subcpusetbuffer(&subcpu);
	}

	iocore.s.ppib = (UINT8)(iocore.s.ppib & (~0x60));
	if (subcpu.s.datcnt) {
		 iocore.s.ppib |= 0x40;
	}
	else {
		 iocore.s.ppib |= 0x20;
	}

	(void)port;
	return(((UINT8 *)(&subcpu))[subcpu.s.datptr + subcpu.s.datcnt]);
}


/* reset */

void subcpu_reset(void) {

	ZeroMemory(&subcpu, sizeof(subcpu));

	iocore.s.ppib = (UINT8)(iocore.s.ppib | 0x20);

#if defined(FIX_Z80A)
	subcpu.e.intrclock = 2000000 * 2 / 480;
#else
	subcpu.e.intrclock = pccore.realclock / 480;
#endif
}

void subcpu_sendkey(void) {

	if (!nevent_iswork(NEVENT_SUBCPU)) {
		nevent_set(NEVENT_SUBCPU, subcpu.e.intrclock,
											neitem_scpu, NEVENT_ABSOLUTE);
	}
}

