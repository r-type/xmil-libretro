
#define	CTCFAST

#include "compiler.h"
#include "z80core.h"
#include "pccore.h"
#include "iocore.h"
#include "nevent.h"
#include "ievent.h"


static SINT32 minclock(const CTCCH *pCh)
{
	UINT32	uEvent;
	UINT	i;
	UINT32	uClock;

	uEvent = 0x01000000;
	for (i=0; i<3; i++)
	{
		if ((pCh->s.cmd[i] & 0x82) == 0x80)
		{
			uClock = pCh->s.count[i];
			uEvent = min(uEvent, uClock);
		}
	}
	if ((pCh->s.cmd[3] & 0x82) == 0x80)
	{
		uClock = pCh->s.count[3];
		if (pCh->s.cmd[3] & 0x40)
		{
			uClock = (uClock - 1) * pCh->s.countmax[0];
			uClock += pCh->s.count[0];
		}
		uEvent = min(uEvent, uClock);
	}
	if (uEvent == 0)
	{
		uEvent = 1;
	}
	return uEvent;
}


#if defined(USE_ARMROUTINE)
extern REG8 LONG_CALL ctcwork(CTCCH *pCh);
#else /* defined(USE_ARMROUTINE) */
static REG8 ctcwork(CTCCH *pCh)
{
	UINT32	uBaseClock;
	SINT32	nStepClock;
	REG8	cIntr;
	SINT32	nPulse3;
	SINT32	nPulse;
	SINT32	nCount;

	uBaseClock = CPU_CLOCK + CPU_BASECLOCK - CPU_REMCLOCK;
	nStepClock = uBaseClock - pCh->s.baseclock;
#if defined(FIX_Z80A)			/* 2x2MHz */
	pCh->s.baseclock += nStepClock & (~1);
	nStepClock = nStepClock >> 1;
#else	/* defined(FIX_Z80A) */
	nStepClock /= pccore.multiple;
	pCh->s.baseclock += nStepClock * pccore.multiple;
#endif	/* defined(FIX_Z80A) */

	cIntr = 0;
	nPulse3 = 0;

	// 0
	if (!(pCh->s.cmd[0] & 0x02))
	{
		nPulse = nStepClock;
		nCount = pCh->s.count[0];
		nCount -= nPulse;
		if (nCount <= 0)
		{
#if defined(CTCFAST)
			nCount += pCh->s.countmax[0];
			if (nCount <= 0)
			{
				nPulse3 = (0 - nCount) / pCh->s.countmax[0];
				nPulse3 += 1;
				nCount += nPulse3 * pCh->s.countmax[0];
			}
			nPulse3 += 1;
#else
			nPulse3 = (0 - nCount) / pCh->s.countmax[0];
			nPulse3 += 1;
			nCount += nPulse3 * pCh->s.countmax[0];
#endif
			cIntr |= (pCh->s.cmd[0] & 0x80) >> (7 - 0);
		}
		pCh->s.count[0] = nCount;
	}

	// 3
	if (!(pCh->s.cmd[3] & 0x02))
	{
		if (!(pCh->s.cmd[3] & 0x40))
		{
			nPulse3 = nStepClock;
		}
		nCount = pCh->s.count[3];
		nCount -= nPulse3;
		if (nCount <= 0)
		{
#if defined(CTCFAST)
			nCount += pCh->s.countmax[3];
			if (nCount <= 0)
			{
				nCount = pCh->s.countmax[3] - ((0 - nCount) % pCh->s.countmax[3]);
			}
#else
			nCount = pCh->s.countmax[3] - ((0 - nCount) % pCh->s.countmax[3]);
#endif
			cIntr |= (pCh->s.cmd[3] & 0x80) >> (7 - 3);
			/* TRACEOUT(("<- ch.3 %.8x [%.2x:%.2x %.2x:%.2x]", baseclock, pCh->s.basecnt[0], pCh->s.cmd[0], pCh->s.basecnt[3], pCh->s.cmd[3])); */
		}
		pCh->s.count[3] = nCount;
	}

	// 1
	if (!(pCh->s.cmd[1] & 0x02))
	{
		nPulse = nStepClock;
		nCount = pCh->s.count[1];
		nCount -= nPulse;
		if (nCount <= 0)
		{
			nCount = pCh->s.countmax[1] - ((0 - nCount) % pCh->s.countmax[1]);
			cIntr |= (pCh->s.cmd[1] & 0x80) >> (7 - 1);
		}
		pCh->s.count[1] = nCount;
	}

	// 2
	if (!(pCh->s.cmd[2] & 0x02))
	{
		nPulse = nStepClock;
		nCount = pCh->s.count[2];
		nCount -= nPulse;
		if (nCount <= 0)
		{
			nCount = pCh->s.countmax[2] - ((0 - nCount) % pCh->s.countmax[2]);
			cIntr |= (pCh->s.cmd[2] & 0x80) >> (7 - 2);
		}
		pCh->s.count[2] = nCount;
	}

	return cIntr;
}
#endif	/* defined(USE_ARMROUTINE) */

static void ctcstep(CTCCH *pCh)
{
	REG8	cIntr;

	cIntr = ctcwork(pCh);
	if (cIntr)
	{
		pCh->s.intr |= cIntr;
		ievent_set(IEVENT_CTC0 + pCh->s.num);
	}
}

static void ctcnextevent(CTCCH *pCh)
{
	UINT32	uEvent;

	if (pCh->s.intr)
	{
		return;
	}
#if defined(FIX_Z80A)
	uEvent = minclock(pCh) * 2;
#else
	uEvent = minclock(pCh) * pccore.multiple;
#endif
	nevent_set(NEVENT_CTC0 + pCh->s.num, uEvent, neitem_ctc, NEVENT_ABSOLUTE);
}

