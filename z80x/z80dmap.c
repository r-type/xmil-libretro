#include	"compiler.h"
#include	"z80core.h"
#include	"pccore.h"
#include	"iocore.h"


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

