#include	"compiler.h"
#include	"pccore.h"
#include	"iocore.h"


static void setdmareaddat(void) {

	UINT	cnt;
	REG8	rrmsk;

	cnt = 0;
	rrmsk = dma.RR_MSK;
	if (rrmsk & 0x01) {
		dma.RR_TBL[cnt++] = offsetof(DMAC, RR);
	}
	if (rrmsk & 0x02) {
		dma.RR_TBL[cnt++] = offsetof(DMAC, BYT_N.b.l);
	}
	if (rrmsk & 0x04) {
		dma.RR_TBL[cnt++] = offsetof(DMAC, BYT_N.b.h);
	}
	if (rrmsk & 0x08) {
		dma.RR_TBL[cnt++] = offsetof(DMAC, CNT_A.b.l);
	}
	if (rrmsk & 0x10) {
		dma.RR_TBL[cnt++] = offsetof(DMAC, CNT_A.b.h);
	}
	if (rrmsk & 0x20) {
		dma.RR_TBL[cnt++] = offsetof(DMAC, CNT_B.b.l);
	}
	if (rrmsk & 0x40) {
		dma.RR_TBL[cnt++] = offsetof(DMAC, CNT_B.b.h);
	}
	dma.RR_CNT = (UINT8)cnt;
	dma.RR_OFF = 0;
}


