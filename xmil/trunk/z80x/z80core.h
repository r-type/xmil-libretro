//----------------------------------------------------------------------------
//
//  Z80X : Z80 Engine  for Intel x86 Protectmode (80386 later)
//
//                                  Copyright by Studio Milmake 1999-2000,2004
//
//----------------------------------------------------------------------------


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
	IFF_IRQ		= 1,
	IFF_NMI		= 2,
	IFF_HALT	= 3
};


#if defined(BYTESEX_LITTLE)
typedef struct {
	UINT8	f;
	UINT8	a;
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
	UINT8	a;
	UINT8	f;
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
	UINT16	af;
	UINT16	bc;
	UINT16	de;
	UINT16	hl;
	UINT16	ix;
	UINT16	iy;
} Z80REG16;

typedef struct {
	union {
		Z80REG8		b;
		Z80REG16	w;
	}	r;
	UINT16	pc;
	UINT16	sp;
	UINT16	af2;
	UINT16	bc2;
	UINT16	de2;
	UINT16	hl2;
	UINT8	i;
	UINT8	im;
	UINT8	r1;
	UINT8	r2;
	UINT16	iff;				// 4byte align
	UINT16	padding1;
	UINT32	icount;
} Z80STAT;

typedef struct {
	UINT	dummy;
} Z80EXT;

typedef struct {
	Z80STAT	s;
	Z80EXT	e;
} Z80CORE;


#include	"z80xmem.h"
#include	"z80xdmap.h"

#ifdef __cplusplus
extern "C" {
#endif

extern	Z80CORE		z80core;

void z80x_initialize(void);
void z80x_reset(void);
void z80x_maketable(void);
REG8 z80x_ableinterrupt(void);
void __fastcall z80x_interrupt(REG8 irq);
void __fastcall z80x_interrupt2(REG8 irq);
void z80x_nonmaskedinterrupt(void);
void z80x_execute(void);

#ifdef __cplusplus
}
#endif


#define	Z80_A		z80core.s.r.b.a
#define	Z80_F		z80core.s.r.b.f
#define	Z80_B		z80core.s.r.b.b
#define	Z80_C		z80core.s.r.b.c
#define	Z80_D		z80core.s.r.b.d
#define	Z80_E		z80core.s.r.b.e
#define	Z80_H		z80core.s.r.b.h
#define	Z80_L		z80core.s.r.b.l

#define	Z80_AF		z80core.s.r.w.af
#define	Z80_BC		z80core.s.r.w.bc
#define	Z80_DE		z80core.s.r.w.de
#define	Z80_HL		z80core.s.r.w.hl
#define	Z80_IX		z80core.s.r.w.ix
#define	Z80_IY		z80core.s.r.w.iy

#define	Z80_PC		z80core.s.pc
#define	Z80_SP		z80core.s.sp
#define	Z80_AF2		z80core.s.af2
#define	Z80_BC2		z80core.s.bc2
#define	Z80_DE2		z80core.s.de2
#define	Z80_HL2		z80core.s.hl2

#define	Z80_I		z80core.s.i
#define Z80_IM		z80core.s.im
#define Z80_R		z80core.s.r1
#define Z80_R2		z80core.s.r2
#define	Z80_IFF		z80core.s.iff
#define	Z80_ICOUNT	z80core.s.icount


#define	Z80_INITIALIZE			z80x_initialize
#define	Z80_DEINITIALIZE()		
#define	Z80_RESET				z80x_reset
#define	Z80_ABLEINTERRUPT		z80x_ableinterrupt
#define	Z80_INT(a)				z80x_interrupt(a)
#define	Z80_INT2(a)				z80x_interrupt2(a)
#define	Z80_NMI					z80x_nonmaskedinterrupt
#define	Z80_EXECUTE				z80x_execute

