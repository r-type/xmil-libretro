#include	"compiler.h"
#include	"timing.h"
#include	"fdd_mtr.h"


static const UINT8 tick3[3] = {16, 17, 17};

typedef struct {
	UINT32	tick;
	UINT	cnt;
	UINT	fraction;
} TIMING;

static	TIMING	timing;


void timing_reset(void) {

	timing.tick = GETTICK();
	timing.cnt = 0;
	timing.fraction = 0;
}

void timing_setcount(UINT value) {

	timing.cnt = value;
}

UINT timing_getcount(void) {

	UINT32	ticknow;
	UINT32	span;
	UINT32	steps;

	ticknow = GETTICK();
	span = ticknow - timing.tick;
#if 1
	if (span < tick3[timing.fraction]) {
		ticknow = GETTICK();
		span = ticknow - timing.tick;
	}
#endif
	if (span) {
		FDDMTR_CALLBACK(ticknow);
		if (span >= 50) {
			steps = span / 50;
			span %= 50;
			timing.tick += (steps * 50);
			timing.cnt += steps * 3;
		}
		while(span >= tick3[timing.fraction]) {
			span -= tick3[timing.fraction];
			timing.tick += tick3[timing.fraction];
			timing.cnt++;
			timing.fraction++;
			if (timing.fraction >= 3) {
				timing.fraction = 0;
			}
		}
	}
	return(timing.cnt);
}

