#include	"compiler.h"
#include	"strres.h"
#include	"profile.h"
#include	"xmil.h"
#include	"dosio.h"
#include	"ini.h"
#include	"pccore.h"


static const char ini_title[] = "Xmillennium";

enum {
	PFRO_BOOL		= PFTYPE_BOOL + PFFLAG_RO,
	PFMAX_UINT8		= PFTYPE_UINT8 + PFFLAG_MAX,
	PFAND_UINT8		= PFTYPE_UINT8 + PFFLAG_AND,
	PFROAND_HEX32	= PFTYPE_HEX32 + PFFLAG_RO + PFFLAG_AND
};

static const PFTBL iniitem[] = {
	{"WindposX", PFTYPE_SINT32,		&xmiloscfg.winx,		0},
	{"WindposY", PFTYPE_SINT32,		&xmiloscfg.winy,		0},
	{"DspClock", PFAND_UINT8,		&xmiloscfg.DISPCLK,		0x03},
	{"s_NOWAIT", PFTYPE_BOOL,		&xmiloscfg.NOWAIT,		0},
	{"SkpFrame", PFTYPE_UINT8,		&xmiloscfg.DRAW_SKIP,	0},
	{"FDfolder", PFTYPE_STR,		fddfolder,				MAX_PATH},
	{"bmap_Dir", PFTYPE_STR,		bmpfilefolder,			MAX_PATH},

	{"IPL_TYPE", PFMAX_UINT8,		&xmilcfg.ROM_TYPE,		3},
	{"Resolute", PFTYPE_HEX8,		&xmilcfg.DIP_SW,		0},

	{"DispSync", PFTYPE_BOOL,		&xmilcfg.DISPSYNC,		0},
	{"Real_Pal", PFTYPE_BOOL,		&xmilcfg.RASTER,		0},
	{"skipline", PFTYPE_BOOL,		&xmilcfg.skipline,		0},
	{"skplight", PFTYPE_UINT16,		&xmilcfg.skiplight,		0},

	{"SampleHz", PFTYPE_UINT16,		&xmilcfg.samplingrate,	0},
	{"Latencys", PFTYPE_UINT16,		&xmilcfg.delayms,		0},
	{"OPMsound", PFTYPE_BOOL,		&xmilcfg.SOUND_SW,		0},
	{"Seek_Snd", PFTYPE_BOOL,		&xmilcfg.MOTOR,			0},
	{"Seek_Vol", PFMAX_UINT8,		&xmilcfg.MOTORVOL,		100},

	{"MouseInt", PFTYPE_BOOL,		&xmilcfg.MOUSE_SW,		0},
	{"btnRAPID", PFTYPE_BOOL,		&xmilcfg.BTN_RAPID,		0},
	{"btn_MODE", PFTYPE_BOOL,		&xmilcfg.BTN_MODE,		0},

	{"Joystick", PFTYPE_BOOL,		&xmiloscfg.JOYSTICK,	0},
	{"Z80_SAVE", PFRO_BOOL,			&xmiloscfg.Z80SAVE,		0}};


void initgetfile(OEMCHAR *path, UINT size) {

	file_cpyname(path, modulefile, size);
	file_cutext(path);
	file_catname(path, OEMTEXT(".ini"), size);
}

void initload(void) {

	OEMCHAR	path[MAX_PATH];

	initgetfile(path, NELEMENTS(path));
	profile_iniread(path, ini_title, iniitem, NELEMENTS(iniitem), NULL);
}

void initsave(void) {

	OEMCHAR	path[MAX_PATH];

	initgetfile(path, NELEMENTS(path));
	profile_iniwrite(path, ini_title, iniitem, NELEMENTS(iniitem), NULL);
}

