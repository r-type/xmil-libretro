#include	"compiler.h"
#include	"timemng.h"
#include	"z80core.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"nevent.h"
#include	"ievent.h"
#include	"keystat.h"
#include	"calendar.h"


void neitem_scpu(UINT id) {

	BRESULT	intr;

	intr = FALSE;
	// こうすると同時押しが判定できないのでキーバッファを持つべし
	if (keystat.req_int) {
		keystat.req_int = 0;
		intr = TRUE;
	}
	else if (subcpu.s.keydata) {
		subcpu.s.keycount++;
		if (subcpu.s.keycount >= subcpu.s.keycountrep) {
			subcpu.s.keycount = 0;
			intr = TRUE;
		}
		nevent_set(NEVENT_SUBCPU, subcpu.e.intrclock,
											neitem_scpu, NEVENT_RELATIVE);
	}
	if (intr) {
		ievent_set(IEVENT_SUBCPU);
	}
}

BRESULT ieitem_scpu(UINT id) {

	UINT	key;
	UINT8	keydata;

	if ((subcpu.s.cmdcnt) || (subcpu.s.datcnt)) {
		keystat.req_int = 1;			// 再送しる
		subcpu_sendkey();
		return(FALSE);
	}
	if (subcpu.s.vect == 0) {			// 割り込み不要だったら捨てる
		return(FALSE);
	}
	key = keystat_getflag();
	keydata = (UINT8)(key >> 8);
	if (subcpu.s.keydata != keydata) {
		subcpu.s.keydata = keydata;
		subcpu.s.keycount = 0;
		subcpu.s.keycountrep = 480;
	}
	else {
		if (keydata == 0) {
			return(FALSE);
		}
		key = key & (~0x20);			// rep
		subcpu.s.keycountrep = 48;		// 0.1sec
	}
	subcpu.s.work[1] = (UINT8)key;
	subcpu.s.work[0] = keydata;
	subcpu.s.cmdptr = offsetof(SUBCPU, s.zero);
	subcpu.s.datptr = offsetof(SUBCPU, s.work);
	subcpu.s.mode = 0xe6;
	subcpu.s.cmdcnt = 0;
	subcpu.s.datcnt = 2;

//	subcpu.s.OBF = 0;
//	subcpu.s.IBF = 1;
	iocore.s.ppib = (UINT8)((iocore.s.ppib & (~0x20)) | 0x40);

	Z80_INTERRUPT(subcpu.s.vect);
	(void)id;
	return(TRUE);
}


// ----

void subcpusetbuffer(SUBCPU *s, REG8 mode) {

	UINT32	key;

	switch(mode) {
		case 0xe3:
			key = keystat_gete3();
			s->s.work[2] = (UINT8)(key >> 16);
			s->s.work[1] = (UINT8)(key >> 8);
			s->s.work[0] = (UINT8)(key >> 0);
			break;

		case 0xe6:
			key = keystat_getflag();
			s->s.work[1] = (UINT8)(key >> 0);
			s->s.work[0] = (UINT8)(key >> 8);
			break;

		case 0xea:
			s->s.work[0] = cmt_ctrl_stat();
			break;

		case 0xeb:
			s->s.work[0] = cmt_tape_stat();
			break;

		case 0xed:
			calendar_getdate(s->s.work);
			break;

		case 0xef:
			calendar_gettime(s->s.work);
			break;
	}
}

void subcpusendctrl(SUBCPU *s) {

	switch(s->s.mode) {
		case 0xe9:
			cmt_ctrl(s->s.work[0]);
			break;

		case 0xec:
			calendar_setdate(s->s.work);
			break;

		case 0xee:
			calendar_settime(s->s.work);
			break;
	}
}


// ----

void subcpu_reset(void) {

	ZeroMemory(&subcpu, sizeof(subcpu));

//	subcpu.OBF = 1;
	iocore.s.ppib = (UINT8)(iocore.s.ppib | 0x20);

#if defined(FIX_Z80A)
	subcpu.e.intrclock = 2000000 * 2 / 480;
#else
	subcpu.e.intrclock = pccore.realclock / 480;
#endif
}

void subcpu_sendkey(void) {

	if (!nevent_iswork(NEVENT_SUBCPU)) {
		nevent_set(NEVENT_SUBCPU, subcpu.e.intrclock,
											neitem_scpu, NEVENT_ABSOLUTE);
	}
}

