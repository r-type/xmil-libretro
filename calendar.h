
typedef struct {
	_SYSTIME	dt;
//	UINT		steps;
} _CALENDAR, *CALENDAR;


#ifdef __cplusplus
extern "C" {
#endif

extern	_CALENDAR	cal;

void calendar_initialize(void);
void calendar_inc(void);
void calendar_getdate(UINT8 *bcd);
void calendar_setdate(const UINT8 *bcd);
void calendar_gettime(UINT8 *bcd);
void calendar_settime(const UINT8 *bcd);

#ifdef __cplusplus
}
#endif

