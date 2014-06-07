
#ifndef SOUNDCALL
#define	SOUNDCALL
#endif

typedef void (SOUNDCALL * SOUNDCB)(void *hdl, SINT32 *pcm, UINT count);

#if !defined(DISABLE_SOUND)

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

BRESULT sound_create(UINT rate, UINT ms);
void sound_destroy(void);

void sound_reset(void);
void sound_changeclock(void);
void sound_streamregist(void *hdl, SOUNDCB cbfn);

void sound_sync(void);
void sound_makesample(UINT length);

const SINT32 *sound_pcmlock(void);
void sound_pcmunlock(const SINT32 *hdl);

#if defined(SUPPORT_WAVEREC)
BRESULT sound_recstart(const OEMCHAR *filename);
void sound_recstop(void);
#endif

#ifdef __cplusplus
}
#endif

#else

#define	sound_create(r, m)			(FAILURE)
#define	sound_destroy()
#define sound_reset()
#define sound_changeclock()
#define sound_streamregist(h, c)
#define	sound_sync()
#define	sound_makesample(l)
#define	sound_pcmlock()				(NULL)
#define	sound_pcmunlock(h)

#endif

