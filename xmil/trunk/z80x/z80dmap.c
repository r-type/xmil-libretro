#include	"compiler.h"
#include	"z80core.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"ievent.h"


void z80dmap(void) {

	DMACNT	*cnt1;
	DMACNT	*cnt2;
	UINT	addr;
	REG8	dat;
	REG8	flag1;
	REG8	flag2;

#if !defined(DMAS_STOIC)
	if (!dma.working)
#else
	if (!(dma.flag & DMAF_WORKING))
#endif
	{
		return;
	}

	if (dma.WR0 & 4) {
		cnt1 = &dma.cnt_a;
		cnt2 = &dma.cnt_b;
	}
	else {
		cnt2 = &dma.cnt_a;
		cnt1 = &dma.cnt_b;
	}

	flag1 = cnt1->b.flag;
	flag2 = cnt2->b.flag;
	do {		// dma_lp
		CPU_REMCLOCK -= 6;
#if !defined(DMAS_STOIC)
		if (dma.increment)
#else
		if (dma.flag & DMAF_INCREMENT)
#endif
		{
			if (!(flag1 & 0x20)) {
				cnt1->w.addr += (flag1 & 0x10)?1:-1;
			}
			if (!(flag2 & 0x20)) {
				cnt2->w.addr += (flag2 & 0x10)?1:-1;
			}
		}
		else {
#if !defined(DMAS_STOIC)
			dma.increment = 1;
#else
			dma.flag |= DMAF_INCREMENT;
#endif
		}
		addr = cnt1->w.addr;
		if (flag1 & 8) {
			dat = iocore_inp(addr);
			TRACEOUT(("dma r %.4x - %.2x", addr, dat));
		}
		else {
			dat = z80mem_read8(addr);
		}
		if (dma.WR0 & 2) {
			if (!((dat ^ dma.MACH_BYT) & (~dma.MASK_BYT))) {
#if !defined(DMAS_STOIC)
				dma.working = FALSE;
				dma.MACH_FLG = 1;
#else
				dma.flag &= ~(DMAF_WORKING | DMAF_MACH);
#endif
			}
		}
		if (dma.WR0 & 1) {
			addr = cnt2->w.addr;
			if (flag2 & 8) {
				iocore_out(addr, dat);
			}
			else {
				z80mem_write8(addr, dat);
				TRACEOUT(("dma w %.4x - %.2x", addr, dat));
			}
		}

		dma.leng.w.n++;
		if (dma.leng.w.n == 0) {
#if !defined(DMAS_STOIC)
			dma.working = FALSE;
			dma.ENDB_FLG = 1;
#else
			dma.flag &= ~(DMAF_WORKING | DMAF_ENDB);
#endif
		}
		else if ((dma.leng.w.l) && ((dma.leng.w.n - 1) >= dma.leng.w.l)) {
#if !defined(DMAS_STOIC)
			dma.working = FALSE;
			dma.ENDB_FLG = 1;
#else
			dma.flag &= ~(DMAF_WORKING | DMAF_ENDB);
#endif
		}
#if !defined(DMAS_STOIC)
		if (!dma.working)
#else
		if (!(dma.flag & DMAF_WORKING))
#endif
		{
			goto intr;
		}
	} while(dma.mode);
	return;

intr:
	if (dma.INT_ENBL) {
		ievent_set(IEVENT_DMA);
	}
}

