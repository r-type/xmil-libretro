#include	"compiler.h"
#include	"z80core.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"x1_io.h"


#if 0
static BOOL _iswork(void) {

	REG8	r;

	r = dma.DMA_CMND;
	if ((r & 3) == 0) return(0);
	if (dma.DMA_ENBL == 0) return(0);
	if (dma.ENDB_FLG != 0) return(0);
	if (r & 2) {
		if (dma.MACH_FLG != 0) return(0);
	}
	if (dma.DMA_MODE != 1) {
		if ((dma.WR[5] ^ dma.DMA_REDY) & 8) return(0);
	}
	return(1);
}

static void _dmap(void) {

	UINT16	*off1;
	UINT16	*off2;
	REG8	flag1;
	REG8	flag2;
	UINT	addr;
	REG8	dat;

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
			dat = Z80_RDMEM((REG16)addr);
		}
TRACEOUT(("DMA IN -> %.4x %.2x", addr, dat));
		if (dma.DMA_CMND & 1) {
			addr = *off2;
			if (flag2 & 8) {
				if ((addr == 0x0ffb) && (!ppi.IO_MODE)) {
					fdcdummyread = 0;
				}
				iocore_out(addr, dat);
			}
			else {
				Z80_WRMEM((REG16)addr, dat);
			}
TRACEOUT(("DMA OUT -> %.4x %.2x", addr, dat));
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
}

static void _intr(void) {

	REG8	vect;

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

LABEL void z80dmap(void) {

	__asm {
#if 1
				call	_iswork
				cmp		al, 0
				jne		short dmaintmain
				ret
#else
				mov		al, dma.DMA_CMND
				test	al, 3
				je		dmaintend
				cmp		dma.DMA_ENBL, 0
				je		dmaintend
				cmp		dma.ENDB_FLG, 0
				jne		dmaintend
				test	al, 2
				je		ckcntmode
				cmp		dma.MACH_FLG, 0
				jne		short dmaintend
ckcntmode:		cmp		dma.DMA_MODE, 1
				je		dmaintmain
				mov		al, dma.WR[5]
				xor		al, dma.DMA_REDY
				test	al, 8
				je		dmaintmain
dmaintend:		ret
#endif

dmaintmain:
#if 1
				call	_dmap
#else
				push	ebx
				mov		ebx, offset dma.CNT_A
				mov		ecx, offset dma.CNT_B

				mov		dx, word ptr (dma.WR[1])
				test	dma.WR[0], 4
				jne		dma_lp
				xchg	ebx, ecx
				xchg	dl, dh

dma_lp:			cmp		dma.ENDB_FLG, 0
				jne		dmalpend
				test	dma.DMA_CMND, 2
				je		dma_lpst
				cmp		dma.MACH_FLG, 0
				jne		dmalpend
dma_lpst:
				push	ecx
				movzx	ecx, word ptr [ebx]
				test	dl, 8
				jne		dma_inport
				push	edx
				call	Z80_RDMEM
				pop		edx
dmasrcend:		pop		ecx

				test	dma.DMA_CMND, 1
				je		dmadstend

				push	eax
				push	ecx
				push	edx
				movzx	ecx, word ptr [ecx]
				mov		dl, al
				test	dh, 8
				jne		dma_outport
				call	Z80_WRMEM
dmadstend:		pop		edx
				pop		ecx
				pop		eax

				test	dma.DMA_CMND, 2
				je		dmamachcend
				xor		al, dma.MACH_BYT
				mov		ah,	dma.MASK_BYT
				not		ah
				and		al, ah
				jne		short dmamachcend
				mov		dma.MACH_FLG, 1
dmamachcend:
				cmp		dma.DMA_MODE, 1
				je		forcesrcaddr
				mov		al, dma.WR[5]
				xor		al, dma.DMA_REDY
				and		al, 8
				mov		dma.DMA_STOP, al
				jne		dmadststpend

forcesrcaddr:	test	dl, 20h
				jne		dmasrcstpend
				test	dl, 10h
				je		dmasrcdec
					inc		word ptr [ebx]
				jmp		dmasrcstpend
dmasrcdec:			dec		word ptr [ebx]
dmasrcstpend:
				test	dh, 20h
				jne		dmadststpend
				test	dh, 10h
				je		dmadstdec
					inc		word ptr [ecx]
				jmp		dmadststpend
dmadstdec:			dec		word ptr [ecx]
dmadststpend:
				inc		dma.BYT_N.w
				je		dmaforceend
				mov		ax, dma.BYT_L.w
				or		ax, ax
				je		dmplpedch				// BYT_L == 0‚È‚ç–³§ŒÀ?
				inc		ax
				je		dmplpedch				// ver0.25
				cmp		ax, dma.BYT_N.w
				ja		dmplpedch
dmaforceend:	mov		dma.ENDB_FLG, 1
				jmp		dmalpend

dmplpedch:		cmp		dma.DMA_MODE, 0
				jne		dma_lp
dmalpend:
				pop		ebx
#endif
#if 1
				jmp		_intr
#else
				cmp		dma.INT_ENBL, 0
				je		dmaintrptend
				mov		dl, 2
				mov		al, dma.INT_FLG
				test	al, 1
				je		dmaintrpt2ck
				cmp		dma.MACH_FLG, 0
				jne		dmaintrptcall
dmaintrpt2ck:	mov		dl, 4
				test	al, 2
				je		dmaintrptend
				cmp		dma.ENDB_FLG, 0
				je		dmaintrptend
dmaintrptcall:	movzx	ecx, dma.INT_VCT
				test	al, 20h
				je		dma_intrpt
				and		cl, 0f9h
				or		cl, dl
dma_intrpt:		jmp		z80c_interrupt
#endif
dmaintrptend:	ret


#if 0
dma_inport:		push	edx
				push	ebx
				cmp		cx, 0ffbh
				jne		Z80inport
				mov		fdcdummyread, 0
Z80inport:		call	iocore_inp
				pop		ebx
				pop		edx
				jmp		dmasrcend

dma_outport:	push	ebx
				cmp		cx, 0ffbh
				jne		Z80outport
				cmp		ppi.IO_MODE, 0
				jne		Z80outport
				mov		fdcdummyread, 0
Z80outport:		call	iocore_out
				pop		ebx
				jmp		dmadstend
#endif
	}
}
#else
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
			dat = Z80_RDMEM((REG16)addr);
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
				Z80_WRMEM((REG16)addr, dat);
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
#endif