void neitem_ctc(NEVENTID uId)
{
	CTCCH	*pCh;
	REG8	cIntr;

	pCh = ctc.ch + (uId - NEVENT_CTC0);
	cIntr = ctcwork(pCh);
	if (cIntr)
	{
		pCh->s.intr |= cIntr;
		ievent_set(IEVENT_CTC0 + pCh->s.num);
	}
	else
	{
		ctcnextevent(pCh);
	}
}

BOOL ieitem_ctc(UINT uId)
{
	CTCCH	*pCh;
	REG8	cIntr;
	BOOL	r;
	UINT	i;
	REG8	cBit;

	pCh = ctc.ch + (uId - IEVENT_CTC0);
	cIntr = ctcwork(pCh);
	cIntr |= pCh->s.intr;
	r = FALSE;
	if (cIntr)
	{
		for (i=0, cBit=1; i<4; i++, cBit<<=1)
		{
			if (cIntr & cBit)
			{
				if (!(pCh->s.cmd[i] & 0x80))
				{
					cIntr ^= cBit;
				}
				else if (!r)
				{
					r = TRUE;
					cIntr ^= cBit;
					pCh->s.irq = (UINT8)i;
					/* TRACEOUT(("ctc int %d %d [%.2x]", pCh->s.num, i, pCh->s.cmd[i])); */
					Z80_INTERRUPT((REG8)(pCh->s.vector + (i << 1)));
				}
			}
		}
	}
	pCh->s.intr = cIntr;
	if (cIntr)
	{
		ievent_set(IEVENT_CTC0 + pCh->s.num);
	}
	else
	{
		ctcnextevent(pCh);
	}
	return r;
}

void ieeoi_ctc(UINT uId)
{
	CTCCH	*pCh;
	REG8	cIntr;
	UINT	uCurIrq;

	pCh = ctc.ch + (uId - IEVENT_CTC0);
	cIntr = ctcwork(pCh) | pCh->s.intr;
	uCurIrq = pCh->s.irq;
	if (cIntr & (1 << uCurIrq))			/* 割り込み中に割り込んだ… */
	{
		/* カウンタが０でなければ割り込みを消す */
		if ((pCh->s.countmax[uCurIrq] - pCh->s.count[uCurIrq]) >= pCh->s.range[uCurIrq])
		{
			cIntr ^= (1 << uCurIrq);
		}
	}
	pCh->s.intr = cIntr;
	if (cIntr)
	{
		ievent_set(uId);
	}
	else
	{
		ctcnextevent(pCh);
	}
}


// ----

static void ctcch_o(CTCCH *pCh, UINT uPort, REG8 cValue)
{
	SINT32	nCount;
	REG8	cScale;

	uPort &= 3;
	if (pCh->s.cmd[uPort] & 0x04)
	{
		ctcstep(pCh);
		pCh->s.basecnt[uPort] = cValue;
		nCount = ((cValue - 1) & 0xff) + 1;
		cScale = 0;
		if (!(pCh->s.cmd[uPort] & 0x40))
		{
			if (pCh->s.cmd[uPort] & 0x20)
			{
				cScale = 8 - 1;
			}
			else 
			{
				cScale = 4 - 1;
			}
		}
		pCh->s.scale[uPort] = cScale;
		pCh->s.countmax[uPort] = nCount << cScale;
		pCh->s.count[uPort] = nCount << cScale;
		pCh->s.range[uPort] = 1 << cScale;
		pCh->s.cmd[uPort] &= ~6;
		ctcnextevent(pCh);
	}
	else if (cValue & 1)
	{
		ctcstep(pCh);
		pCh->s.cmd[uPort] = cValue;
		ctcnextevent(pCh);
	}
	else if (!uPort)
	{
		pCh->s.vector = (UINT8)(cValue & 0xf8);
	}
}

static REG8 ctcch_i(CTCCH *pCh, UINT uPort)
{
	uPort &= 3;
	if (uPort != 3)
	{
		return pCh->s.basecnt[uPort];
	}
	else
	{
		ctcstep(pCh);
		return ((REG8)(pCh->s.count[3] >> pCh->s.scale[3]));
	}
}


// ----

static CTCCH *getctcch(UINT uPort)
{
	uPort &= ~3;
	if (uPort == 0x1fa0)
	{
		return ctc.ch + CTC_TURBO1;
	}
	if (uPort == 0x1fa8)
	{
		return ctc.ch + CTC_TURBO2;
	}
	if (uPort == 0x0704)
	{
		return ctc.ch + CTC_OPM;
	}
	return NULL;
}

void IOOUTCALL ctc_o(UINT uPort, REG8 cValue)
{
	CTCCH	*pCh;

	/* TRACEOUT(("ctc - %.4x %.2x [%.4x]", port, value, Z80_PC)); */
	pCh = getctcch(uPort);
	if (pCh != NULL)
	{
		ctcch_o(pCh, uPort, cValue);
	}
}

REG8 IOINPCALL ctc_i(UINT uPort)
{
	CTCCH	*pCh;

	pCh = getctcch(uPort);
	if (pCh != NULL)
	{
		return ctcch_i(pCh, uPort);
	}
	else
	{
		return 0xff;
	}
}


/* reset */

void ctc_reset(void)
{
	UINT	i;
	UINT	j;

	ZeroMemory(&ctc, sizeof(ctc));
	for (i=0; i<3; i++)
	{
		ctc.ch[i].s.num = (UINT8)i;
		for (j=0; j<4; j++)
		{
			ctc.ch[i].s.cmd[j] = 0x23;
			ctc.ch[i].s.scale[j] = 7;
			ctc.ch[i].s.count[j] = 256 << 7;
			ctc.ch[i].s.countmax[j] = 256 << 7;
		}
	}
}

