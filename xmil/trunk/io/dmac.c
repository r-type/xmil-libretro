#include	"compiler.h"
#include	"z80core.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"nevent.h"
#include	"ievent.h"


#define	DMACMD(a)	(((a) >> 2) & 0x1f)


static REG8 iswork(const DMAC *d) {

	if (d->enable == 0) return(0);
	if (!(d->WR0 & 3)) return(0);
#if !defined(DMAS_STOIC)
	if (d->ENDB_FLG != 0) return(0);
	if ((d->WR0 & 2) && (d->MACH_FLG != 0)) return(0);
#else
	if (!(d->flag & DMAF_ENDB)) return(0);
	if ((d->WR0 & 2) && (!(d->flag & DMAF_MACH))) return(0);
#endif
	if (d->mode != 1) {
		if ((d->WR5 ^ d->ready) & 8) return(0);
	}
	return(1);
}

void dmac_sendready(BRESULT ready) {

	REG8	working;

	if (!ready) {
#if !defined(DMAS_STOIC)
		dma.working = FALSE;
#else
		dma.flag &= ~DMAF_WORKING;
#endif
		dma.ready = 8;
	}
	else {
		dma.ready = 0;
		working = iswork(&dma);
#if !defined(DMAS_STOIC)
		if (dma.working != working) {
			dma.working = working;
			nevent_forceexit();
		}
#else
		if ((dma.flag ^ working) & DMAF_WORKING) {
			dma.flag ^= DMAF_WORKING;
			nevent_forceexit();
		}
#endif
	}
}


BRESULT ieitem_dmac(UINT id) {

	REG8	vect;

	if (dma.INT_ENBL) {
		vect = 0;
#if !defined(DMAS_STOIC)
		if ((dma.INT_FLG & 1) && (dma.MACH_FLG)) {
			vect = 2;
		}
		else if ((dma.INT_FLG & 2) && (dma.ENDB_FLG)) {
			vect = 4;
		}
#else
		if ((dma.INT_FLG & 1) && (!(dma.flag & DMAF_MACH))) {
			vect = 2;
		}
		else if ((dma.INT_FLG & 2) && (!(dma.flag & DMAF_ENDB))) {
			vect = 4;
		}
#endif
		if (vect) {
			if (dma.INT_FLG & 0x20) {
				vect += (dma.INT_VCT & 0xf9);
			}
			else {
				vect = dma.INT_VCT;
			}
			Z80_INTERRUPT(vect);
			return(TRUE);
		}
	}
	(void)id;
	return(FALSE);
}


// ----

static void setdmareaddat(DMAC *d) {

	REG8	rrmsk;
	UINT8	*ptr;

	rrmsk = d->RR_MSK;
	ptr = d->rtbl;
	if (rrmsk & 0x01) {
		*ptr++ = offsetof(DMAC, RR);
	}
	if (rrmsk & 0x02) {
		*ptr++ = offsetof(DMAC, leng.b.nl);
	}
	if (rrmsk & 0x04) {
		*ptr++ = offsetof(DMAC, leng.b.nh);
	}
	if (rrmsk & 0x08) {
		*ptr++ = offsetof(DMAC, cnt_a.b.addrl);
	}
	if (rrmsk & 0x10) {
		*ptr++ = offsetof(DMAC, cnt_a.b.addrh);
	}
	if (rrmsk & 0x20) {
		*ptr++ = offsetof(DMAC, cnt_b.b.addrl);
	}
	if (rrmsk & 0x40) {
		*ptr++ = offsetof(DMAC, cnt_b.b.addrh);
	}
	d->rcnt = (UINT8)(ptr - d->rtbl);
	d->rptr = 0;
}


