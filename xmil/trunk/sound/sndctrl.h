
#include	"opmgen.h"
#include	"psggen.h"

#if !defined(DISABLE_SOUND)

#ifdef __cplusplus
extern "C" {
#endif

#if defined(SUPPORT_TURBOZ) || defined(SUPPORT_OPM)
extern	OPMCH		opmch[OPMCH_MAX];
extern	_OPMGEN		opmgen;
#endif
extern	_PSGGEN		psggen;

void sndctrl_initialize(void);
void sndctrl_deinitialize(void);
void sndctrl_reset(void);

#ifdef __cplusplus
}
#endif

#else

#define	sndctrl_initialize()
#define sndctrl_deinitialize()
#define sndctrl_reset()

#endif

