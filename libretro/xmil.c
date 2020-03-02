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

static void usage(const char *progname) {

	printf("Usage: %s [options]\n", progname);
	printf("\t--help   [-h]       : print this message\n");
}

// ---- proc


char* get_file_ext(char* filepath){
   if(filepath[strlen(filepath) - 3]=='.')return filepath + strlen(filepath) - 2;
   return filepath + strlen(filepath) - 3;
}

int xmil_main(int argc, char *argv[]) {

	int		pos;
	char	*p;
        int load_floppy=0;

	pos = 1;
	while(pos < argc) {
		p = argv[pos++];
		if ((!milstr_cmp(p, "-h")) || (!milstr_cmp(p, "--help"))) {
			usage(argv[0]);
			goto np2main_err1;
		}
		else if(strcmp(get_file_ext(p), "88d") == 0 ||\
			strcmp(get_file_ext(p), "88D") == 0 ||\
			strcmp(get_file_ext(p), "dx1") == 0 ||\
			strcmp(get_file_ext(p), "DX1") == 0 ||\
			strcmp(get_file_ext(p), "2d")  == 0 ||\
			strcmp(get_file_ext(p), "2D")  == 0 ||\
			strcmp(get_file_ext(p), "xdf") == 0 ||\
			strcmp(get_file_ext(p), "XDF") == 0 ||\
			strcmp(get_file_ext(p), "hdm") == 0 ||\
			strcmp(get_file_ext(p), "HDM") == 0 ||\
			strcmp(get_file_ext(p), "dup") == 0 ||\
			strcmp(get_file_ext(p), "DUP") == 0 ||\
			strcmp(get_file_ext(p), "2HD") == 0 ||\
			strcmp(get_file_ext(p), "2hd") == 0 ||\
			strcmp(get_file_ext(p), "tfd") == 0 ||\
			strcmp(get_file_ext(p), "TFD") == 0 ||\
			strcmp(get_file_ext(p), "d88") == 0 ||\
			strcmp(get_file_ext(p), "D88") == 0 )
	        {
			load_floppy=1; break;
		}
		else {
			printf("error command: %s\n", p);
			goto np2main_err1;
		}
	}

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
 
	if(load_floppy)diskdrv_setfdd(0,p, 0/*read_only*/);

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

