
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
	UINT8	padding[7];
} IOSTAT;

typedef struct {
	IOINP	inpfn[0x20];
	IOOUT	outfn[0x20];
} IOEXT;

typedef struct {
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
extern	MEMIO		memio;
extern	PCG			pcg;
extern	PPI			ppi;
extern	SIO			sio;
extern	SNDBOARD	sndboard;
extern	SUBCPU		subcpu;


void iocore_reset(void);
void iocore_regoutmsb(UINT msb, IOOUT fn);
void iocore_reginpmsb(UINT msb, IOINP fn);

void iocore_regout(UINT port, IOOUT fn);
void iocore_reginp(UINT port, IOINP fn);

void IOOUTCALL iocore_out(UINT port, REG8 dat);
REG8 IOINPCALL iocore_inp(UINT port);

#ifdef __cplusplus
}
#endif

