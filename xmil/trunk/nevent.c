#include	"compiler.h"
#include	"z80core.h"
#include	"pccore.h"
#include	"nevent.h"


	_NEVENT		nevent;


void nevent_allreset(void) {

	ZeroMemory(&nevent, sizeof(nevent));
}

void nevent_progress(void) {

	SINT32		nextbase;
	UINT		eventnum;
	UINT		waitevents;
	UINT		i;
	UINT		curid;
	NEVENTITEM	item;
	UINT		waitlevel[NEVENT_MAXEVENTS];

	CPU_CLOCK += CPU_BASECLOCK;

	nextbase = NEVENT_MAXCLOCK;
	eventnum = 0;
	waitevents = 0;
	for (i=0; i<nevent.readyevents; i++) {
		curid = nevent.level[i];
		item = nevent.item + curid;
		item->clock -= CPU_BASECLOCK;
		if (item->clock > 0) {
			// �C�x���g�҂���
			nevent.level[eventnum++] = curid;
			if (nextbase >= item->clock) {
				nextbase = item->clock;
			}
		}
		else {
			waitlevel[waitevents++] = curid;
//			TRACEOUT(("event = %x", curid));
		}
	}
	nevent.readyevents = eventnum;
	CPU_BASECLOCK = nextbase;
	CPU_REMCLOCK += nextbase;
//	TRACEOUT(("nextbase = %d (%d)", nextbase, CPU_REMCLOCK));

	// �C�x���g����
	for (i=0; i<waitevents; i++) {
		curid = waitlevel[i];
		item = nevent.item + curid;
		if (item->proc != NULL) {
			item->proc(curid);
		}
	}
}

void nevent_reset(UINT id) {

	UINT	i;

	// ���ݐi�s���Ă�C�x���g������
	for (i=0; i<nevent.readyevents; i++) {
		if (nevent.level[i] == id) {
			break;
		}
	}
	// �C�x���g�͑��݂����H
	if (i < nevent.readyevents) {
		// ���݂��Ă�������
		nevent.readyevents--;
		for (; i<nevent.readyevents; i++) {
			nevent.level[i] = nevent.level[i+1];
		}
	}
}

void nevent_set(UINT id, SINT32 eventclock, NEVENTCB proc, BRESULT absolute) {

	SINT32		clock;
	NEVENTITEM	item;
	UINT		eventid;
	UINT		i;

	TRACEOUT(("event %d - %xclocks", id, eventclock));

	clock = CPU_BASECLOCK - CPU_REMCLOCK;
	item = nevent.item + id;
	item->baseclock = eventclock;
	item->proc = proc;
	if (absolute) {
		item->clock = eventclock + clock;
	}
	else {
		item->clock += eventclock;
	}

	// �C�x���g�̍폜
	nevent_reset(id);

	// �C�x���g�̑}���ʒu�̌���
	for (eventid=0; eventid<nevent.readyevents; eventid++) {
		if (item->clock < nevent.item[nevent.level[eventid]].clock) {
			break;
		}
	}

	// �C�x���g�̑}��
	for (i=nevent.readyevents; i>eventid; i--) {
		nevent.level[i] = nevent.level[i-1];
	}
	nevent.level[eventid] = id;
	nevent.readyevents++;

	// �����ŒZ�C�x���g��������...
	if (eventid == 0) {
		clock = CPU_BASECLOCK;
		if (item->clock > 0) {
			clock -= item->clock;
		}
		CPU_BASECLOCK -= clock;
		CPU_REMCLOCK -= clock;
//		TRACEOUT(("reset nextbase -%d (%d)", clock, CPU_REMCLOCK));
	}
}

void nevent_repeat(UINT id) {

	NEVENTITEM	item;

	item = nevent.item + id;
	nevent_set(id, item->baseclock, item->proc, NEVENT_RELATIVE);
}

void nevent_setbyms(UINT id, SINT32 ms, NEVENTCB proc, BRESULT absolute) {

	nevent_set(id, (pccore.realclock / 1000) * ms, proc, absolute);
}

BRESULT nevent_iswork(UINT id) {

	UINT	i;

	// ���ݐi�s���Ă�C�x���g������
	for (i=0; i<nevent.readyevents; i++) {
		if (nevent.level[i] == id) {
			return(TRUE);
		}
	}
	return(FALSE);
}

SINT32 nevent_getwork(UINT id) {

	UINT	i;

	// ���ݐi�s���Ă�C�x���g������
	for (i=0; i<nevent.readyevents; i++) {
		if (nevent.level[i] == id) {
			return(nevent.item[id].baseclock - 
				(nevent.item[id].clock - (CPU_BASECLOCK - CPU_REMCLOCK)));
		}
	}
	return(-1);
}

SINT32 nevent_getremain(UINT id) {

	UINT	i;

	// ���ݐi�s���Ă�C�x���g������
	for (i=0; i<nevent.readyevents; i++) {
		if (nevent.level[i] == id) {
			return(nevent.item[id].clock - (CPU_BASECLOCK - CPU_REMCLOCK));
		}
	}
	return(-1);
}

void nevent_forceexit(void) {

	if (CPU_REMCLOCK > 0) {
		CPU_BASECLOCK -= CPU_REMCLOCK;
		CPU_REMCLOCK = 0;
	}
}

