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

	DMAC	*d;
	REG8	working;

	d = &dma;
	if (!ready) {
#if !defined(DMAS_STOIC)
		d->working = FALSE;
#else
		d->flag &= ~DMAF_WORKING;
#endif
		d->ready = 8;
	}
	else {
		d->ready = 0;
		working = iswork(&dma);
#if !defined(DMAS_STOIC)
		if (d->working != working) {
			d->working = working;
			nevent_forceexit();
		}
#else
		if ((d->flag ^ working) & DMAF_WORKING) {
			d->flag ^= DMAF_WORKING;
			nevent_forceexit();
		}
#endif
	}
}


BRESULT ieitem_dmac(UINT id) {

	DMAC	*d;
	REG8	vect;

	d = &dma;
	if (d->INT_ENBL) {
		vect = 0;
#if !defined(DMAS_STOIC)
		if ((d->INT_FLG & 1) && (d->MACH_FLG)) {
			vect = 2;
		}
		else if ((d->INT_FLG & 2) && (d->ENDB_FLG)) {
			vect = 4;
		}
#else
		if ((d->INT_FLG & 1) && (!(d->flag & DMAF_MACH))) {
			vect = 2;
		}
		else if ((d->INT_FLG & 2) && (!(d->flag & DMAF_ENDB))) {
			vect = 4;
		}
#endif
		if (vect) {
			if (d->INT_FLG & 0x20) {
				vect += (d->INT_VCT & 0xf9);
			}
			else {
				vect = d->INT_VCT;
			}
			Z80_INTERRUPT(vect);
			return(TRUE);
		}
	}
	(void)id;
	return(FALSE);
}


/* ---- */

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

static void enable(DMAC *d) {

	REG8	working;

	d->enable = 1;
	working = iswork(d);
#if !defined(DMAS_STOIC)
	if (d->working != working) {
		d->working = working;
		if (d->mode == 1) {
			z80dmap();
		}
		else {
			nevent_forceexit();
		}
	}
#else
	if ((d->flag ^ working) & DMAF_WORKING) {
		d->flag ^= DMAF_WORKING;
		if (d->mode == 1) {
			z80dmap();
		}
		else {
			nevent_forceexit();
		}
	}
#endif
}

static void disable(DMAC *d) {

	if (!d->enable)
	{
		d->enable = 0;
#if !defined(DMAS_STOIC)
		d->working = 0;
#else
		d->flag &= ~DMAF_WORKING;
#endif
	}
}

