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





#if 0	// old
LABEL void z80dmap(void) {

	__asm {
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

dmaintmain:		push	ebx
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
				push	edx
				movzx	ecx, word ptr [ebx]
				test	dl, 8
				jne		dma_inport
				call	memrd8_ecx_al
dmasrcend:		pop		edx
				pop		ecx

				test	dma.DMA_CMND, 1
				je		dmadstend

				push	eax
				push	ecx
				push	edx
				movzx	ecx, word ptr [ecx]
				mov		dl, al
				test	dh, 8
				jne		dma_outport
				call	memwr8_ecx_dl
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
				je		dmplpedch				// BYT_L == 0Ç»ÇÁñ≥êßå¿?
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
dma_intrpt:		jmp		z80x_interrupt
dmaintrptend:	ret


dma_inport:		call	iocore_inp
				jmp		dmasrcend

dma_outport:	call	iocore_out
				jmp		dmadstend
	}
}
#endif

