
#ifndef IOOUTCALL
#define	IOOUTCALL
#endif
#ifndef IOINPCALL
#define	IOINPCALL
#endif

typedef	void (IOOUTCALL *IOOUT)(UINT port, REG8 dat);
typedef	REG8 (IOINPCALL *IOINP)(UINT port);

typedef struct {
	UINT8	mode;
	UINT8	ppib;
#if defined(SUPPORT_BANKMEM)
	UINT8	bankmem;
#else
	UINT8	__bankmem;			/* protect */
#endif
	UINT8	padding;
} IOSTAT;

typedef struct {
	IOINP	inpfn[0x40];
	IOOUT	outfn[0x40];
} IOFNTBL;

typedef struct {
	SINT32	framestartclock;
	SINT32	dispclock;
	SINT32	vsyncstart;
#if !defined(MAINFRAMES_OLD)
	SINT32	vsyncend;
#else
	SINT32	vpulseclock;
#endif
} IOEXT;

typedef struct {
	IOFNTBL	f;
	IOSTAT	s;
	IOEXT	e;
} IOCORE;


#include	"cgrom.h"
#include	"cmt.h"
#include	"crtc.h"
#include	"ctc.h"
#include	"dipsw.h"
#include	"dmac.h"
#include	"fdc.h"
#include	"memio.h"
#include	"pcg.h"
#include	"ppi.h"
#include	"sio.h"
#include	"sndboard.h"
#include	"subcpu.h"
#include	"vramio.h"


#ifdef __cplusplus
extern "C" {
#endif

extern	IOCORE		iocore;
extern	CGROM		cgrom;
extern	CMT			cmt;
extern	CRTC		crtc;
extern	CTC			ctc;
extern	DMAC		dma;
extern	FDC			fdc;
extern	PCG			pcg;
extern	PPI			ppi;
extern	SIO			sio;
extern	SNDBOARD	sndboard;
extern	SUBCPU		subcpu;


void iocore_reset(void);

void IOOUTCALL iocore_out(UINT port, REG8 dat);
REG8 IOINPCALL iocore_inp(UINT port);

#ifdef __cplusplus
}
#endif

