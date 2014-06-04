
typedef struct {
	_SYSTIME	dt;
	UINT		steps;
} _CALENDAR, *CALENDAR;


#ifdef __cplusplus
extern "C" {
#endif

extern	_CALENDAR	cal;


void neitem_rtc(UINT id);

void calendar_getdate(UINT8 *bcd);
void calendar_setdate(const UINT8 *bcd);
void calendar_gettime(UINT8 *bcd);
void calendar_settime(const UINT8 *bcd);

void calendar_reset(void);

#ifdef __cplusplus
}
#endif

