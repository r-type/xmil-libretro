#include	"compiler.h"
#include	"strres.h"
#include	"profile.h"
#include	"xmil.h"
#include	"dosio.h"
#include	"ini.h"
#include	"pccore.h"


// ---- Å´ ÉgÉã

typedef struct {
const char		*title;
const INITBL	*tbl;
const INITBL	*tblterm;
	UINT		count;
} _INIARG, *INIARG;


static BOOL inigetbmp(const BYTE *ptr, UINT pos) {

	return((ptr[pos >> 3] >> (pos & 7)) & 1);
}

static void inisetbmp(BYTE *ptr, UINT pos, BOOL set) {

	UINT8	bit;

	ptr += (pos >> 3);
	bit = 1 << (pos & 7);
	if (set) {
		*ptr |= bit;
	}
	else {
		*ptr &= ~bit;
	}
}

static void inirdarg8(BYTE *dst, int dsize, const char *src) {

	int		i;
	BYTE	val;
	BOOL	set;
	char	c;

	for (i=0; i<dsize; i++) {
		val = 0;
		set = FALSE;
		while(*src == ' ') {
			src++;
		}
		while(1) {
			c = *src;
			if ((c == '\0') || (c == ' ')) {
				break;
			}
			else if ((c >= '0') && (c <= '9')) {
				val <<= 4;
				val += c - '0';
				set = TRUE;
			}
			else {
				c |= 0x20;
				if ((c >= 'a') && (c <= 'f')) {
					val <<= 4;
					val += c - 'a' + 10;
					set = TRUE;
				}
			}
			src++;
		}
		if (set == FALSE) {
			break;
		}
		dst[i] = val;
	}
}

static BOOL inireadcb(void *arg, const char *para,
										const char *key, const char *data) {

const INITBL	*p;

	if (arg == NULL) {
		return(FAILURE);
	}
	if (milstr_cmp(para, ((INIARG)arg)->title)) {
		return(SUCCESS);
	}
	p = ((INIARG)arg)->tbl;
	while(p < ((INIARG)arg)->tblterm) {
		if (!milstr_cmp(key, p->item)) {
			switch(p->itemtype & INITYPE_MASK) {
				case INITYPE_STR:
					milstr_ncpy((char *)p->value, data, p->size);
					break;

				case INITYPE_BOOL:
					*((BYTE *)p->value) = (!milstr_cmp(data, str_true))?1:0;
					break;

				case INITYPE_BITMAP:
					inisetbmp((BYTE *)p->value, p->size,
										(milstr_cmp(data, str_true) == 0));
					break;

				case INITYPE_BYTEARG:
					inirdarg8((BYTE *)p->value, p->size, data);
					break;

				case INITYPE_SINT8:
				case INITYPE_UINT8:
					*((BYTE *)p->value) = (BYTE)milstr_solveINT(data);
					break;

				case INITYPE_SINT16:
				case INITYPE_UINT16:
					*((UINT16 *)p->value) = (UINT16)milstr_solveINT(data);
					break;

				case INITYPE_SINT32:
				case INITYPE_UINT32:
					*((UINT32 *)p->value) = (UINT32)milstr_solveINT(data);
					break;

				case INITYPE_HEX8:
					*((BYTE *)p->value) = (BYTE)milstr_solveHEX(data);
					break;

				case INITYPE_HEX16:
					*((UINT16 *)p->value) = (UINT16)milstr_solveHEX(data);
					break;

				case INITYPE_HEX32:
					*((UINT32 *)p->value) = (UINT32)milstr_solveHEX(data);
					break;
			}
		}
		p++;
	}
	return(SUCCESS);
}

void ini_read(const char *path, const char *title,
											const INITBL *tbl, UINT count) {

	_INIARG	iniarg;

	if (path == NULL) {
		return;
	}
	iniarg.title = title;
	iniarg.tbl = tbl;
	iniarg.tblterm = tbl + count;
	profile_enum(path, &iniarg, inireadcb);
}


// ----

static void iniwrsetstr(char *work, int size, const char *ptr) {

	int		i;
	char	c;

	if (ptr[0] == ' ') {
		goto iwss_extend;
	}
	i = strlen(ptr);
	if ((i) && (ptr[i-1] == ' ')) {
		goto iwss_extend;
	}
	while(i > 0) {
		i--;
		if (ptr[i] == '\"') {
			goto iwss_extend;
		}
	}
	milstr_ncpy(work, ptr, size);
	return;

iwss_extend:
	if (size > 3) {
		size -= 3;
		*work++ = '\"';
		while(size > 0) {
			size--;
			c = *ptr++;
			if (c == '\"') {
				if (size > 0) {
					size--;
					work[0] = c;
					work[1] = c;
					work += 2;
				}
			}
			else {
				*work++ = c;
			}
		}
		work[0] = '\"';
		work[1] = '\0';
	}
}

static void iniwrsetarg8(char *work, int size, const BYTE *ptr, int arg) {

	int		i;
	char	tmp[8];

	if (arg > 0) {
		SPRINTF(tmp, "%.2x", ptr[0]);
		milstr_ncpy(work, tmp, size);
	}
	for (i=1; i<arg; i++) {
		SPRINTF(tmp, " %.2x", ptr[i]);
		milstr_ncat(work, tmp, size);
	}
}

