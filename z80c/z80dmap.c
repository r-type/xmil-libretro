#include	"compiler.h"
#include	"z80core.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"ievent.h"


void z80dmap(void) {

	UINT16	*off1;
	UINT16	*off2;
	REG8	flag1;
	REG8	flag2;
	UINT	addr;
	REG8	dat;

	if (!dma.working) {
		return;
	}

	if (dma.WR[0] & 4) {
		off1 = &dma.CNT_A.w;
		flag1 = dma.WR[1];
		off2 = &dma.CNT_B.w;
		flag2 = dma.WR[2];
	}
	else {
		off2 = &dma.CNT_A.w;
		flag2 = dma.WR[1];
		off1 = &dma.CNT_B.w;
		flag1 = dma.WR[2];
	}

	do {		// dma_lp
		if (dma.increment) {
			if (!(flag1 & 0x20)) {
				*off1 += (flag1 & 0x10)?1:-1;
			}
			if (!(flag2 & 0x20)) {
				*off2 += (flag2 & 0x10)?1:-1;
			}
		}
		dma.increment = 1;
		addr = *off1;
		if (flag1 & 8) {
			dat = iocore_inp(addr);
		}
		else {
			dat = z80mem_read8(addr);
		}
		if (dma.cmd & 1) {
			addr = *off2;
			if (flag2 & 8) {
				iocore_out(addr, dat);
			}
			else {
				z80mem_write8(addr, dat);
			}
		}
		if (dma.cmd & 2) {
			if (!((dat ^ dma.MACH_BYT) & (~dma.MASK_BYT))) {
				dma.working = FALSE;
				dma.MACH_FLG = 1;
			}
		}

		dma.BYT_N.w++;
		if (dma.BYT_N.w == 0) {
			dma.working = FALSE;
			dma.ENDB_FLG = 1;
			goto intr;
		}
		if ((dma.BYT_L.w) && (dma.BYT_L.w != 0xffff) && (dma.BYT_N.w >= (dma.BYT_L.w + 1))) {
			dma.working = FALSE;
			dma.ENDB_FLG = 1;
			goto intr;
		}
		if (!dma.working) {
			return;
		}
	} while(dma.mode);
	return;

intr:
	if (dma.INT_ENBL) {
		ievent_set(IEVENT_DMA);
	}
}

