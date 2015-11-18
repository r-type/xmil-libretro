/**
 * @file	dlgcfg.c
 * @brief	Implementation of the configure dialog
 */

#include "compiler.h"
#include "dlgcfg.h"
#include "../menubase/menudlg.h"
#include "../menubase/menures.h"
#include "strres.h"
#include "pccore.h"
#include "xmil.h"
#include "sysmng.h"
#if !defined(SIZE_QVGA)
#include	"palettes.h"
#endif

enum {
	DID_Z80A		= DID_USER,
	DID_Z80B,
	DID_Z80H,
	DID_RATE11,
	DID_RATE22,
	DID_RATE44,
	DID_BUFFER,
	DID_BUFSTR,
	DID_SKIPLINE,
	DID_SKIPRATIO,
	DID_SKIPLIGHT,
	DID_SKIPSTR
};

static const OEMCHAR str_cpu[] = OEMTEXT("CPU");
static const OEMCHAR str_z80a[] = OEMTEXT("Z80A - 4MHz");
static const OEMCHAR str_z80b[] = OEMTEXT("Z80B - 6MHz");
static const OEMCHAR str_z80h[] = OEMTEXT("Z80H - 8MHz");
static const OEMCHAR str_sound[] = OEMTEXT("Sound");
static const OEMCHAR str_rate[] = OEMTEXT("Rate");
static const OEMCHAR str_11khz[] = OEMTEXT("11KHz");
static const OEMCHAR str_22khz[] = OEMTEXT("22KHz");
static const OEMCHAR str_44khz[] = OEMTEXT("44KHz");
static const OEMCHAR str_buffer[] = OEMTEXT("Buffer");
#if !defined(SIZE_QVGA)
static const OEMCHAR str_screen[] = OEMTEXT("Screen");
static const OEMCHAR str_skipline[] = OEMTEXT("Use skipline revisions");
static const OEMCHAR str_ratio[] = OEMTEXT("Ratio");
#endif


#if defined(SIZE_QVGA)
static const MENUPRM res_cfg[] = {
			{DLGTYPE_FRAME,		DID_STATIC,		0,
				str_cpu,								  6,   6, 185,  59},
			{DLGTYPE_RADIO,		DID_Z80A,		MENU_TABSTOP,
				str_z80a,								 16,  20, 120,  11},
			{DLGTYPE_RADIO,		DID_Z80B,		MENU_GRAY,
				str_z80b,								 16,  34, 120,  11},
			{DLGTYPE_RADIO,		DID_Z80H,		MENU_GRAY,
				str_z80h,								 16,  48, 120,  11},
			{DLGTYPE_FRAME,		DID_STATIC,		0,
				str_sound,								  6,  70, 244,  52},
			{DLGTYPE_LTEXT,		DID_STATIC,		0,
				str_rate,								 16,  86,  40,  11},
			{DLGTYPE_RADIO,		DID_RATE11,		MENU_TABSTOP,
				str_11khz,								 60,  86,  60,  11},
			{DLGTYPE_RADIO,		DID_RATE22,		0,
				str_22khz,								120,  86,  60,  11},
			{DLGTYPE_RADIO,		DID_RATE44,		0,
				str_44khz,								180,  86,  60,  11},
			{DLGTYPE_LTEXT,		DID_STATIC,		0,
				str_buffer,								 16, 105,  40,  11},
			{DLGTYPE_SLIDER,	DID_BUFFER,		MSS_BOTH | MENU_TABSTOP,
				(void *)SLIDERPOS(100, 1000),			 60, 104, 140,  13},
			{DLGTYPE_RTEXT,		DID_BUFSTR,		0,
				NULL,									200, 105,  44,  11},
			{DLGTYPE_BUTTON,	DID_OK,			MENU_TABSTOP,
				mstr_ok,								197,  11,  52,  15},
			{DLGTYPE_BUTTON,	DID_CANCEL,		MENU_TABSTOP,
				mstr_cancel,							197,  30,  52,  15}};
#else
static const MENUPRM res_cfg[] = {
			{DLGTYPE_FRAME,		DID_STATIC,		0,
				str_cpu,								  8,   7, 202,  70},
			{DLGTYPE_RADIO,		DID_Z80A,		MENU_TABSTOP,
				str_z80a,								 20,  24, 120,  13},
			{DLGTYPE_RADIO,		DID_Z80B,		MENU_GRAY,
				str_z80b,								 20,  40, 120,  13},
			{DLGTYPE_RADIO,		DID_Z80H,		MENU_GRAY,
				str_z80h,								 20,  56, 120,  13},
			{DLGTYPE_FRAME,		DID_STATIC,		0,
				str_sound,								  8,  82, 288,  60},
			{DLGTYPE_LTEXT,		DID_STATIC,		0,
				str_rate,								 20, 100,  48,  13},
			{DLGTYPE_RADIO,		DID_RATE11,		MENU_TABSTOP,
				str_11khz,								 68, 100,  66,  13},
			{DLGTYPE_RADIO,		DID_RATE22,		0,
				str_22khz,								134, 100,  66,  13},
			{DLGTYPE_RADIO,		DID_RATE44,		0,
				str_44khz,								200, 100,  66,  13},
			{DLGTYPE_LTEXT,		DID_STATIC,		0,
				str_buffer,								 20, 120,  48,  13},
			{DLGTYPE_SLIDER,	DID_BUFFER,		MSS_BOTH | MENU_TABSTOP,
				(void *)SLIDERPOS(100, 1000),			 68, 120, 160,  13},
			{DLGTYPE_RTEXT,		DID_BUFSTR,		0,
				NULL,									228, 120,  48,  13},
			{DLGTYPE_FRAME,		DID_STATIC,		0,
				str_screen,								  8, 146, 288,  60},
			{DLGTYPE_CHECK,		DID_SKIPLINE,	0,
				str_skipline,							 20, 164, 192,  13},
			{DLGTYPE_LTEXT,		DID_SKIPRATIO,	0,
				str_ratio,								 28, 184,  40,  13},
			{DLGTYPE_SLIDER,	DID_SKIPLIGHT,	MSS_BOTH | MENU_TABSTOP,
				(void *)SLIDERPOS(0, 256),				 68, 184, 160,  13},
			{DLGTYPE_RTEXT,		DID_SKIPSTR,	0,
				NULL,									228, 184,  48,  13},
			{DLGTYPE_BUTTON,	DID_OK,			MENU_TABSTOP,
				mstr_ok,								218,  13,  77,  21},
			{DLGTYPE_BUTTON,	DID_CANCEL,		MENU_TABSTOP,
				mstr_cancel,							218,  38,  77,  21}};
