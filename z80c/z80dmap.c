#include	"compiler.h"
#include	"z80core.h"
#include	"pccore.h"
#include	"iocore.h"


void z80dmap(void) {

	REG8	r;
	UINT16	*off1;
	UINT16	*off2;
	REG8	flag1;
	REG8	flag2;
	UINT	addr;
	REG8	dat;
	REG8	vect;

	r = dma.DMA_CMND;
	if ((r & 3) == 0) return;
	if (dma.DMA_ENBL == 0) return;
	if (dma.ENDB_FLG != 0) return;
	if (r & 2) {
		if (dma.MACH_FLG != 0) return;
	}
	if (dma.DMA_MODE != 1) {
		if ((dma.WR[5] ^ dma.DMA_REDY) & 8) return;
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
		if (dma.ENDB_FLG) {
			break;
		}
		if ((dma.DMA_CMND & 2) && (dma.MACH_FLG)) {
			break;
		}
		addr = *off1;
		if (flag1 & 8) {
			if (addr == 0x0ffb) {
				fdcdummyread = 0;
			}
			dat = iocore_inp(addr);
		}
		else {
			dat = mem_read8(addr);
		}
		if (dma.DMA_CMND & 1) {
			addr = *off2;
			if (flag2 & 8) {
				if ((addr == 0x0ffb) && (!ppi.IO_MODE)) {
					fdcdummyread = 0;
				}
				iocore_out(addr, dat);
			}
			else {
				mem_write8(addr, dat);
			}
		}
		if (dma.DMA_CMND & 2) {
			if (!((dat ^ dma.MACH_BYT) & (~dma.MASK_BYT))) {
				dma.MACH_FLG = 1;
			}
		}
		if (dma.DMA_MODE != 1) {
			dma.DMA_STOP = (dma.WR[5] ^ dma.DMA_REDY) & 8;
			if (dma.DMA_STOP) {
				goto dma_stop;
			}
		}
		if (!(flag1 & 0x20)) {
			*off1 += (flag1 & 0x10)?1:-1;
		}
		if (!(flag2 & 0x20)) {
			*off2 += (flag2 & 0x10)?1:-1;
		}

dma_stop:
		dma.BYT_N.w++;
		if (dma.BYT_N.w == 0) {
			dma.ENDB_FLG = 1;
			break;
		}
		if ((dma.BYT_L.w) && (dma.BYT_L.w != 0xffff) && (dma.BYT_N.w >= (dma.BYT_L.w + 1))) {
			dma.ENDB_FLG = 1;
			break;
		}
	} while(dma.DMA_MODE);

	if (dma.INT_ENBL) {
		vect = 0;
		if ((dma.INT_FLG & 1) && (dma.MACH_FLG)) {
			vect = 2;
		}
		else if ((dma.INT_FLG & 2) && (dma.ENDB_FLG)) {
			vect = 4;
		}
		if (vect) {
			if (dma.INT_FLG & 0x20) {
				vect += (dma.INT_VCT & 0xf9);
			}
			else {
				vect = dma.INT_VCT;
			}
			z80c_interrupt(vect);
		}
	}
}

