
#ifndef IOOUTCALL
#define	IOOUTCALL
#endif
#ifndef IOINPCALL
#define	IOINPCALL
#endif

typedef	void (IOOUTCALL *IOOUT)(UINT port, REG8 val);
typedef	REG8 (IOINPCALL *IOINP)(UINT port);

typedef void (*IOCBFN)(void);

enum {
	IOCORE_OUTHTBL	= 0x01,
	IOCORE_INPHTBL	= 0x02
};

typedef union {
	IOOUT	fn;
	IOOUT	*tbl;
} OUTHTBL;

typedef union {
	IOINP	fn;
	IOINP	*tbl;
} INPHTBL;

typedef struct {
	UINT	mode;
	IOOUT	outv[3];
	IOINP	inpv[3];
	UINT8	iofg[0x20];
	OUTHTBL	outh[0x20];
	INPHTBL	inph[0x20];
} IOCORE;


#include	"cgrom.h"
#include	"cmt.h"
#include	"ctc.h"
#include	"dipsw.h"
#include	"dmac.h"
#include	"fdc.h"
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
extern	CTC			ctc;
extern	DMAC		dma;
extern	FDC			fdc;
extern	PCG			pcg;
extern	PPI			ppi;
extern	SIO			sio;
extern	SNDBOARD	sndboard;
extern	SUBCPU		subcpu;


void iocore_reset(void);
void iocore_regouthigh(UINT port, IOOUT fn);
void iocore_regoutlow(UINT port, IOOUT fn);
void iocore_reginphigh(UINT port, IOINP fn);
void iocore_reginplow(UINT port, IOINP fn);

// void IOOUTCALL iocore_out(UINT port, REG8 dat);
// REG8 IOINPCALL iocore_inp(UINT port);

#ifdef __cplusplus
}
#endif

