#include	"compiler.h"
#include	"pccore.h"
#include	"timing.h"
#include	"fdd_mtr.h"


#define	MSSHIFT		16

typedef struct {
	UINT32	tick;
	UINT32	msstep;
	UINT	cnt;
	UINT32	fraction;
} TIMING;

static	TIMING	timing;


void timing_reset(void) {

	timing.tick = GETTICK();
	timing.cnt = 0;
	timing.fraction = 0;
}

void timing_setrate(UINT32 clock) {

	/* timing.msstep = (1 << MSSHIFT) / (clock / 4000) */;
	timing.msstep = (4000 << MSSHIFT) / clock;
}

void timing_setcount(UINT value) {

	timing.cnt = value;
}

UINT timing_getcount(void) {

	UINT32	ticknow;
	UINT32	span;
	UINT32	fraction;

	ticknow = GETTICK();
	span = ticknow - timing.tick;
	if (span) {
		timing.tick = ticknow;
		fddmtr_callback(ticknow);

		if (span >= 1000) {
			span = 1000;
		}
		fraction = timing.fraction + (span * timing.msstep);
		timing.cnt += fraction >> MSSHIFT;
		timing.fraction = fraction & ((1 << MSSHIFT) - 1);
	}
	return(timing.cnt);
}

