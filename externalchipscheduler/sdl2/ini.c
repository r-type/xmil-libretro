#include "compiler.h"
#include "strres.h"
#include "profile.h"
#include "dosio.h"
#include "xmil.h"
#include "ini.h"
#include "pccore.h"

static const char ini_title[] = "Xmillennium";
static const char inifile[] = "xmil.cfg";

enum
{
	PFMAX_UINT8			= PFTYPE_UINT8 + PFFLAG_MAX,
};

static const PFTBL iniitem[] = {

	// OSàÀë∂Å`
	{"s_NOWAIT", PFTYPE_BOOL,		&xmiloscfg.NOWAIT,		0},
	{"SkpFrame", PFTYPE_UINT8,		&xmiloscfg.DRAW_SKIP,	0},

	// xmil
	{"IPL_TYPE", PFMAX_UINT8,		&xmilcfg.ROM_TYPE,		3},
	{"Resolute", PFTYPE_HEX8,		&xmilcfg.DIP_SW,		0},

	{"DispSync", PFTYPE_BOOL,		&xmilcfg.DISPSYNC,		0},
	{"Real_Pal", PFTYPE_BOOL,		&xmilcfg.RASTER,		0},
	{"skipline", PFTYPE_BOOL,		&xmilcfg.skipline,		0},
	{"skplight", PFTYPE_UINT16,		&xmilcfg.skiplight,		0},

	{"SampleHz", PFTYPE_UINT16,		&xmilcfg.samplingrate,	0},
	{"Latencys", PFTYPE_UINT16,		&xmilcfg.delayms,		0},
#if defined(SUPPORT_OPM)
	{"OPMsound", PFTYPE_BOOL,		&xmilcfg.SOUND_SW,		0},
#endif	//defined(SUPPORT_OPM)
	{"Seek_Snd", PFTYPE_BOOL,		&xmilcfg.MOTOR,			0},
	{"Seek_Vol", PFMAX_UINT8,		&xmilcfg.MOTORVOL,		100},

	{"MouseInt", PFTYPE_BOOL,		&xmilcfg.MOUSE_SW,		0},
	{"btnRAPID", PFTYPE_BOOL,		&xmilcfg.BTN_RAPID,		0},
	{"btn_MODE", PFTYPE_BOOL,		&xmilcfg.BTN_MODE,		0}};

void initload(void) {

	char	path[MAX_PATH];

	milstr_ncpy(path, file_getcd(inifile), SDL_arraysize(path));
	profile_iniread(path, ini_title, iniitem, SDL_arraysize(iniitem), NULL);
}

void initsave(void) {

	char	path[MAX_PATH];

	milstr_ncpy(path, file_getcd(inifile), SDL_arraysize(path));
	profile_iniwrite(path, ini_title, iniitem, SDL_arraysize(iniitem), NULL);
}