void IOOUTCALL dmac_o(UINT port, REG8 value) {

	DMAC	*d;
	REG8	cmd;
	REG8	mode;

	/* TRACEOUT(("out %.4x %.2x", port, value)); */

	d = &dma;
	if (!d->wcnt) {
		disable(d);
		d->wptr = 0;
		if (!(value & 0x80)) {
			if ((value & 3) != 0) {
				d->WR0 = value;
				if (value & 0x08) {
					d->wtbl[d->wcnt++] = offsetof(DMAC, addr.b.al);
				}
				if (value & 0x10) {
					d->wtbl[d->wcnt++] = offsetof(DMAC, addr.b.ah);
				}
				if (value & 0x20) {
					d->wtbl[d->wcnt++] = offsetof(DMAC, leng.b.ll);
				}
				if (value & 0x40) {
					d->wtbl[d->wcnt++] = offsetof(DMAC, leng.b.lh);
				}
			}
			else {
				if (value & 4) {
					d->cnt_a.b.flag = value;
				}
				else {
					d->cnt_b.b.flag = value;
				}
				if (value & 0x40) {
					d->wtbl[d->wcnt++] = offsetof(DMAC, dummydat);
				}
			}
		}
		else {
			cmd = value & 3;
			if (cmd == 0) {
				/* d->WR3 = value; */
				if (value & 0x08) {
					d->wtbl[d->wcnt++] = offsetof(DMAC, MASK_BYT);
				}
				if (value & 0x10) {
					d->wtbl[d->wcnt++] = offsetof(DMAC, MACH_BYT);
				}
				d->INT_ENBL = (UINT8)((value & 0x20)?1:0);
				if (value & 0x40)
				{
					enable(d);
				}
			}
			else if (cmd == 1) {
				mode = (REG8)(value & (3 << 5));
				if (mode != (3 << 5)) {
					d->WR4 = value;
					d->mode = (UINT8)(mode >> 5);
					if (value & 0x04) {
						d->wtbl[d->wcnt++] = offsetof(DMAC, addr.b.bl);
					}
					if (value & 0x08) {
						d->wtbl[d->wcnt++] = offsetof(DMAC, addr.b.bh);
					}
					if (value & 0x10) {
						d->wtbl[d->wcnt++] = offsetof(DMAC, INT_FLG);
					}
				}
			}
#if 1
			else if (cmd == 2) {
				if (!(value & 0x44)) {
					d->WR5 = value;
				}
			}
#else
			else if (((value & 7) == 2) && (!(value & 0x40))) {
				d->WR5 = value;
			}
#endif
			else if (cmd == 3) {
				switch(DMACMD(value)) {
					case DMACMD(0x83):		/* dma disable */
						break;

					case DMACMD(0x87):		/* dma enable */
#if !defined(DMAS_STOIC)
						d->increment = 0;
#else
						d->flag &= ~DMAF_INCREMENT;
#endif
						enable(d);
						break;

					case DMACMD(0x8b):		/* re-init status byte */
#if !defined(DMAS_STOIC)
						d->MACH_FLG = 0;
						d->ENDB_FLG = 0;
#else
						d->flag |= DMAF_MACH | DMAF_ENDB;
#endif
						break;

					case DMACMD(0xa7):		/* イニシエイトリードシーケンス */
						setdmareaddat(d);
						break;

					case DMACMD(0xab):		/* interrupt enable */
						d->INT_ENBL = 1;
						break;

					case DMACMD(0xaf):		/* interrupt disable */
						d->INT_ENBL = 0;
						break;

					case DMACMD(0xb3):		/* force ready */
						d->ready = (d->WR5 & 0x08);
						break;

					case DMACMD(0xbb):		/* read mask follows */
						d->wtbl[d->wcnt++] = offsetof(DMAC, RR_MSK);
						break;

					case DMACMD(0xbf):		/* read status byte */
						d->RR_MSK = 1;
						setdmareaddat(d);
						break;

					case DMACMD(0xc3):		/* reset */
											/* ローグアライアンス */	/* ver0.25 */
						d->WR0 &= ~3;		/* 0でいいと思うケド… */
#if !defined(DMAS_STOIC)
						d->increment = 0;
#else
						d->flag &= ~DMAF_INCREMENT;
#endif
						/* d->enable = 0; */
						d->INT_ENBL = 0;
						break;

					case DMACMD(0xc7):				/* リセットタイミングA */
					case DMACMD(0xcb):				/* リセットタイミングB */
						break;

					case DMACMD(0xcf):				/* ロード */
						/* d->mode = (UINT8)((d->WR4 >> 5) & 3); */
						d->cnt_a.w.addr = d->addr.w.a;
						d->cnt_b.w.addr = d->addr.w.b;
						d->leng.w.n = 0;
#if !defined(DMAS_STOIC)
						d->MACH_FLG = 0;
						d->ENDB_FLG = 0;
#else
						d->flag |= DMAF_MACH | DMAF_ENDB;
#endif
						/* d->enable = 0; */
						break;

					case DMACMD(0xd3):				/* コンティニュー */
#if !defined(DMAS_STOIC)
						if (d->increment) {
							d->increment = 0;
							switch(d->cnt_a.b.flag & 0x30) {
								case 0x00:
									d->cnt_a.w.addr--;
									break;

								case 0x10:
									d->cnt_a.w.addr++;
									break;
							}
							switch(d->cnt_b.b.flag & 0x30) {
								case 0x00:
									d->cnt_b.w.addr--;
									break;

								case 0x10:
									d->cnt_b.w.addr++;
									break;
							}
						}
#else
						if (d->flag & DMAF_INCREMENT) {
							d->flag &= ~DMAF_INCREMENT;
							switch(d->cnt_a.b.flag & 0x30) {
								case 0x00:
									d->cnt_a.w.addr--;
									break;

								case 0x10:
									d->cnt_a.w.addr++;
									break;
							}
							switch(d->cnt_b.b.flag & 0x30) {
								case 0x00:
									d->cnt_b.w.addr--;
									break;

								case 0x10:
									d->cnt_b.w.addr++;
									break;
							}
						}
#endif
#if !defined(DMAS_STOIC)
						d->MACH_FLG = 0;
						d->ENDB_FLG = 0;
#else
						d->flag |= DMAF_MACH | DMAF_ENDB;
#endif
						d->leng.w.n = 0;
						enable(d);
						break;
				}
			}
		}
	}
	else {
		*(((UINT8 *)d) + d->wtbl[d->wptr]) = value;
		if (d->wtbl[d->wptr] == offsetof(DMAC, INT_FLG)) {
			if (value & 0x08) {
				d->wtbl[d->wptr + d->wcnt] = offsetof(DMAC, INT_PLS);
				d->wcnt++;
			}
			if (value & 0x10) {
				d->wtbl[d->wptr + d->wcnt] = offsetof(DMAC, INT_VCT);
				d->wcnt++;
			}
		}
		else if (d->wtbl[d->wptr] == offsetof(DMAC, RR_MSK)) {
			setdmareaddat(d);
		}
		d->wptr++;
		d->wcnt--;
	}
	(void)port;
}

REG8 IOINPCALL dmac_i(UINT port) {

	DMAC	*d;
	REG8	ret;

	d = &dma;
	ret = 0xcc;
	if (d->enable) {
		ret |= 0x01;
	}
	if ((d->mode != 1) && ((d->WR5 ^ d->ready) & 8)) {
		ret |= 0x02;
	}
#if !defined(DMAS_STOIC)
	if (!d->MACH_FLG) {
		ret |= 0x10;
	}
	if (!d->ENDB_FLG) {
		ret |= 0x20;
	}
#else
	ret |= (d->flag & (DMAF_MACH | DMAF_ENDB));
#endif
	d->RR = ret;
	if (d->rcnt) {
		if (d->rptr >= d->rcnt) {
			d->rptr = 0;
		}
		ret = (*(((UINT8 *)d) + d->rtbl[d->rptr++]));
	}
	(void)port;
	/* TRACEOUT(("inp %.4x %.2x", port, ret)); */
	return(ret);
}


/* reset */

void dmac_reset(void) {

	DMAC *d;

	d = &dma;
	ZeroMemory(d, sizeof(*d));
#if defined(DMAS_STOIC)
	d->flag = DMAF_MACH | DMAF_ENDB;
#endif
	d->ready = 8;
	d->RR = 0x38;
}

