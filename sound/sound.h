
#ifndef SOUNDCALL
#define	SOUNDCALL
#endif

typedef void (SOUNDCALL * SOUNDCB)(void *hdl, SINT32 *pcm, UINT count);

typedef struct {
	UINT	rate;
	UINT32	hzbase;
	UINT32	clockbase;
	UINT32	minclock;
	UINT32	lastclock;
	UINT	writecount;
} SOUNDCFG;


#ifdef __cplusplus
extern "C" {
#endif

extern	SOUNDCFG	soundcfg;

BOOL sound_create(UINT rate, UINT ms);
void sound_destroy(void);

void sound_reset(void);
void sound_changeclock(void);
void sound_streamregist(void *hdl, SOUNDCB cbfn);

void sound_sync(void);
void sound_makesample(UINT length);

const SINT32 *sound_pcmlock(void);
void sound_pcmunlock(const SINT32 *hdl);

#if defined(SUPPORT_WAVEREC)
BOOL sound_recstart(const char *filename);
void sound_recstop(void);
#endif

#ifdef __cplusplus
}
#endif

