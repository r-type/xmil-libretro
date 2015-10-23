#include	"compiler.h"
#include	"advuser.h"
#if !defined(DISABLE_SOUND)
#include	"soundmng.h"
#endif


LRESULT CALLBACK advuser_proc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {

#if !defined(DISABLE_SOUND)
	switch(msg) {
		case WM_ENTERMENULOOP:
			soundmng_disable(SNDPROC_MAIN);
			break;

		case WM_EXITMENULOOP:
			soundmng_enable(SNDPROC_MAIN);
			break;
	}
#endif
	return(AdvProc(hWnd, msg, wp, lp));
}

BRESULT advuser_initialize(void) {

	win9xsub_initialize();
#if !defined(DISABLE_SOUND)
	soundmng_initialize(__adv_hWnd);
#endif
	return(SUCCESS);
}

void advuser_deinitialize(void) {

#if !defined(DISABLE_SOUND)
	soundmng_deinitialize();
#endif
}

