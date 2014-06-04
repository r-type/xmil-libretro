#include "compiler.h"
#include "libnds.h"
#include "z80core.h"
#include "pccore.h"
#include "ipcxfer.h"
#include "nds9psg.h"


#if !defined(USE_ARMROUTINE)
void nds9psg_setreg(PNDS9PSG pPsg, REG8 cReg, REG8 cValue)
{
	if (cReg >= 14)
	{
		return;
	}

	if (cReg == 13)
	{
		pPsg->reg.b[cReg] = cValue;
		pPsg->cUpdateEnv = 1;
	}
	else if (pPsg->reg.b[cReg] != cValue)
	{
		pPsg->reg.b[cReg] = cValue;
		pPsg->cUpdateReg = 1;
	}
}

REG8 nds9psg_getreg(PNDS9PSG pPsg, REG8 cReg)
{
	return pPsg->reg.b[cReg & 15];
}
#endif	// !defined(USE_ARMROUTINE)


// ----

void nds9psg_reset(PNDS9PSG pPsg)
{
	ZeroMemory(pPsg, sizeof(*pPsg));
	pPsg->reg.r.cMixer = 0x3f;
	pPsg->reg.r.cIo1 = 0xff;
	pPsg->reg.r.cIo2 = 0xff;
	pPsg->uClock = CPU_CLOCK;
}

void nds9psg_sync(PNDS9PSG pPsg)
{
	const UINT uClock = CPU_CLOCK + CPU_BASECLOCK - CPU_REMCLOCK;
	const UINT uPast = uClock - pPsg->uClock;
	if (uPast)
	{
		PNDSPSGXFER pXfer = IPCPSGXFER;
		if (!pXfer->uClock)
		{
			CopyMemory(pXfer->reg.b, pPsg->reg.b, sizeof(pXfer->reg));
			pXfer->cUpdateReg = pPsg->cUpdateReg;
			pXfer->cUpdateEnv = pPsg->cUpdateEnv;
			pXfer->uClock = uPast;

			pPsg->uClock = uClock;
			pPsg->cUpdateReg = 0;
			pPsg->cUpdateEnv = 0;
		}
	}
}

