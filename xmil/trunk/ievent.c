#include	"compiler.h"
#include	"z80core.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"nevent.h"
#include	"ievent.h"


	IEVENT	ievent;


typedef BRESULT (*IEVENTFN)(UINT id);

static BRESULT dummy(UINT id) {

	(void)id;
	return(FALSE);
}

static const IEVENTFN ieventfn[IEVENT_MAX] = {
				dummy,			// IEVENT_SIO
				dummy,			// IEVENT_DMA
				ieitem_ctc,		// IEVENT_CTC0
				ieitem_ctc,		// IEVENT_CTC1
				ieitem_ctc,		// IEVENT_CTC2
				ieitem_scpu};	// IEVENT_SUBCPU


// ----

void ievent_reset(void) {
}

void ievent_progress(void) {

	UINT	i;
	UINT	bit;

	if ((CPU_REQIRQ == 0) || (!Z80_ABLEINTERRUPT())) {
		return;
	}
	for (i=0, bit=1; i<IEVENT_MAX; i++, bit<<=1) {
		if (CPU_REQIRQ & bit) {
			CPU_REQIRQ ^= bit;
			if (ieventfn[i](i)) {
				return;
			}
		}
	}
}

void ievent_setbit(UINT bit) {

	if (CPU_REQIRQ & bit) {
		return;
	}
	CPU_REQIRQ |= bit;
	if (Z80_ABLEINTERRUPT()) {
		nevent_forceexit();
	}
}

