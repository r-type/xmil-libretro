
struct tagNdsSoundChannelReg
{
	UINT32 uControl;
	UINT32 uSource;
	UINT16 wTimer;
	UINT16 wRepeat;
	UINT32 uLength;
};

struct tagNdsSoundReg
{
	UINT16 uControl;
	tagNdsSoundChannelReg ch[16];
};

extern tagNdsSoundReg s_ndssound;

#define SCHANNEL_CR(n)				s_ndssound.ch[(n)].uControl
// #define SCHANNEL_VOL(n)				(*(vuint8*)(0x04000400 + ((n)<<4)))
// #define SCHANNEL_PAN(n)				(*(vuint8*)(0x04000402 + ((n)<<4)))
#define SCHANNEL_SOURCE(n)			s_ndssound.ch[(n)].uSource
#define SCHANNEL_TIMER(n)			s_ndssound.ch[(n)].wTimer
#define SCHANNEL_REPEAT_POINT(n)	s_ndssound.ch[(n)].wRepeat
#define SCHANNEL_LENGTH(n)			s_ndssound.ch[(n)].uLength

#define SOUND_CR          s_ndssound.uControl

#define SOUND_VOL(n)		(n)
#define SOUND_FREQ(n)		((-0x1000000 / (n)))
#define SOUND_ENABLE		(1 << 15)
#define SOUND_REPEAT    	(1 << 27)
#define SOUND_ONE_SHOT  	(1 << 28)
#define SOUND_FORMAT_16BIT	(1 << 29)
#define SOUND_FORMAT_8BIT	(0 << 29)
#define SOUND_FORMAT_PSG	(3 << 29)
#define SOUND_FORMAT_ADPCM	(2 << 29)
#define SOUND_16BIT			(1 << 29)
#define SOUND_8BIT			(0)

#define SOUND_PAN(n)		((n) << 16)

#define SCHANNEL_ENABLE		(1 << 31)
