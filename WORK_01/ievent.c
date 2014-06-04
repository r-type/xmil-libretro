#include	"compiler.h"
#include	"z80core.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"nevent.h"
#include	"ievent.h"


/* ここでデイジーチェイン */


typedef BRESULT (*IEVENTFN)(UINT id);

static BRESULT dummy(UINT id) {

	(void)id;
	return(FALSE);
}

static const IEVENTFN ieventfn[IEVENT_MAX] = {
				dummy,			/* IEVENT_SIO    */
				ieitem_dmac,	/* IEVENT_DMA    */
				ieitem_ctc,		/* IEVENT_CTC0   */
				ieitem_ctc,		/* IEVENT_CTC1   */
				ieitem_ctc,		/* IEVENT_CTC2   */
				ieitem_scpu};	/* IEVENT_SUBCPU */


/* ---- */

void ievent_progress(void) {

	UINT	i;
	UINT	bit;

	if ((CPU_REQIRQ == 0) || (Z80_DI)) {
		return;
	}
	for (i=0, bit=1; i<IEVENT_MAX; i++, bit<<=1) {
		if (CPU_IRQ & bit) {
			break;
		}
		if (CPU_REQIRQ & bit) {
			CPU_REQIRQ ^= bit;
			if (ieventfn[i](i)) {
				if (i != IEVENT_SUBCPU) {		/* サブCPUは別処理 */
					CPU_IRQ |= bit;
				}
				return;
			}
		}
	}
}

void ievent_setbit(UINT bit) {

	UINT	r;

	if (CPU_REQIRQ & bit) {
		return;
	}
	CPU_REQIRQ |= bit;
	if (Z80_DI) {
		return;
	}
	r = CPU_IRQ;
	r = (r ^ (r - 1)) >> 1;
	if (!(r & bit)) {
		return;
	}
	nevent_forceexit();
}

void ievent_eoi(void) {

	UINT	i;
	UINT	bit;

	if (CPU_IRQ == 0) {
		return;
	}
	for (i=0, bit=1; i<IEVENT_MAX; i++, bit<<=1) {
		if (CPU_IRQ & bit) {
			CPU_IRQ ^= bit;
			if ((i >= IEVENT_CTC0) && (i <= IEVENT_CTC2)) {
				ieeoi_ctc(i);
			}
			break;
		}
	}
	if ((!(Z80_IFF & ((1 << IFF_IFLAG) | (1 << IFF_NMI)))) &&
		(CPU_REQIRQ != 0)) {
		CPU_BASECLOCK -= CPU_REMCLOCK;
		CPU_REMCLOCK = 0;
	}
}

