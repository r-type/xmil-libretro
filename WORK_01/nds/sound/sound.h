
#ifndef SOUNDCALL
#define	SOUNDCALL
#endif

typedef void (SOUNDCALL * SOUNDCB)(void *hdl, SINT32 *pcm, UINT count);

#define	sound_create(r, m)			(FAILURE)
#define	sound_destroy()
#define sound_reset()
#define sound_changeclock()
#define sound_streamregist(h, c)
#define	sound_sync()
#define	sound_makesample(l)
#define	sound_pcmlock()				(NULL)
#define	sound_pcmunlock(h)