void IOOUTCALL dmac_o(UINT port, REG8 value) {

	REG8	wr;

	dma.DMA_ENBL = 0;

	if (!dma.WR_CNT) {
		wr = 6;
		dma.WR_CNT = 0;
		dma.WR_OFF = 0;
		if (!(value & 0x80)) {
			if ((value & 3) != 0) {
				wr = 0;
			}
			else if (value & 4) {
				wr = 1;
			}
			else {
				wr = 2;
			}
		}
		else {
			if ((value & 3) == 0) {
				wr = 3;
			}
			else if (((value & 3) == 1) && (value >> 5) != 7) {
				wr = 4;
			}
			else if (((value & 7) == 2) && (!(value & 0x40))) {
				wr = 5;
			}
			else if ((value & 3) == 3) {
				wr = 6;
			}
		}
		dma.WR[wr] = value;
		switch(wr) {
			case 0:
				dma.DMA_CMND = (UINT8)(value & 3);
				if (value & 0x08) {
					dma.WR_TBL[dma.WR_CNT++] = offsetof(DMAC, ADR_A.b.l);
				}
				if (value & 0x10) {
					dma.WR_TBL[dma.WR_CNT++] = offsetof(DMAC, ADR_A.b.h);
				}
				if (value & 0x20) {
					dma.WR_TBL[dma.WR_CNT++] = offsetof(DMAC, BYT_L.b.l);
				}
				if (value & 0x40) {
					dma.WR_TBL[dma.WR_CNT++] = offsetof(DMAC, BYT_L.b.h);
				}
				break;

			case 1:
			case 2:
				if (value & 0x40) {
					dma.WR_TBL[dma.WR_CNT++] = offsetof(DMAC, dummydat);
				}
				break;

			case 3:
				if (value & 0x08) {
					dma.WR_TBL[dma.WR_CNT++] = offsetof(DMAC, MASK_BYT);
				}
				if (value & 0x10) {
					dma.WR_TBL[dma.WR_CNT++] = offsetof(DMAC, MACH_BYT);
				}
				dma.INT_ENBL = (UINT8)((value & 0x20)?1:0);
				dma.DMA_ENBL = (UINT8)((value & 0x40)?1:0);
				break;

			case 4:
				dma.DMA_MODE = (UINT8)((dma.WR[4] >> 5) & 3);
				if (value & 0x04) {
					dma.WR_TBL[dma.WR_CNT++] = offsetof(DMAC, ADR_B.b.l);
				}
				if (value & 0x08) {
					dma.WR_TBL[dma.WR_CNT++] = offsetof(DMAC, ADR_B.b.h);
				}
				if (value & 0x10){
					dma.WR_TBL[dma.WR_CNT++] = offsetof(DMAC, INT_FLG);
				}
				break;

			case 6:
				switch(value) {
					case 0x83:				// dma disable
						dma.DMA_ENBL = 0;
						break;

					case 0x87:				// dma enable
						dma.DMA_ENBL = 1;
						break;

					case 0x8b:				// re-init status byte
						dma.MACH_FLG = dma.ENDB_FLG = 0;
						break;

					case 0xa7:				// イニシエイトリードシーケンス
						setdmareaddat();
						break;

					case 0xab:				// interrupt enable
						dma.INT_ENBL = 1;
						break;

					case 0xaf:				// interrupt disable
						dma.INT_ENBL = 0;
						break;

					case 0xb3:				// force ready
						dma.DMA_REDY = (dma.WR[5] & 0x08);
						break;

					case 0xbb:				// read mask follows
						dma.WR_TBL[dma.WR_CNT++] = offsetof(DMAC, RR_MSK);
						break;

					case 0xbf:				// read status byte
						dma.RR_MSK = 1;
						setdmareaddat();
						break;

					case 0xc3:				// reset
#if 1										// ローグアライアンス	// ver0.25
						dma.DMA_CMND = 0;
						dma.DMA_ENBL = 0;
						dma.INT_ENBL = 0;
#else
						init_dma();
#endif
						break;

					case 0xc7:						// リセットタイミングA
					case 0xcb:						// リセットタイミングB
						break;

					case 0xcf:						// ロード
						dma.DMA_MODE = (UINT8)((dma.WR[4] >> 5) & 3);
						dma.CNT_A.w = dma.ADR_A.w;
						dma.CNT_B.w = dma.ADR_B.w;
						dma.BYT_N.w = 0;
						dma.ENDB_FLG = 0;
						dma.MACH_FLG = 0;			// 0619
						dma.DMA_ENBL = 0;
						break;

					case 0xd3:						// コンティニュー
						if (dma.DMA_STOP) {			// 前回途中でNOT READY
							dma.DMA_STOP = 0;
							// ここでインクリメントするのはちょい無理が…
							switch(dma.WR[1] & 0x30) {
								case 0x00:
									dma.CNT_A.w--;
									break;

								case 0x10:
									dma.CNT_A.w++;
									break;
							}
							switch(dma.WR[2] & 0x30) {
								case 0x00:
									dma.CNT_B.w--;
									break;

								case 0x10:
									dma.CNT_B.w++;
									break;
							}
						}
						dma.BYT_N.w = 0;			// 0619
						dma.MACH_FLG = 0;			// 0619
						dma.ENDB_FLG = 0;
						dma.DMA_ENBL = 1;
						break;
				}
				break;
		}
	}
	else {
		*(((UINT8 *)&dma) + dma.WR_TBL[dma.WR_OFF]) = value;
		if (dma.WR_TBL[dma.WR_OFF] == offsetof(DMAC, INT_FLG)) {
			if (value & 0x08) {
				dma.WR_TBL[dma.WR_OFF + dma.WR_CNT] = offsetof(DMAC, INT_PLS);
				dma.WR_CNT++;
			}
			if (value & 0x10) {
				dma.WR_TBL[dma.WR_OFF + dma.WR_CNT] = offsetof(DMAC, INT_VCT);
				dma.WR_CNT++;
			}
		}
		else if (dma.WR_TBL[dma.WR_OFF] == offsetof(DMAC, RR_MSK)) {
			setdmareaddat();
		}
		dma.WR_OFF++;
		dma.WR_CNT--;
	}
	(void)port;
}

REG8 IOINPCALL dmac_i(UINT port) {

	REG8	ret;

	ret = 0xcc;
	if (dma.DMA_ENBL) {
		ret |= 0x01;
	}
	if ((dma.DMA_MODE != 1) && ((dma.WR[5] ^ dma.DMA_REDY) & 8)) {
		ret |= 0x02;
	}
	if (!dma.MACH_FLG) {
		ret |= 0x10;
	}
	if (!dma.ENDB_FLG) {
		ret |= 0x20;
	}
	dma.RR = ret;
	if (dma.RR_CNT) {
		if (dma.RR_OFF >= dma.RR_CNT) {
			dma.RR_OFF = 0;
		}
		ret = (*(((UINT8 *)&dma) + dma.RR_TBL[dma.RR_OFF++]));
	}
	(void)port;
	return(ret);
}


// ----

void dmac_reset(void) {

	ZeroMemory(&dma, sizeof(dma));
	dma.DMA_REDY = 8;
	dma.RR = 0x38;
}