void IOOUTCALL dmac_o(UINT port, REG8 value) {

	REG8	working;

	TRACEOUT(("out %.4x %.2x", port, value));

	dma.enable = 0;

	if (!dma.wcnt) {
//		dma.wcnt = 0;
		dma.wptr = 0;
		if (!(value & 0x80)) {
			if ((value & 3) != 0) {
				dma.WR0 = value;
				if (value & 0x08) {
					dma.wtbl[dma.wcnt++] = offsetof(DMAC, addr.b.al);
				}
				if (value & 0x10) {
					dma.wtbl[dma.wcnt++] = offsetof(DMAC, addr.b.ah);
				}
				if (value & 0x20) {
					dma.wtbl[dma.wcnt++] = offsetof(DMAC, leng.b.ll);
				}
				if (value & 0x40) {
					dma.wtbl[dma.wcnt++] = offsetof(DMAC, leng.b.lh);
				}
			}
			else {
				if (value & 4) {
					dma.cnt_a.b.flag = value;
				}
				else {
					dma.cnt_b.b.flag = value;
				}
				if (value & 0x40) {
					dma.wtbl[dma.wcnt++] = offsetof(DMAC, dummydat);
				}
			}
		}
		else {
			REG8 cmd;
			cmd = value & 3;
			if (cmd == 0) {
//				dma.WR3 = value;
				if (value & 0x08) {
					dma.wtbl[dma.wcnt++] = offsetof(DMAC, MASK_BYT);
				}
				if (value & 0x10) {
					dma.wtbl[dma.wcnt++] = offsetof(DMAC, MACH_BYT);
				}
				dma.INT_ENBL = (UINT8)((value & 0x20)?1:0);
				dma.enable = (UINT8)((value & 0x40)?1:0);
			}
			else if (cmd == 1) {
				REG8 mode;
				mode = (REG8)(value & (3 << 5));
				if (mode != (3 << 5)) {
					dma.WR4 = value;
					dma.mode = (UINT8)(mode >> 5);
					if (value & 0x04) {
						dma.wtbl[dma.wcnt++] = offsetof(DMAC, addr.b.bl);
					}
					if (value & 0x08) {
						dma.wtbl[dma.wcnt++] = offsetof(DMAC, addr.b.bh);
					}
					if (value & 0x10) {
						dma.wtbl[dma.wcnt++] = offsetof(DMAC, INT_FLG);
					}
				}
			}
#if 1
			else if (cmd == 2) {
				if (!(value & 0x44)) {
					dma.WR5 = value;
				}
			}
#else
			else if (((value & 7) == 2) && (!(value & 0x40))) {
				dma.WR5 = value;
			}
#endif
			else if (cmd == 3) {
				switch(DMACMD(value)) {
					case DMACMD(0x83):		// dma disable
					//	dma.enable = 0;
						break;

					case DMACMD(0x87):		// dma enable
#if !defined(DMAS_STOIC)
						dma.increment = 0;
#else
						dma.flag &= ~DMAF_INCREMENT;
#endif
						dma.enable = 1;
						break;

					case DMACMD(0x8b):		// re-init status byte
#if !defined(DMAS_STOIC)
						dma.MACH_FLG = 0;
						dma.ENDB_FLG = 0;
#else
						dma.flag |= DMAF_MACH | DMAF_ENDB;
#endif
						break;

					case DMACMD(0xa7):		// イニシエイトリードシーケンス
						setdmareaddat(&dma);
						break;

					case DMACMD(0xab):		// interrupt enable
						dma.INT_ENBL = 1;
						break;

					case DMACMD(0xaf):		// interrupt disable
						dma.INT_ENBL = 0;
						break;

					case DMACMD(0xb3):		// force ready
						dma.ready = (dma.WR5 & 0x08);
						break;

					case DMACMD(0xbb):		// read mask follows
						dma.wtbl[dma.wcnt++] = offsetof(DMAC, RR_MSK);
						break;

					case DMACMD(0xbf):		// read status byte
						dma.RR_MSK = 1;
						setdmareaddat(&dma);
						break;

					case DMACMD(0xc3):		// reset
											// ローグアライアンス	// ver0.25
						dma.WR0 &= ~3;		// 0でいいと思うケド…
#if !defined(DMAS_STOIC)
						dma.increment = 0;
#else
						dma.flag &= ~DMAF_INCREMENT;
#endif
					//	dma.enable = 0;
						dma.INT_ENBL = 0;
						break;

					case DMACMD(0xc7):				// リセットタイミングA
					case DMACMD(0xcb):				// リセットタイミングB
						break;

					case DMACMD(0xcf):				// ロード
//						dma.mode = (UINT8)((dma.WR4 >> 5) & 3);
						dma.cnt_a.w.addr = dma.addr.w.a;
						dma.cnt_b.w.addr = dma.addr.w.b;
						dma.leng.w.n = 0;
#if !defined(DMAS_STOIC)
						dma.MACH_FLG = 0;
						dma.ENDB_FLG = 0;
#else
						dma.flag |= DMAF_MACH | DMAF_ENDB;
#endif
					//	dma.enable = 0;
						break;

					case DMACMD(0xd3):				// コンティニュー
#if !defined(DMAS_STOIC)
						if (dma.increment) {
							dma.increment = 0;
							switch(dma.cnt_a.b.flag & 0x30) {
								case 0x00:
									dma.cnt_a.w.addr--;
									break;

								case 0x10:
									dma.cnt_a.w.addr++;
									break;
							}
							switch(dma.cnt_b.b.flag & 0x30) {
								case 0x00:
									dma.cnt_b.w.addr--;
									break;

								case 0x10:
									dma.cnt_b.w.addr++;
									break;
							}
						}
#else
						if (dma.flag & DMAF_INCREMENT) {
							dma.flag &= ~DMAF_INCREMENT;
							switch(dma.cnt_a.b.flag & 0x30) {
								case 0x00:
									dma.cnt_a.w.addr--;
									break;

								case 0x10:
									dma.cnt_a.w.addr++;
									break;
							}
							switch(dma.cnt_b.b.flag & 0x30) {
								case 0x00:
									dma.cnt_b.w.addr--;
									break;

								case 0x10:
									dma.cnt_b.w.addr++;
									break;
							}
						}
#endif
#if !defined(DMAS_STOIC)
						dma.MACH_FLG = 0;
						dma.ENDB_FLG = 0;
#else
						dma.flag |= DMAF_MACH | DMAF_ENDB;
#endif
						dma.leng.w.n = 0;
						dma.enable = 1;
						break;
				}
			}
		}
	}
	else {
		*(((UINT8 *)&dma) + dma.wtbl[dma.wptr]) = value;
		if (dma.wtbl[dma.wptr] == offsetof(DMAC, INT_FLG)) {
			if (value & 0x08) {
				dma.wtbl[dma.wptr + dma.wcnt] = offsetof(DMAC, INT_PLS);
				dma.wcnt++;
			}
			if (value & 0x10) {
				dma.wtbl[dma.wptr + dma.wcnt] = offsetof(DMAC, INT_VCT);
				dma.wcnt++;
			}
		}
		else if (dma.wtbl[dma.wptr] == offsetof(DMAC, RR_MSK)) {
			setdmareaddat(&dma);
		}
		dma.wptr++;
		dma.wcnt--;
	}

	working = iswork(&dma);
#if !defined(DMAS_STOIC)
	if (dma.working != working) {
		dma.working = working;
		if (working) {
			nevent_forceexit();
		}
	}
#else
	if ((dma.flag ^ working) & DMAF_WORKING) {
		dma.flag ^= DMAF_WORKING;
		if (working) {
			nevent_forceexit();
		}
	}
#endif
	(void)port;
}

REG8 IOINPCALL dmac_i(UINT port) {

	REG8	ret;

	ret = 0xcc;
	if (dma.enable) {
		ret |= 0x01;
	}
	if ((dma.mode != 1) && ((dma.WR5 ^ dma.ready) & 8)) {
		ret |= 0x02;
	}
#if !defined(DMAS_STOIC)
	if (!dma.MACH_FLG) {
		ret |= 0x10;
	}
	if (!dma.ENDB_FLG) {
		ret |= 0x20;
	}
#else
	ret |= (dma.flag & (DMAF_MACH | DMAF_ENDB));
#endif
	dma.RR = ret;
	if (dma.rcnt) {
		if (dma.rptr >= dma.rcnt) {
			dma.rptr = 0;
		}
		ret = (*(((UINT8 *)&dma) + dma.rtbl[dma.rptr++]));
	}
	(void)port;
	TRACEOUT(("inp %.4x %.2x", port, ret));
	return(ret);
}


// ----

void dmac_reset(void) {

	ZeroMemory(&dma, sizeof(dma));
#if defined(DMAS_STOIC)
	dma.flag = DMAF_MACH | DMAF_ENDB;
#endif
	dma.ready = 8;
	dma.RR = 0x38;
}

