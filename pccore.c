#include	"compiler.h"
#include	"dosio.h"
#include	"scrnmng.h"
#include	"soundmng.h"
#include	"sysmng.h"
#include	"timemng.h"
#include	"z80core.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"x1_io.h"
#include	"x1_crtc.h"
#include	"x1_vram.h"
#include	"x1_fdc.h"
#include	"draw.h"
#include	"sound.h"
#include	"sndctrl.h"
#include	"menu.h"
#include	"font.h"
#include	"juliet.h"
#include	"xmilver.h"
#include	"fddfile.h"
#include	"calendar.h"
#include	"timing.h"
#include	"defrom.res"
#include	"keystat.h"


const OEMCHAR xmilversion[] = OEMTEXT(XMILVER_CORE);

	XMILCFG		xmilcfg = {	0, 0, 1, 0, 1, 2, 0, 1,
							22050, 1000, 0, 80,
							0, 0,
							0, 0,
							0, 0, 0,
							1,
							1, 0};

	PCCORE		pccore = {250, 0, 1, 1, 0};
	BYTE		mMAIN[0x10000];
	BYTE		mBIOS[0x8000];
	BYTE		mBANK[16][0x8000];
//	BYTE		KNJ_FNT[0x4bc00];
	BYTE		GRP_RAM[0x20000];
	BYTE		TXT_RAM[0x01800];
	WORD		v_cnt;
	int			s_cnt;

	BYTE		*RAM0r;
	BYTE		*RAM0w;
	BYTE		lastmem;
	DWORD		h_cntbase;
	BRESULT		soundrenewal;



/***********************************************************************
	IPL-ROM LOAD
***********************************************************************/

void ipl_load(void) {

	FILEH	hdl;

	ZeroMemory(mBIOS, sizeof(mBIOS));
	memcpy(mBIOS, DEFROM, sizeof(DEFROM));

	if (pccore.ROM_TYPE >= 2) {
		if ((hdl = file_open_c("IPLROM.X1T")) != FILEH_INVALID) {
			file_read(hdl, mBIOS, 0x8000);
			file_close(hdl);
		}
	}
	else if (pccore.ROM_TYPE == 1) {
		if ((hdl = file_open_c("IPLROM.X1")) != FILEH_INVALID) {
			file_read(hdl, mBIOS, 0x8000);
			file_close(hdl);
		}
	}
}


/***********************************************************************
	FONT-ROM LOAD
***********************************************************************/

void fnt_load(void) {

	font_load(NULL, TRUE);
}


/***********************************************************************
	初期化
***********************************************************************/

BYTE reset_x1(BYTE ROM_TYPE, BYTE SOUND_SW, BYTE DIP_SW) {

	pccore.HSYNC_CLK = 250;
	pccore.ROM_SW = 1;
	pccore.ROM_TYPE = ROM_TYPE;
	pccore.SOUND_SW = SOUND_SW;
	pccore.DIP_SW = DIP_SW;

	// スクリーンモードの変更...
	if (pccore.ROM_TYPE >= 3) {
		if (scrnmng_setcolormode(TRUE) != SUCCESS) {
			pccore.ROM_TYPE = 2;
		}
	}
	else {
		scrnmng_setcolormode(FALSE);
	}

	textdrawproc_renewal();
	ipl_load();

	Z80_RESET();

	lastmem = 0x78;
	RAM0r = mBIOS;
	RAM0w = mMAIN;
	h_cntbase = 0;
	sysmng_cpureset();

	calendar_initialize();

	init_fdc();
	subcpu_reset();
	ppi_reset();
	init_crtc();
	init_vram();

	cgrom_reset();
	cmt_reset();
	ctc_reset();
	dmac_reset();
	pcg_reset();
	sio_reset();
	sndboard_reset();

	timing_reset();
	return(SUCCESS);
}



/***********************************************************************
	実行／終了
***********************************************************************/

static	BYTE	keyintcnt = 0;
static	BYTE	flame = 0;
static	BYTE	inttiming = 0;

void pccore_initialize(void) {

	sndctrl_initialize();

	init_draw();
	fnt_load();

	pcg_initialize();
	ppi_initialize();

	reset_x1(xmilcfg.ROM_TYPE, xmilcfg.SOUND_SW, xmilcfg.DIP_SW);

//	keystat_initialize();
//	keystat_reset();
}

void pccore_reset(void) {

	soundmng_stop();
	if (soundrenewal) {
		soundrenewal = 0;
		sndctrl_deinitialize();
		sndctrl_initialize();
	}
	reset_x1(xmilcfg.ROM_TYPE, xmilcfg.SOUND_SW, xmilcfg.DIP_SW);
	soundmng_play();
}

void pccore_deinitialize(void) {

	sndctrl_deinitialize();

//	keyboard_term();

	fdd_eject(0);
	fdd_eject(1);
	fdd_eject(2);
	fdd_eject(3);
}


// ----

// #define	IPTRACE			(1 << 14)

#if defined(TRACE) && IPTRACE
static	UINT	trpos = 0;
static	UINT16	treip[IPTRACE];

void iptrace_out(void) {

	FILEH	fh;
	UINT	s;
	UINT16	pc;
	char	buf[32];

	s = trpos;
	if (s > IPTRACE) {
		s -= IPTRACE;
	}
	else {
		s = 0;
	}
	fh = file_create_c("his.txt");
	while(s < trpos) {
		pc = treip[s & (IPTRACE - 1)];
		s++;
		SPRINTF(buf, "%.4x\r\n", pc);
		file_write(fh, buf, strlen(buf));
	}
	file_close(fh);
}
#endif



void x1r_exec(void) {

extern	BYTE	disp_flashscreen;

	v_cnt = 0;
	s_cnt = 0;
	xmilcfg.DISPSYNC &= 1;
	inttiming = xmilcfg.CPU8MHz & 1;

	while(s_cnt < 266) {
		while(h_cnt < pccore.HSYNC_CLK) {
#if IPTRACE
			treip[trpos & (IPTRACE - 1)] = Z80_PC;
			trpos++;
#endif
			Z80_EXECUTE();
			z80dmap();
		}
		h_cnt -= pccore.HSYNC_CLK;
		h_cntbase += pccore.HSYNC_CLK;
		inttiming ^= 2;
		if (inttiming != 3) {
			if (xmilcfg.SOUNDPLY) {
				sound_makesample(pcmbufsize[s_cnt]);
				juliet2_exec();
			}
			s_cnt++;
			x1_ctc_int();
			if (!((++keyintcnt) & 15)) {
				x1_sub_int();
				if (xmilcfg.MOUSE_SW) {
					sio_int();
				}
			}
		}
		v_cnt++;
		if (crtc.CRT_YL == v_cnt) {
			pcg.r.vsync = 1;
			if (xmilcfg.DISPSYNC == 1) {
				xmilcfg.DISPSYNC |= 0x80;
				scrnupdate();
			}
		}
	}
	if (++flame >= 60) {
		flame = 0;
		calendar_inc();
	}
	if (!xmilcfg.SOUNDPLY) {
		sound_makesample(framesoundcnt);
	}
	if (!(xmilcfg.DISPSYNC & 0x80)) {
		scrnupdate();
	}
}

