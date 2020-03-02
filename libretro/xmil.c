#include "compiler.h"
#include "strres.h"
#include "xmil.h"
#include "dosio.h"
#include "fontmng.h"
#include "scrnmng.h"
#include "soundmng.h"
#include "sysmng.h"
#include "taskmng.h"
#include "ini.h"
#include "pccore.h"
#include "iocore.h"
#include "scrndraw.h"
#include "x1f.h"
#include "diskdrv.h"
#include "timing.h"
#include "keystat.h"
#include "vramhdl.h"
#include "menubase.h"

XMILOSCFG	xmiloscfg = {0, 0};

// ---- proc

int xmil_main(const char *floppy) {

	initload();

	TRACEINIT();

	if (fontmng_init() != SUCCESS) {
		goto np2main_err2;
	}

//	keystat_initialize();

	scrnmng_initialize();
	if (scrnmng_create(FULLSCREEN_WIDTH, FULLSCREEN_HEIGHT) != SUCCESS) {
		goto np2main_err4;
	}

	soundmng_initialize();
	sysmng_initialize();
	taskmng_initialize();
	pccore_initialize();
//	S98_init();

	scrndraw_redraw();
	pccore_reset();
 
	if(floppy)diskdrv_setfdd(0,floppy, 0/*read_only*/);

#if defined(SUPPORT_RESUME)
	if (xmiloscfg.resume) {
		id = flagload(str_sav, str_resume, FALSE);
		if (id == DID_CANCEL) {
			goto np2main_err5;
		}
	}
#endif

	return(SUCCESS);

#if defined(SUPPORT_RESUME)
np2main_err5:
	pccore_deinitialize();
	x1f_close();
	soundmng_deinitialize();
#endif	/* defined(SUPPORT_RESUME) */

np2main_err4:
	scrnmng_destroy();

np2main_err3:

np2main_err2:
	TRACETERM();
	//SDL_Quit();

np2main_err1:
	return(FAILURE);

}

int xmil_end(){
//	pccore_cfgupdate();
#if defined(SUPPORT_RESUME)
	if (xmiloscfg.resume) {
		flagsave(str_sav);
	}
	else {
		flagdelete(str_sav);
	}
#endif
	pccore_deinitialize();
	x1f_close();
	soundmng_deinitialize();

	sysmng_deinitialize();

	scrnmng_destroy();
	TRACETERM();
	//SDL_Quit();
	return(SUCCESS);

}

