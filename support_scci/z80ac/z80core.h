/* -----------------------------------------------------------------------
 *
 * Z80C : Z80 Engine - GENERIC
 *
 *                              Copyright by Studio Milmake 1999-2000,2004
 *
 *------------------------------------------------------------------------ */

#ifndef CPUCALL
#define	CPUCALL
#endif

enum {
	S_FLAG		= 0x80,
	Z_FLAG		= 0x40,
	H_FLAG		= 0x10,
	V_FLAG		= 0x04,
	N_FLAG		= 0x02,
	C_FLAG		= 0x01
};

enum {
	IFF_IFLAG	= 0,
	IFF_NMI		= 1,
	IFF_HALT	= 2
};


#if defined(BYTESEX_LITTLE)
typedef struct {
	UINT8	c;
	UINT8	b;
	UINT8	e;
	UINT8	d;
	UINT8	l;
	UINT8	h;
	UINT8	ixl;
	UINT8	ixh;
	UINT8	iyl;
	UINT8	iyh;
} Z80REG8;
#else
typedef struct {
	UINT8	b;
	UINT8	c;
	UINT8	d;
	UINT8	e;
	UINT8	h;
	UINT8	l;
	UINT8	ixh;
	UINT8	ixl;
	UINT8	iyh;
	UINT8	iyl;
} Z80REG8;
#endif

typedef struct {
	UINT16	bc;
	UINT16	de;
	UINT16	hl;
	UINT16	ix;
	UINT16	iy;
	UINT16	af2;
} Z80REG16;

typedef struct {
	union {
		Z80REG8		b;
		Z80REG16	w;
	}	r;
	UINT16	bc2;
	UINT16	de2;
	UINT16	hl2;
	UINT8	r2;
	UINT8	padding;
	UINT32	irq;
	UINT32	reqirq;
	UINT32	padding2;
	UINT32	clock;
	SINT32	baseclock;
	SINT32	remainclock;
	union {
		struct {
			UINT8	f;
			UINT8	a;
		}		b;
		UINT16	w;
		UINT32	dummy;
	}		af;
	UINT16	sp;
	UINT16	pc;
	UINT8	iff;
	UINT8	i;
	UINT8	im;
	UINT8	r1;
} Z80STAT;

typedef struct {
const UINT8	*memread;
	UINT8	*__memwrite;
} Z80EXT;

typedef struct {
	UINT8	szp[256];
	UINT8	inc[256];
	UINT8	dec[256];
} Z80FLAG;

typedef struct {
	Z80STAT	s;
	Z80EXT	e;
} Z80CORE;


#include	"z80mem.h"
#include	"z80dmap.h"

#ifdef __cplusplus
extern "C" {
#endif

extern	Z80CORE		z80core;
extern	Z80FLAG		z80flag;
extern	UINT8		mainmem[0x10000];

void CPUCALL z80ac_initialize(void);
void CPUCALL z80ac_reset(void);
void CPUCALL z80ac_maketable(void);
void CPUCALL z80ac_interrupt(REG8 irq);
void CPUCALL z80ac_nonmaskedinterrupt(void);
void CPUCALL z80ac_execute(void);
void CPUCALL z80ac_step(void);

#ifdef __cplusplus
}
#endif


#define	Z80_A			z80core.s.af.b.a
#define	Z80_F			z80core.s.af.b.f
#define	Z80_B			z80core.s.r.b.b
#define	Z80_C			z80core.s.r.b.c
#define	Z80_D			z80core.s.r.b.d
#define	Z80_E			z80core.s.r.b.e
#define	Z80_H			z80core.s.r.b.h
#define	Z80_L			z80core.s.r.b.l

#define	Z80_AF			z80core.s.af.w
#define	Z80_BC			z80core.s.r.w.bc
#define	Z80_DE			z80core.s.r.w.de
#define	Z80_HL			z80core.s.r.w.hl
#define	Z80_IX			z80core.s.r.w.ix
#define	Z80_IY			z80core.s.r.w.iy

#define	Z80_PC			z80core.s.pc
#define	Z80_SP			z80core.s.sp
#define	Z80_AF2			z80core.s.r.w.af2
#define	Z80_BC2			z80core.s.bc2
#define	Z80_DE2			z80core.s.de2
#define	Z80_HL2			z80core.s.hl2

#define	Z80_I			z80core.s.i
#define Z80_IM			z80core.s.im
#define Z80_R			z80core.s.r1
#define Z80_R2			z80core.s.r2
#define	Z80_IFF			z80core.s.iff

#define	CPU_IRQ			z80core.s.irq
#define	CPU_REQIRQ		z80core.s.reqirq

#define	CPU_REMCLOCK	z80core.s.remainclock
#define	CPU_BASECLOCK	z80core.s.baseclock
#define	CPU_CLOCK		z80core.s.clock
#define	CPU_CLOCKCOUNT	(CPU_CLOCK + CPU_BASECLOCK - CPU_REMCLOCK)

#define	Z80_DI			((z80core.s.iff & 3) != 0)
#define	Z80_EI			((z80core.s.iff & 3) == 0)

#define	Z80_INITIALIZE			z80ac_initialize
#define	Z80_DEINITIALIZE()		
#define	Z80_RESET				z80ac_reset
#define	Z80_INTERRUPT(a)		z80ac_interrupt(a)
#define	Z80_NMI					z80ac_nonmaskedinterrupt
#define	Z80_EXECUTE				z80ac_execute
#define	Z80_STEP				z80ac_step

