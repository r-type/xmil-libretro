#include	"compiler.h"
#include	<windowsx.h>
#include	<io.h>
#include	"strres.h"
#include	"xmil.h"
#include	"oemtext.h"
#include	"dosio.h"
#include	"ini.h"
#include	"pccore.h"


void ini_read(const OEMCHAR *path, const OEMCHAR *title,
											const INITBL *tbl, UINT count) {

const INITBL	*p;
const INITBL	*pterm;
#if defined(OSLANG_UCS2)
	OEMCHAR		item[10];
#else
const OEMCHAR	*item;
#endif
	OEMCHAR		work[512];
	UINT32		val;

	p = tbl;
	pterm = tbl + count;
	while(p < pterm) {
#if defined(OSLANG_UCS2)
		oemtext_sjis2oem(item, NELEMENTS(item), p->item, (UINT)-1);
#else
		item = p->item;
#endif
		switch(p->itemtype & INITYPE_MASK) {
			case INITYPE_STR:
				GetPrivateProfileString(title, item, (OEMCHAR *)p->value,
											(OEMCHAR *)p->value, p->arg, path);
				break;

			case INITYPE_BOOL:
				GetPrivateProfileString(title, item,
									(*((UINT8 *)p->value))?str_true:str_false,
												work, NELEMENTS(work), path);
				*((UINT8 *)p->value) = (!milstr_cmp(work, str_true))?1:0;
				break;

			case INITYPE_SINT8:
			case INITYPE_UINT8:
				val = (UINT8)GetPrivateProfileInt(title, item,
												*(UINT8 *)p->value, path);
				*(UINT8 *)p->value = (UINT8)val;
				break;

			case INITYPE_SINT16:
			case INITYPE_UINT16:
				val = (UINT16)GetPrivateProfileInt(title, item,
												*(UINT16 *)p->value, path);
				*(UINT16 *)p->value = (UINT16)val;
				break;

			case INITYPE_SINT32:
			case INITYPE_UINT32:
				val = (UINT32)GetPrivateProfileInt(title, item,
												*(UINT32 *)p->value, path);
				*(UINT32 *)p->value = (UINT32)val;
				break;

			case INITYPE_HEX8:
				SPRINTF(work, str_x, *(UINT8 *)p->value),
				GetPrivateProfileString(title, item, work,
												work, NELEMENTS(work), path);
				val = (UINT8)milstr_solveHEX(work);
				*(UINT8 *)p->value = (UINT8)val;
				break;

			case INITYPE_HEX16:
				SPRINTF(work, str_x, *(UINT16 *)p->value),
				GetPrivateProfileString(title, item, work,
												work, NELEMENTS(work), path);
				val = (UINT16)milstr_solveHEX(work);
				*(UINT16 *)p->value = (UINT16)val;
				break;

			case INITYPE_HEX32:
				SPRINTF(work, str_x, *(UINT32 *)p->value),
				GetPrivateProfileString(title, item, work,
												work, NELEMENTS(work), path);
				val = (UINT32)milstr_solveHEX(work);
				*(UINT32 *)p->value = (UINT32)val;
				break;
		}
		p++;
	}
}

void ini_write(const OEMCHAR *path, const OEMCHAR *title,
											const INITBL *tbl, UINT count) {

const INITBL	*p;
const INITBL	*pterm;
const OEMCHAR	*set;
	OEMCHAR		work[512];

	p = tbl;
	pterm = tbl + count;
	while(p < pterm) {
		if (!(p->itemtype & INIFLAG_RO)) {
			work[0] = '\0';
			set = work;
			switch(p->itemtype & INITYPE_MASK) {
				case INITYPE_STR:
					set = (OEMCHAR *)p->value;
					break;

				case INITYPE_BOOL:
					set = (*((UINT8 *)p->value))?str_true:str_false;
					break;

				case INITYPE_SINT8:
					SPRINTF(work, str_d, *((SINT8 *)p->value));
					break;

				case INITYPE_SINT16:
					SPRINTF(work, str_d, *((SINT16 *)p->value));
					break;

				case INITYPE_SINT32:
					SPRINTF(work, str_d, *((SINT32 *)p->value));
					break;

				case INITYPE_UINT8:
					SPRINTF(work, str_u, *((UINT8 *)p->value));
					break;

				case INITYPE_UINT16:
					SPRINTF(work, str_u, *((UINT16 *)p->value));
					break;

				case INITYPE_UINT32:
					SPRINTF(work, str_u, *((UINT32 *)p->value));
					break;

				case INITYPE_HEX8:
					SPRINTF(work, str_x, *((UINT8 *)p->value));
					break;

				case INITYPE_HEX16:
					SPRINTF(work, str_x, *((UINT16 *)p->value));
					break;

				case INITYPE_HEX32:
					SPRINTF(work, str_x, *((UINT32 *)p->value));
					break;

				default:
					set = NULL;
					break;
			}
			if (set) {
#if defined(OSLANG_UCS2)
				OEMCHAR	item[10];
				oemtext_sjis2oem(item, NELEMENTS(item), p->item, (UINT)-1);
				WritePrivateProfileString(title, item, set, path);
#else
				WritePrivateProfileString(title, p->item, set, path);
#endif
			}
		}
		p++;
	}
}


