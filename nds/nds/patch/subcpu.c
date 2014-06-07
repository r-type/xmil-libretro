#include "compiler.h"
#include "timemng.h"
#include "z80core.h"
#include "pccore.h"
#include "iocore.h"
#include "nevent.h"
#include "ievent.h"
#include "keystat.h"
#include "calendar.h"


void neitem_scpu(UINT uId)
{
	BRESULT	bIntr;

	bIntr = FALSE;
	/* こうすると同時押しが判定できないのでキーバッファを持つべし */
	if (keystat.req_int)
	{
		keystat.req_int = 0;
		bIntr = TRUE;
	}
	else if (subcpu.s.keydata)
	{
		subcpu.s.keycount++;
		if (subcpu.s.keycount >= subcpu.s.keycountrep)
		{
			subcpu.s.keycount = 0;
			bIntr = TRUE;
		}
		nevent_set(NEVENT_SUBCPU, subcpu.e.intrclock, neitem_scpu, NEVENT_RELATIVE);
	}
	if (bIntr)
	{
		ievent_set(IEVENT_SUBCPU);
	}
}

BRESULT ieitem_scpu(UINT uId)
{
	UINT	uKey;
	REG8	cKeyData;

	if ((subcpu.s.cmdcnt) || (subcpu.s.datcnt))
	{
		keystat.req_int = 1;		/* 再送しる */
		subcpu_sendkey();
		return FALSE;
	}
	if (subcpu.s.vect == 0)			/* 割り込み不要だったら捨てる */
	{
		return FALSE;
	}
	uKey = keystat_getflag();
	cKeyData = (UINT8)(uKey >> 8);
	if (subcpu.s.keydata != cKeyData)
	{
		subcpu.s.keydata = cKeyData;
		subcpu.s.keycount = 0;
		subcpu.s.keycountrep = 480;
	}
	else
	{
		if (cKeyData == 0)
		{
			return FALSE;
		}
		uKey = uKey & (~0x20);			// rep
		subcpu.s.keycountrep = 48;		// 0.1sec
	}
	subcpu.s.work[1] = (UINT8)uKey;
	subcpu.s.work[0] = cKeyData;
	subcpu.s.cmdptr = offsetof(SUBCPU, s.zero);
	subcpu.s.datptr = offsetof(SUBCPU, s.work);
	subcpu.s.mode = 0xe6;
	subcpu.s.cmdcnt = 0;
	subcpu.s.datcnt = 2;

//	subcpu.s.OBF = 0;
//	subcpu.s.IBF = 1;
	iocore.s.ppib = (UINT8)((iocore.s.ppib & (~0x20)) | 0x40);

	Z80_INTERRUPT(subcpu.s.vect);
	(void)uId;
	return TRUE;
}


// ----

