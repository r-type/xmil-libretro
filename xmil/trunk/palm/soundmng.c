#include	"compiler.h"
#include	"parts.h"
#include	"soundmng.h"
#include	"sound.h"
#include	"pealstub.h"

#define	SOUNDBUFFERS	2


typedef struct {
	SndStreamRef	hdl;
	void*			cb;
	UINT			rate;
	UINT			samples;
	UINT			buffersize;
	SINT32			indata;
	SINT16*			nextdata;
	SINT16*			extendbuffer[SOUNDBUFFERS];
	UINT32			pos;
} _QSOUND, *QSOUND;

static	BOOL		QS_Avail = FALSE;
		_QSOUND		QSound;
static	BOOL		QSound_Playing = FALSE;

void *Callback_sound = NULL;

//static Err QSoundCallback(void* userDataP, SndStreamRef stream, void* bufferP, UInt32 frameCount);

Err m68k_sound_init (void* data) {
	Err ret = 0;
	ArgOne params;
	params.ptr = ByteSwap32(data);

	ret = (Err)(gCall68KFuncP)(gEmulStateP, (unsigned long)Callback_sound, &params, sizeof(params));
	return(ret);
}
/*
Err ARM_SndStreamCreate (void* strem, Int8 mode, UINT32 rate, Int16 type, Int8 width, SndStreamBufferCallback cb, void* userData, UINT32 size, Boolean arm) {

typedef struct RAWDATA {
	UINT32	s_all;
	UINT32	m_r;
	UINT32	r_t;
	UINT32	w_c;
	UINT32	c_d;
	UINT32	d_z;
	UINT32	z_a;
} param;

	param	args;
	Err ret = 0;
	
	args.s_all = ByteSwap32(strem);
	args.m_r = ((UINT32)mode << 8) | (ByteSwap32(rate) << 16);
	args.r_t = ByteSwap16(rate & 0xffff) | ByteSwap32(type);
	args.w_c = ((UINT32)width << 8) | (ByteSwap32(cb) << 16);
	args.c_d = (ByteSwap16(cb & 0xffff)) | (ByteSwap32(userData) << 16);
	args.d_z = (ByteSwap16(userData & 0xffff)) | (ByteSwap32(size) << 16);
	args.z_a = (ByteSwap16(size & 0xffff)) | ByteSwap32(arm);

	ret = (Err)(gCall68KFuncP)(gEmulStateP, PceNativeTrapNo(sysTrapSndStreamCreate), &args, sizeof(args));
	return(ret);
}
//static Err QSoundCallback(void* userDataP, SndStreamRef stream, void* bufferP, UInt32 frameCount) {
Err QSoundCallback(void* indata) {

	typedef struct {
		UINT8*	buffer;
		UINT32	frames;
	} CALLBACKDATA;
	
const SINT32	*src;
	CALLBACKDATA* data = indata;
	
	if (QSound_Playing) {
		src = sound_pcmlock();
		if (src) {
			satuation_s16(data->buffer, src, data->frames);
			sound_pcmunlock(src);
		}
	}

	return(0);
}
*/

static BOOL SoundBuffer_Init(UINT rate, UINT samples) {

	QSOUND			qs;
	Err				err;
	UINT32			buffersize;
	SINT16*			buffer[SOUNDBUFFERS];
//	SndStreamRef	ref;
	int	i;
	
	qs = &QSound;
	qs->rate = rate;
	qs->samples = samples;
	buffersize = samples * 4;
	qs->buffersize = buffersize;
	for (i=0;i<SOUNDBUFFERS;i++) {
		buffer[i] = _MALLOC(buffersize, "sound buffer");
		if (buffer[i] == NULL) {
			return(FAILURE);
		}
		ZeroMemory(buffer[i], buffersize);
	}
	
//	err = ARM_SndStreamCreate(&ref, sndOutput, rate, sndInt16Little, sndStereo, QSoundCallback, qs, 0, true);
	err = m68k_sound_init(qs);
	if (!err) {
//		ref = ByteSwap32(ref);
//		err = ARM_SndStreamSetVolume(ref, sndGameVolume);
//		qs->hdl = ref;
//		qs->cb = QSoundCallback;
		qs->indata = SOUNDBUFFERS-1;
		qs->pos = 0;
		for (i=0;i<SOUNDBUFFERS;i++) {
			qs->extendbuffer[i] = buffer[i];
		}
		qs->nextdata = qs->extendbuffer[SOUNDBUFFERS-1];
		QS_Avail = TRUE;
		return(SUCCESS);
	}
	return(FAILURE);
}

static void SoundBuffer_Term(void) {

	QSOUND	qs;
	int		i;

	qs = &QSound;
	qs->nextdata = NULL;
	ARM_SndStreamDelete(qs->hdl);
	for (i=0;i<SOUNDBUFFERS;i++) {
		if (qs->extendbuffer[i]) {
			_MFREE(qs->extendbuffer[i]);
			qs->extendbuffer[i] = NULL;
		}
	}
}

UINT soundmng_create(UINT rate, UINT ms) {

	UINT	samples;

	QSound_Playing = FALSE;

	if (rate < 11025) {
		rate = 11025;
	}
	else if (rate > 44100) {
		rate = 44100;
	}
/*
	if (SoundChannel_Init()) {
		goto qsinit_err;
	}
*/
	samples = rate * ms / (SOUNDBUFFERS * 1000);
	samples = (samples + 3) & (~3);
	if (SoundBuffer_Init(rate, samples)) {
		goto qsinit_err;
	}
	return(samples);

qsinit_err:
	soundmng_destroy();
	return(0);
}

void soundmng_destroy(void) {

	if (QS_Avail) {
		QS_Avail = FALSE;
		SoundBuffer_Term();
//		SoundChannel_Term();
	}
}

void soundmng_play(void) {

	QSOUND			qs;

	if (QS_Avail == TRUE) {
		qs = &QSound;
		ARM_SndStreamStart(qs->hdl);
		QSound_Playing = TRUE;
	}
}

void soundmng_stop(void) {

	QSOUND			qs;

	if (QS_Avail == TRUE) {
		qs = &QSound;
		ARM_SndStreamStop(qs->hdl);
		QSound_Playing = FALSE;
	}
}

void soundmng_sync(void) {

	QSOUND		qs;
const SINT32	*src;
static int		num = -1;
	
	if (QS_Avail == FALSE) {
		return;
	}

	qs = &QSound;
	if (qs->nextdata == NULL) {
		return;
	}
	if (num != qs->indata) {
		src = sound_pcmlock();
		if (src) {
			satuation_s16(qs->nextdata, src, qs->buffersize);
			num++;
			if (num >= SOUNDBUFFERS) {
				num = 0;
			}
			qs->nextdata = qs->extendbuffer[num];
			sound_pcmunlock(src);
		}
	}
}