// ----

static const OEMCHAR ini_title[] = OEMTEXT("Xmillennium");

enum {
	INIRO_BOOL			= INITYPE_BOOL + INIFLAG_RO,
	INIMAX_UINT8		= INITYPE_UINT8 + INIFLAG_MAX,
	INIAND_UINT8		= INITYPE_UINT8 + INIFLAG_AND,
	INIROAND_HEX32		= INITYPE_HEX32 + INIFLAG_RO + INIFLAG_AND
};

static const INITBL iniitem[] = {
	{"WindposX", INITYPE_SINT32,	&xmiloscfg.winx,		0},
	{"WindposY", INITYPE_SINT32,	&xmiloscfg.winy,		0},
	{"Win_Snap", INITYPE_BOOL,		&xmiloscfg.WINSNAP,		0},
	{"backgrnd", INIAND_UINT8,		&xmiloscfg.background,	0x03},
	{"DspClock", INIAND_UINT8,		&xmiloscfg.DISPCLK,		0x03},

	{"StartDir", INITYPE_STR,		fddfolder,				MAX_PATH},
	{"bmap_Dir", INITYPE_STR,		bmpfilefolder,			MAX_PATH},

	{"IPL_TYPE", INIMAX_UINT8,		&xmilcfg.ROM_TYPE,		3},
	{"cpu_8MHz", INITYPE_BOOL,		&xmilcfg.CPU8MHz,		0},

	{"Resolute", INITYPE_HEX8,		&xmilcfg.DIP_SW,		0},
	{"SkipLine", INITYPE_BOOL,		&xmilcfg.SKIP_LINE,		0},
	{"s_NOWAIT", INITYPE_BOOL,		&xmilcfg.NOWAIT,		0},
	{"SkpFrame", INITYPE_UINT8,		&xmilcfg.DRAW_SKIP,		0},
	{"LinLIGHT", INITYPE_HEX8,		&xmilcfg.LINEDEPTH,		0},
	{"BlkLIGHT", INITYPE_HEX8,		&xmilcfg.BLKLIGHT,		0},
	{"TxtLIGHT", INITYPE_HEX8,		&xmilcfg.LINETEXT,		0},
	{"Text_Ext", INITYPE_BOOL,		&xmilcfg.TEXTMODE,		0},
	{"Text_400", INITYPE_HEX8,		&xmilcfg.TEXT400L,		0},

	{"OPMsound", INITYPE_BOOL,		&xmilcfg.SOUND_SW,		0},
	{"SampleHz", INITYPE_UINT16,	&xmilcfg.samplingrate,	0},
	{"Latencys", INITYPE_UINT16,	&xmilcfg.delayms,		0},
	{"Seek_Snd", INITYPE_BOOL,		&xmilcfg.MOTOR,			0},
	{"Seek_Vol", INIMAX_UINT8,		&xmilcfg.MOTORVOL,		100},

	{"btnRAPID", INITYPE_BOOL,		&xmilcfg.BTN_RAPID,		0},
	{"btn_MODE", INITYPE_BOOL,		&xmilcfg.BTN_MODE,		0},
	{"MouseInt", INITYPE_BOOL,		&xmilcfg.MOUSE_SW,		0},

	{"Joystick", INITYPE_BOOL,		&xmiloscfg.JOYSTICK,	0},

	{"clocknow", INITYPE_UINT8,		&xmiloscfg.clockx,		0},
	{"clockfnt", INITYPE_UINT8,		&xmiloscfg.clockfnt,	0},
	{"clock_up", INIROAND_HEX32,	&xmiloscfg.clockcolor1,	0xffffff},
	{"clock_dn", INIROAND_HEX32,	&xmiloscfg.clockcolor2,	0xffffff},

	{"Z80_SAVE", INIRO_BOOL,		&xmiloscfg.Z80SAVE,		0},
};


static void initgetfile(OEMCHAR *path, UINT size) {

	file_cpyname(path, modulefile, size);
	file_cutext(path);
	file_catname(path, OEMTEXT(".ini"), size);
}

void initload(void) {

	OEMCHAR	path[MAX_PATH];

	initgetfile(path, NELEMENTS(path));
	ini_read(path, ini_title, iniitem, NELEMENTS(iniitem));
}

void initsave(void) {

	OEMCHAR	path[MAX_PATH];

	initgetfile(path, NELEMENTS(path));
	ini_write(path, ini_title, iniitem, NELEMENTS(iniitem));
}