void subcpusetbuffer(SUBCPU *s, REG8 cMode)
{
	UINT32	uKey;

	switch(cMode)
	{
		case 0xe3:
			uKey = keystat_gete3();
			s->s.work[2] = (UINT8)(uKey >> 16);
			s->s.work[1] = (UINT8)(uKey >> 8);
			s->s.work[0] = (UINT8)(uKey >> 0);
			break;

		case 0xe6:
			uKey = keystat_getflag();
			s->s.work[1] = (UINT8)(uKey >> 0);
			s->s.work[0] = (UINT8)(uKey >> 8);
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

void subcpusendctrl(SUBCPU *s)
{
	switch(s->s.mode)
	{
		case 0xe9:
			cmt_ctrl(s->s.work[0]);
			break;

		case 0xec:
			calendar_setdate(s->s.work);
			break;

		case 0xee:
			calendar_settime(s->s.work);
			break;
	}
}


typedef struct
{
	UINT8	cFlag;
	UINT8	cPos;
} SCPUTBL;

enum
{
	SCPU_CNTMASK	= 0x07,

	SCPU_OUTPUT		= 0x08,
	SCPU_INPUT		= 0x00,
	SCPU_IODIR		= 0x08
};

static const SCPUTBL s_scputbl[0x20] =
{
		{6 | SCPU_OUTPUT,	offsetof(SUBCPU, s.timer[0])},	// d0: timer0 set
		{6 | SCPU_OUTPUT,	offsetof(SUBCPU, s.timer[1])},	// d1: timer1 set
		{6 | SCPU_OUTPUT,	offsetof(SUBCPU, s.timer[2])},	// d2: timer2 set
		{6 | SCPU_OUTPUT,	offsetof(SUBCPU, s.timer[3])},	// d3: timer3 set
		{6 | SCPU_OUTPUT,	offsetof(SUBCPU, s.timer[4])},	// d4: timer4 set
		{6 | SCPU_OUTPUT,	offsetof(SUBCPU, s.timer[5])},	// d5: timer5 set
		{6 | SCPU_OUTPUT,	offsetof(SUBCPU, s.timer[6])},	// d6: timer6 set
		{6 | SCPU_OUTPUT,	offsetof(SUBCPU, s.timer[7])},	// d7: timer7 set
		{6 | SCPU_INPUT,	offsetof(SUBCPU, s.timer[0])},	// d8: timer0 get
		{6 | SCPU_INPUT,	offsetof(SUBCPU, s.timer[1])},	// d9: timer1 get
		{6 | SCPU_INPUT,	offsetof(SUBCPU, s.timer[2])},	// da: timer2 get
		{6 | SCPU_INPUT,	offsetof(SUBCPU, s.timer[3])},	// db: timer3 get
		{6 | SCPU_INPUT,	offsetof(SUBCPU, s.timer[4])},	// dc: timer4 get
		{6 | SCPU_INPUT,	offsetof(SUBCPU, s.timer[5])},	// dd: timer5 get
		{6 | SCPU_INPUT,	offsetof(SUBCPU, s.timer[6])},	// de: timer6 get
		{6 | SCPU_INPUT,	offsetof(SUBCPU, s.timer[7])},	// df: timer7 get
		{0 | SCPU_OUTPUT,	offsetof(SUBCPU, s.zero)},		// e0:
		{0 | SCPU_OUTPUT,	offsetof(SUBCPU, s.zero)},		// e1:
		{0 | SCPU_OUTPUT,	offsetof(SUBCPU, s.zero)},		// e2:
		{3 | SCPU_INPUT,	offsetof(SUBCPU, s.work)},		// e3: game keys
		{1 | SCPU_OUTPUT,	offsetof(SUBCPU, s.vect)},		// e4: intr vector
		{0 | SCPU_OUTPUT,	offsetof(SUBCPU, s.zero)},		// e5:
		{2 | SCPU_INPUT,	offsetof(SUBCPU, s.work)},		// e6: game keys
		{1 | SCPU_OUTPUT,	offsetof(SUBCPU, s.tvctrl)},	// e7: set TV ctrl
		{1 | SCPU_INPUT,	offsetof(SUBCPU, s.tvctrl)},	// e8: get TV ctrl
		{1 | SCPU_OUTPUT,	offsetof(SUBCPU, s.work)},		// e9: cmt ctrl
		{1 | SCPU_INPUT,	offsetof(SUBCPU, s.work)},		// ea: cmtctrlstat
		{1 | SCPU_INPUT,	offsetof(SUBCPU, s.work)},		// eb: cmttypestat
		{3 | SCPU_OUTPUT,	offsetof(SUBCPU, s.work)},		// ec: date set
		{3 | SCPU_INPUT,	offsetof(SUBCPU, s.work)},		// ed: date set
		{3 | SCPU_OUTPUT,	offsetof(SUBCPU, s.work)},		// ee: time set
		{3 | SCPU_INPUT,	offsetof(SUBCPU, s.work)},		// ef: time set
};

#if !defined(USE_ARMROUTINE)
void IOOUTCALL subcpu_o(UINT uPort, REG8 cValue)
{
	UINT		uTblPos;
const SCPUTBL	*p;

	if (iocore.s.ppib & 0x40)
	{
		return;
	}
	if (!subcpu.s.cmdcnt)
	{
		subcpu.s.mode = (UINT8)cValue;
		uTblPos = cValue - 0xd0;
		if (uTblPos >= 0x20)
		{
			uTblPos = 0x10;
		}
		p = s_scputbl + uTblPos;
		iocore.s.ppib = (UINT8)(iocore.s.ppib & (~0x60));
		if ((p->cFlag & SCPU_IODIR) == SCPU_INPUT)
		{
			subcpu.s.cmdptr = offsetof(SUBCPU, s.zero);
			subcpu.s.datptr = p->cPos;
		//	subcpu.s.cmdcnt = 0;
			subcpu.s.datcnt = p->cFlag & SCPU_CNTMASK;
			iocore.s.ppib |= 0x40;
			subcpusetbuffer(&subcpu, cValue);
		}
		else
		{
			subcpu.s.cmdptr = p->cPos;
			subcpu.s.datptr = offsetof(SUBCPU, s.zero);
			subcpu.s.cmdcnt = p->cFlag & SCPU_CNTMASK;
			subcpu.s.datcnt = 0;
			iocore.s.ppib |= 0x20;
		}
	}
	else
	{
		subcpu.s.cmdcnt--;
		((UINT8 *)(&subcpu))[subcpu.s.cmdptr + subcpu.s.cmdcnt] = cValue;
		if (subcpu.s.cmdcnt == 0)
		{
			subcpusendctrl(&subcpu);
		}
	}
	(void)uPort;
}

REG8 IOINPCALL subcpu_i(UINT uPort)
{
	if (subcpu.s.datcnt)
	{
		subcpu.s.datcnt--;
	}
	else								/* D-SIDE で通るように… */
	{
		subcpusetbuffer(&subcpu, subcpu.s.mode);
	}

	iocore.s.ppib = (UINT8)(iocore.s.ppib & (~0x60));
	if (subcpu.s.datcnt)
	{
		 iocore.s.ppib |= 0x40;
	}
	else
	{
		 iocore.s.ppib |= 0x20;
	}

	(void)uPort;
	return (((UINT8 *)(&subcpu))[subcpu.s.datptr + subcpu.s.datcnt]);
}
#endif	/* !defined(USE_ARMROUTINE) */


/* reset */

void subcpu_reset(void)
{
	ZeroMemory(&subcpu, sizeof(subcpu));

	/* subcpu.OBF = 1; */
	iocore.s.ppib = (UINT8)(iocore.s.ppib | 0x20);

#if defined(FIX_Z80A)
	subcpu.e.intrclock = 2000000 * 2 / 480;
#else	/* defined(FIX_Z80A) */
	subcpu.e.intrclock = pccore.realclock / 480;
#endif	/* defined(FIX_Z80A) */
}

void subcpu_sendkey(void)
{
	if (!nevent_iswork(NEVENT_SUBCPU))
	{
		nevent_set(NEVENT_SUBCPU, subcpu.e.intrclock, neitem_scpu, NEVENT_ABSOLUTE);
	}
}

