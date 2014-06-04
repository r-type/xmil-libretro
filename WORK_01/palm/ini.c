#include	"compiler.h"
#include	"strres.h"
#include	"profile.h"
#include	"xmil.h"
#include	"dosio.h"
#include	"ini.h"
#include	"pccore.h"

// ----

static const char ini_title[] = "Xmil";
static const char inifile[] = "xmil.cfg";

// ---- INI* -> PF* �ɕύX (common/profile.h)

enum {
	PFRO_BOOL			= PFTYPE_BOOL + PFFLAG_RO,
	PFMAX_UINT8			= PFTYPE_UINT8 + PFFLAG_MAX,
	PFAND_UINT8			= PFTYPE_UINT8 + PFFLAG_AND,
	PFROAND_HEX32		= PFTYPE_HEX32 + PFFLAG_RO + PFFLAG_AND
};

static const PFTBL iniitem[] = {
	{"DspClock", PFAND_UINT8,		&xmiloscfg.DISPCLK,		0x03},
	{"s_NOWAIT", PFTYPE_BOOL,		&xmiloscfg.NOWAIT,		0},
	{"SkpFrame", PFTYPE_UINT8,		&xmiloscfg.DRAW_SKIP,	0},

	{"IPL_TYPE", PFMAX_UINT8,		&xmilcfg.ROM_TYPE,		3},
	{"Resolute", PFTYPE_HEX8,		&xmilcfg.DIP_SW,		0},

	{"DispSync", PFTYPE_BOOL,		&xmilcfg.DISPSYNC,		0},
	{"Real_Pal", PFTYPE_BOOL,		&xmilcfg.RASTER,		0},

	{"SampleHz", PFTYPE_UINT16,		&xmilcfg.samplingrate,	0},
	{"Latencys", PFTYPE_UINT16,		&xmilcfg.delayms,		0},
#if defined(SUPPORT_TURBOZ) || defined(SUPPORT_OPM)
	{"OPMsound", PFTYPE_BOOL,		&xmilcfg.SOUND_SW,		0},
	{"OPM_Volm", PFMAX_UINT8,		&xmilcfg.vol_fm,		100},
#endif
	{"PSG_Volm", PFMAX_UINT8,		&xmilcfg.vol_ssg,		100},
	{"Seek_Snd", PFTYPE_BOOL,		&xmilcfg.MOTOR,			0},
	{"Seek_Vol", PFMAX_UINT8,		&xmilcfg.MOTORVOL,		100},

	{"MouseInt", PFTYPE_BOOL,		&xmilcfg.MOUSE_SW,		0},
	{"btnRAPID", PFTYPE_BOOL,		&xmilcfg.BTN_RAPID,		0},
	{"btn_MODE", PFTYPE_BOOL,		&xmilcfg.BTN_MODE,		0},

	{"HARDKEY1", PFMAX_UINT8,		&xmiloscfg.HARDKEY1,	0},
	{"HARDKEY2", PFMAX_UINT8,		&xmiloscfg.HARDKEY2,		0},
	{"HARDKEY3", PFMAX_UINT8,		&xmiloscfg.HARDKEY3,		0},
	{"HARDKEY4", PFMAX_UINT8,		&xmiloscfg.HARDKEY4,		0},

#ifdef SUPPORT_RESUME
	{"x1resume", PFTYPE_BOOL,		&xmiloscfg.resume,		0},
#endif
	{"key_MODE", PFMAX_UINT8,		&xmiloscfg.FIVEWAY,		0},
	{"graffiti", PFTYPE_BOOL,		&xmiloscfg.graffiti,	0},
	{"Z80_SAVE", PFRO_BOOL,			&xmiloscfg.Z80SAVE,		0}};


void initload(void) {

	char	path[MAX_PATH];

	file_cpyname(path, file_getcd(inifile), sizeof(path));
	profile_iniread(path, ini_title, iniitem, NELEMENTS(iniitem), NULL);
}

void initsave(void) {

	char	path[MAX_PATH];

	file_cpyname(path, file_getcd(inifile), sizeof(path));
	profile_iniwrite(path, ini_title, iniitem, NELEMENTS(iniitem), NULL);
}

