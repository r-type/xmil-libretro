/* -----------------------------------------------------------------------
 *
 * Z80C : Z80 Engine - GENERIC
 *
 *                              Copyright by Studio Milmake 1999-2000,2004
 *
 *------------------------------------------------------------------------ */

extern UINT8 z80inc_flag2[256];
extern UINT8 z80dec_flag2[256];
extern UINT8 z80szc_flag[512];
extern UINT8 z80szp_flag[256];

extern const UINT8 cycles_main[256];
extern const UINT8 cycles_xx[256];
extern const UINT8 cycles_ed[256];


#define	Z80FN	static void
#define Z80EXT	void

typedef void (*Z80OP)(void);

extern const Z80OP z80c_mainop[256];
extern void z80c_cb(void);
extern void z80c_ix(void);
extern void z80c_sub(void);
extern void z80c_iy(void);
extern void z80c_ixcb(void);
extern void z80c_iycb(void);


#define	R_Z80A		z80core.s.r.b.a
#define	R_Z80F		z80core.s.r.b.f
#define	R_Z80B		z80core.s.r.b.b
#define	R_Z80C		z80core.s.r.b.c
#define	R_Z80D		z80core.s.r.b.d
#define	R_Z80E		z80core.s.r.b.e
#define	R_Z80H		z80core.s.r.b.h
#define	R_Z80L		z80core.s.r.b.l

#define	R_Z80AF		z80core.s.r.w.af
#define	R_Z80BC		z80core.s.r.w.bc
#define	R_Z80DE		z80core.s.r.w.de
#define	R_Z80HL		z80core.s.r.w.hl
#define	R_Z80IX		z80core.s.r.w.ix
#define	R_Z80IY		z80core.s.r.w.iy

#define	R_Z80PC		z80core.s.pc
#define	R_Z80SP		z80core.s.sp
#define	R_Z80AF2	z80core.s.af2
#define	R_Z80BC2	z80core.s.bc2
#define	R_Z80DE2	z80core.s.de2
#define	R_Z80HL2	z80core.s.hl2

#define	R_Z80I		z80core.s.i
#define R_Z80IM		z80core.s.im
#define R_Z80R		z80core.s.r1
#define R_Z80R2		z80core.s.r2
#define	R_Z80IFF	z80core.s.iff

