#include	"compiler.h"
#include	"timemng.h"


BOOL timemng_gettime(_SYSTIME *systime) {

	UINT32			now_time[4];
	DateTimeType*	ptr;

	ptr = (DateTimeType*)now_time;
	ARM_TimSecondsToDateTime (ARM_TimGetSeconds(), ptr);

	systime->year = ByteSwap32(now_time[2]) & 0xffff;
	systime->month = ByteSwap16(now_time[2] & 0xffff);
	systime->week = ByteSwap16(now_time[3] & 0xffff);
	systime->day = ByteSwap32(now_time[1]) & 0xffff;
	systime->hour = ByteSwap16(now_time[1] & 0xffff);
	systime->minute = ByteSwap32(now_time[0]) & 0xffff;
	systime->second = ByteSwap16(now_time[0] & 0xffff);
	systime->milli = 0;
/*
	systime->year = ByteSwap16(now_time.year);
	systime->month = ByteSwap16(now_time.month);
	systime->week = ByteSwap16(now_time.weekDay);
	systime->day = ByteSwap16(now_time.day);
	systime->hour = ByteSwap16(now_time.hour);
	systime->minute = ByteSwap16(now_time.minute);
	systime->second = ByteSwap16(now_time.second);
*/
	return(SUCCESS);
}