void ini_write(const char *path, const char *title,
											const INITBL *tbl, UINT count) {

	FILEH		fh;
const INITBL	*p;
const INITBL	*pterm;
	BOOL		set;
	char		work[512];

	fh = file_create(path);
	if (fh == FILEH_INVALID) {
		return;
	}
	milstr_ncpy(work, "[", sizeof(work));
	milstr_ncat(work, title, sizeof(work));
	milstr_ncat(work, "]\r", sizeof(work));
	file_write(fh, work, strlen(work));

	p = tbl;
	pterm = tbl + count;
	while(p < pterm) {
		work[0] = '\0';
		set = SUCCESS;
		switch(p->itemtype & INITYPE_MASK) {
			case INITYPE_STR:
				iniwrsetstr(work, sizeof(work), (char *)p->value);
				break;

			case INITYPE_BOOL:
				milstr_ncpy(work, (*((BYTE *)p->value))?str_true:str_false,
																sizeof(work));
				break;

			case INITYPE_BITMAP:
				milstr_ncpy(work,
					(inigetbmp((BYTE *)p->value, p->size))?str_true:str_false,
																sizeof(work));
				break;

			case INITYPE_BYTEARG:
				iniwrsetarg8(work, sizeof(work), (BYTE *)p->value, p->size);
				break;

			case INITYPE_SINT8:
				SPRINTF(work, str_d, *((char *)p->value));
				break;

			case INITYPE_SINT16:
				SPRINTF(work, str_d, *((SINT16 *)p->value));
				break;

			case INITYPE_SINT32:
				SPRINTF(work, str_d, *((SINT32 *)p->value));
				break;

			case INITYPE_UINT8:
				SPRINTF(work, str_u, *((BYTE *)p->value));
				break;

			case INITYPE_UINT16:
				SPRINTF(work, str_u, *((UINT16 *)p->value));
				break;

			case INITYPE_UINT32:
				SPRINTF(work, str_u, *((UINT32 *)p->value));
				break;

			case INITYPE_HEX8:
				SPRINTF(work, str_x, *((BYTE *)p->value));
				break;

			case INITYPE_HEX16:
				SPRINTF(work, str_x, *((UINT16 *)p->value));
				break;

			case INITYPE_HEX32:
				SPRINTF(work, str_x, *((UINT32 *)p->value));
				break;

			default:
				set = FAILURE;
				break;
		}
		if (set == SUCCESS) {
			file_write(fh, p->item, strlen(p->item));
			file_write(fh, " = ", 3);
			file_write(fh, work, strlen(work));
			file_write(fh, "\r", 1);
		}
		p++;
	}
	file_close(fh);
}


// ---- Å™ ÉgÉã
// ----

static const char ini_title[] = "Xmil";
static const char inifile[] = "xmil.cfg";

// ---- INI* -> PF* Ç…ïœçX (common/profile.h)

enum {
	INIRO_BOOL			= INITYPE_BOOL + INIFLAG_RO,
	INIMAX_UINT8		= INITYPE_UINT8 + INIFLAG_MAX,
	INIAND_UINT8		= INITYPE_UINT8 + INIFLAG_AND,
	INIROAND_HEX32		= INITYPE_HEX32 + INIFLAG_RO + INIFLAG_AND
};

static const INITBL iniitem[] = {
	{"windposx", INITYPE_SINT32,	&xmiloscfg.posx,		0},
	{"windposy", INITYPE_SINT32,	&xmiloscfg.posy,		0},
	{"DspClock", INIAND_UINT8,		&xmiloscfg.DISPCLK,		0x03},
	{"s_NOWAIT", INITYPE_BOOL,		&xmiloscfg.NOWAIT,		0},
	{"SkpFrame", INITYPE_UINT8,		&xmiloscfg.DRAW_SKIP,	0},

	{"IPL_TYPE", INIMAX_UINT8,		&xmilcfg.ROM_TYPE,		3},
	{"Resolute", INITYPE_HEX8,		&xmilcfg.DIP_SW,		0},

	{"DispSync", INITYPE_BOOL,		&xmilcfg.DISPSYNC,		0},
	{"Real_Pal", INITYPE_BOOL,		&xmilcfg.RASTER,		0},
	{"skipline", INITYPE_BOOL,		&xmilcfg.skipline,		0},
	{"skplight", INITYPE_UINT16,	&xmilcfg.skiplight,		0},

	{"SampleHz", INITYPE_UINT16,	&xmilcfg.samplingrate,	0},
	{"Latencys", INITYPE_UINT16,	&xmilcfg.delayms,		0},
	{"OPMsound", INITYPE_BOOL,		&xmilcfg.SOUND_SW,		0},
	{"Seek_Snd", INITYPE_BOOL,		&xmilcfg.MOTOR,			0},
	{"Seek_Vol", INIMAX_UINT8,		&xmilcfg.MOTORVOL,		100},

	{"MouseInt", INITYPE_BOOL,		&xmilcfg.MOUSE_SW,		0},
	{"btnRAPID", INITYPE_BOOL,		&xmilcfg.BTN_RAPID,		0},
	{"btn_MODE", INITYPE_BOOL,		&xmilcfg.BTN_MODE,		0},

	{"Z80_SAVE", INIRO_BOOL,		&xmiloscfg.Z80SAVE,		0}};


void initload(void) {

	char	path[MAX_PATH];

	file_cpyname(path, file_getcd(inifile), sizeof(path));
	ini_read(path, ini_title, iniitem, NELEMENTS(iniitem));
}

void initsave(void) {

	char	path[MAX_PATH];

	file_cpyname(path, file_getcd(inifile), sizeof(path));
	ini_write(path, ini_title, iniitem, NELEMENTS(iniitem));
}

