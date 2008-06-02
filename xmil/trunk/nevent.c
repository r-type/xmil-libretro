#include	"compiler.h"
#include	"z80core.h"
#include	"pccore.h"
#include	"nevent.h"


	_NEVENT		nevent;


void nevent_allreset(void) {

	UINT	i;

	ZeroMemory(&nevent, sizeof(nevent));
	nevent.first = NEVENTITEM_TERM;
	for (i=0; i<NEVENT_MAXEVENTS; i++) {
		nevent.item[i].next = NEVENTITEM_NONE;
	}
}

void nevent_progress(void) {

	NEVENTITEM	item;
	NEVENTITEM	itemnext;
	NEVENTITEM	*witem;
	NEVENTITEM	waititem[NEVENT_MAXEVENTS];
	SINT32		nextbase;
	NEVENTITEM	*wi;

	CPU_CLOCK += CPU_BASECLOCK;

	item = nevent.first;
	witem = waititem;
	while(item != NEVENTITEM_TERM) {
		item->clock -= CPU_BASECLOCK;
		if (item->clock > 0) {
			break;
		}
		*witem++ = item;
		itemnext = item->next;
		item->next = NEVENTITEM_NONE;
		item = itemnext;
	}
	nevent.first = item;
	if (item != NEVENTITEM_TERM) {
		nextbase = item->clock;
		item = item->next;
		while(item != NEVENTITEM_TERM) {
			item->clock -= CPU_BASECLOCK;
			item = item->next;
		}
	}
	else {
		nextbase = NEVENT_MAXCLOCK;
	}
	CPU_BASECLOCK = nextbase;
	CPU_REMCLOCK += nextbase;
	/* TRACEOUT(("nextbase = %d (%d)", nextbase, CPU_REMCLOCK)); */

	/* ÉCÉxÉìÉgî≠ê∂ */
	wi = waititem;
	while(wi < witem) {
		item = *wi++;
		if (item->proc != NULL) {
			item->proc(item - nevent.item);
		}
	}
}

void nevent_reset(UINT id) {

	NEVENTITEM	item;
	NEVENTITEM	*back;
	NEVENTITEM	cur;

	item = nevent.item + id;
	if (item->next != NEVENTITEM_NONE) {
		back = &nevent.first;
		cur = *back;
		while(cur != item) {
			__ASSERT(cur != NEVENTITEM_NONE);
			__ASSERT(cur != NEVENTITEM_TERM);
			back = &cur->next;
			cur = *back;
		}
		*back = cur->next;
		cur->next = NEVENTITEM_NONE;
	}
}

void nevent_set(UINT id, SINT32 eventclock, NEVENTCB proc, BRESULT absolute) {

	SINT32		clock;
	NEVENTITEM	item;
	NEVENTITEM	*back;
	NEVENTITEM	n;
	NEVENTITEM	p;

	/* TRACEOUT(("event %d - %xclocks", id, eventclock)); */

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

	if (item->next != NEVENTITEM_NONE) {
		back = &nevent.first;
		n = *back;
		while(n != item) {
			__ASSERT(n != NEVENTITEM_NONE);
			__ASSERT(n != NEVENTITEM_TERM);
			back = &n->next;
			n = *back;
		}
		*back = n->next;
		n->next = NEVENTITEM_NONE;
	}

	n = nevent.first;
	if ((n == NEVENTITEM_TERM) || (item->clock < n->clock)) {
		item->next = n;
		nevent.first = item;
		clock = CPU_BASECLOCK;
		if (item->clock > 0) {
			clock -= item->clock;
		}
		CPU_BASECLOCK -= clock;
		CPU_REMCLOCK -= clock;
		/* TRACEOUT(("reset nextbase -%d (%d)", clock, CPU_REMCLOCK)); */
	}
	else {
		p = n->next;
		while((p != NEVENTITEM_TERM) && (item->clock >= p->clock)) {
			n = p;
			p = n->next;
		}
		item->next = p;
		n->next = item;
	}
}

void nevent_repeat(UINT id) {

	NEVENTITEM	item;

	item = nevent.item + id;
	nevent_set(id, item->baseclock, item->proc, NEVENT_RELATIVE);
}

void nevent_setbyms(UINT id, SINT32 ms, NEVENTCB proc, BRESULT absolute) {

	SINT32	clock;

#if defined(FIX_Z80A)
	clock = (2000000 * 2 / 1000) * ms;
#else
	clock = (pccore.realclock / 1000) * ms;
#endif
	nevent_set(id, clock, proc, absolute);
}

BRESULT nevent_iswork(UINT id) {

	NEVENTITEM	item;

	item = nevent.item + id;
	if (item->next != NEVENTITEM_NONE) {
		return(TRUE);
	}
	else {
		return(FALSE);
	}
}

SINT32 nevent_getwork(UINT id) {

	NEVENTITEM	item;

	item = nevent.item + id;
	if (item->next != NEVENTITEM_NONE) {
		return(item->baseclock -
							(item->clock - (CPU_BASECLOCK - CPU_REMCLOCK)));
	}
	else {
		return(-1);
	}
}

SINT32 nevent_getremain(UINT id) {

	NEVENTITEM	item;

	item = nevent.item + id;
	if (item->next != NEVENTITEM_NONE) {
		return(item->clock - (CPU_BASECLOCK - CPU_REMCLOCK));
	}
	else {
		return(-1);
	}
}

void nevent_forceexit(void) {

	if (CPU_REMCLOCK > 0) {
		CPU_BASECLOCK -= CPU_REMCLOCK;
		CPU_REMCLOCK = 0;
	}
}

