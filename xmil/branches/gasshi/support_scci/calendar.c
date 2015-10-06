#include	"compiler.h"
#include	"parts.h"
#include	"timemng.h"
#include	"pccore.h"
#include	"nevent.h"
#include	"calendar.h"


	_CALENDAR	cal;


static const UINT8 days[12] = {	31, 28, 31, 30, 31, 30,
								31, 31, 30, 31, 30, 31};


static void secinc(_SYSTIME *dt) {

	UINT	month;
	UINT16	daylimit;

	dt->second++;
	if (dt->second < 60) {
		goto secinc_exit;
	}
	dt->second = 0;
	dt->minute++;
	if (dt->minute < 60) {
		goto secinc_exit;
	}
	dt->minute = 0;
	dt->hour++;
	if (dt->hour < 24) {
		goto secinc_exit;
	}
	dt->hour = 0;

	month = dt->month - 1;
	if (month < 12) {
		daylimit = days[month];
		if ((daylimit == 28) && (!(dt->year & 3))) {
			daylimit++;						/* = 29 */
		}
	}
	else {
		daylimit = 30;
	}
	dt->week = (UINT16)((dt->week + 1) % 7);
	dt->day++;
	if (dt->day < daylimit) {
		goto secinc_exit;
	}
	dt->day = 1;
	dt->month++;
	if (dt->month <= 12) {
		goto secinc_exit;
	}
	dt->month = 1;
	dt->year++;

secinc_exit:
	return;
}


/* ---- */

void calendar_getdate(UINT8 *bcd) {

	bcd[2] = AdjustAfterMultiply((UINT8)(cal.dt.year % 100));
	bcd[1] = ((cal.dt.month << 4) + cal.dt.week);
	bcd[0] = AdjustAfterMultiply((UINT8)cal.dt.day);
}

void calendar_setdate(const UINT8 *bcd) {

	UINT	year;

	year = AdjustBeforeDivision(bcd[2]);
	if (year < 80) {
		year += 100;
	}
	cal.dt.year = (UINT16)(year + 1900);
	cal.dt.week = (UINT8)(bcd[1] & 0x0f);
	cal.dt.month = (UINT8)(bcd[1] >> 4);
	cal.dt.day = AdjustBeforeDivision(bcd[0]);
}

void calendar_gettime(UINT8 *bcd) {

	bcd[2] = AdjustAfterMultiply((UINT8)cal.dt.hour);
	bcd[1] = AdjustAfterMultiply((UINT8)cal.dt.minute);
	bcd[0] = AdjustAfterMultiply((UINT8)cal.dt.second);
}

void calendar_settime(const UINT8 *bcd) {

	cal.dt.hour = AdjustBeforeDivision(bcd[2]);
	cal.dt.minute = AdjustBeforeDivision(bcd[1]);
	cal.dt.second = AdjustBeforeDivision(bcd[0]);
}


/* ---- */

void neitem_rtc(UINT id) {

	nevent_repeat(id);
	cal.steps++;
	if (cal.steps >= 5) {
		cal.steps -= 5;
		secinc(&cal.dt);
	}
}

void calendar_reset(void) {

	SINT32	clock;

	timemng_gettime(&cal.dt);
#if defined(FIX_Z80A)
	clock = 2000000 * 2 / 5;
#else
	clock = pccore.realclock / 5;
#endif
	nevent_set(NEVENT_RTC, clock, neitem_rtc, NEVENT_RELATIVE);
}

