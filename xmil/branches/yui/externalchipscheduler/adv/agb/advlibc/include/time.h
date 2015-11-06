#ifndef _ADVLIBC_TIME_
#define _ADVLIBC_TIME_

#include	"advtypes.h"

struct tm {
	int		tm_sec;
	int		tm_min;
	int		tm_hour;
	int		tm_mday;
	int		tm_mon;
	int		tm_year;
	int		tm_wday;
	int		tm_yday;
	int		tm_isdst;
};

#ifdef __cplusplus
extern "C" {
#endif

time_t LIBCCALL time(time_t *timer);
time_t LIBCCALL mktime(struct tm *timeptr);
struct tm LIBCCALL *gmtime(const time_t *timer);
struct tm LIBCCALL *localtime(const time_t *timer);
char LIBCCALL *asctime(const struct tm *timeptr);
char LIBCCALL *ctime(const time_t *timer);
char LIBCCALL *strdate(char *datestr);
char LIBCCALL *strtime(char *timestr);
size_t LIBCCALL strftime(char *string, size_t count, const char *format, const struct tm *timeptr);

#ifdef __cplusplus
}
#endif

#endif
