
#ifdef __cplusplus
extern "C" {
#endif

extern	BRESULT	__adv_avail;
extern	HWND	__adv_hWnd;
extern	OEMCHAR	__adv_modulefile[MAX_PATH];
extern	UINT8	__adv_reg[0x1000];

LRESULT CALLBACK AdvProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);

BRESULT advsys_task(void);

#ifdef __cplusplus
}
#endif

#include	"advvideo.h"
#include	"advsound.h"
#include	"advkey.h"