#endif


// ----

static const OEMCHAR str_buffmt[] = OEMTEXT("%ums");

static void setbufstr(void) {

	UINT	val;
	OEMCHAR	work[32];

	val = menudlg_getval(DID_BUFFER);
	if (val < 100) {
		val = 100;
	}
	else if (val > 1000) {
		val = 1000;
	}
	OEMSPRINTF(work, str_buffmt, val);
	menudlg_settext(DID_BUFSTR, work);
}

#if !defined(SIZE_QVGA)
static void setskipen(UINT en) {

	menudlg_setenable(DID_SKIPRATIO, en);
	menudlg_setenable(DID_SKIPLIGHT, en);
	menudlg_setenable(DID_SKIPSTR, en);
}

static void setskipstr(void) {

	UINT	val;
	OEMCHAR	work[32];

	val = menudlg_getval(DID_SKIPLIGHT);
	if (val > 256) {
		val = 256;
	}
	OEMSPRINTF(work, str_u, val);
	menudlg_settext(DID_SKIPSTR, work);
}
#endif

static void dlginit(void) {

	MENUID	id;

	menudlg_appends(res_cfg, NELEMENTS(res_cfg));
	menudlg_setval(DID_Z80A, 1);

	if (xmilcfg.samplingrate < ((11025 + 22050) / 2)) {
		id = DID_RATE11;
	}
	else if (xmilcfg.samplingrate < ((22050 + 44100) / 2)) {
		id = DID_RATE22;
	}
	else {
		id = DID_RATE44;
	}
	menudlg_setval(id, 1);
	menudlg_setval(DID_BUFFER, xmilcfg.delayms);
	setbufstr();

#if !defined(SIZE_QVGA)
	menudlg_setval(DID_SKIPLINE, xmilcfg.skipline);
	menudlg_setval(DID_SKIPLIGHT, xmilcfg.skiplight);
	setskipen(xmilcfg.skipline);
	setskipstr();
#endif
}

static void dlgupdate(void) {

	UINT	update;
	UINT	val;
#if !defined(SIZE_QVGA)
	BRESULT	renewalflg;
#endif

	update = 0;

	if (menudlg_getval(DID_RATE11)) {
		val = 11025;
	}
	else if (menudlg_getval(DID_RATE44)) {
		val = 44100;
	}
	else {
		val = 22050;
	}
	if (xmilcfg.samplingrate != (UINT16)val) {
		xmilcfg.samplingrate = (UINT16)val;
		update |= SYS_UPDATECFG;
		corestat.soundrenewal = 1;
	}

	val = menudlg_getval(DID_BUFFER);
	if (val < 100) {
		val = 100;
	}
	else if (val > 1000) {
		val = 1000;
	}
	if (xmilcfg.delayms != (UINT16)val) {
		xmilcfg.delayms = (UINT16)val;
		update |= SYS_UPDATECFG;
		corestat.soundrenewal = 1;
	}

#if !defined(SIZE_QVGA)
	renewalflg = FALSE;
	val = menudlg_getval(DID_SKIPLINE);
	if (xmilcfg.skipline != (UINT8)val) {
		xmilcfg.skipline = (UINT8)val;
		renewalflg = TRUE;
	}
	val = menudlg_getval(DID_SKIPLIGHT);
	if (val > 256) {
		val = 256;
	}
	if (xmilcfg.skiplight != (UINT16)val) {
		xmilcfg.skiplight = (UINT16)val;
		renewalflg = TRUE;
	}
	if (renewalflg) {
		pal_reset();
		update |= SYS_UPDATECFG;
	}
#endif

	sysmng_update(update);
}

int dlgcfg_cmd(int msg, MENUID id, long param) {

	switch(msg) {
		case DLGMSG_CREATE:
			dlginit();
			break;

		case DLGMSG_COMMAND:
			switch(id) {
				case DID_OK:
					dlgupdate();
					menubase_close();
					break;

				case DID_CANCEL:
					menubase_close();
					break;

				case DID_BUFFER:
					setbufstr();
					break;

#if !defined(SIZE_QVGA)
				case DID_SKIPLINE:
					setskipen(menudlg_getval(DID_SKIPLINE));
					break;

				case DID_SKIPLIGHT:
					setskipstr();
					break;
#endif
			}
			break;

		case DLGMSG_CLOSE:
			menubase_close();
			break;
	}
	(void)param;
	return(0);
}

