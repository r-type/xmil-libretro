
#include	"opmgen.h"
#include	"psggen.h"

#ifdef __cplusplus
extern "C" {
#endif

extern	UINT		pcmbufsize[];
extern	OPMCH		opmch[OPMCH_MAX];
extern	_OPMGEN		opmgen;
extern	_PSGGEN		psggen;

void sndctrl_initialize(void);
void sndctrl_deinitialize(void);
void sndctrl_reset(void);

#ifdef __cplusplus
}
#endif

