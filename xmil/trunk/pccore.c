#include	"compiler.h"
#include	"dosio.h"
#include	"scrnmng.h"
#include	"soundmng.h"
#include	"sysmng.h"
#include	"timemng.h"
#include	"xmilver.h"
#include	"z80core.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"timing.h"
#include	"calendar.h"
#include	"keystat.h"
#include	"palettes.h"
#include	"makescrn.h"
#include	"sound.h"
#include	"sndctrl.h"
#include	"font.h"
#include	"fddfile.h"
#include	"defrom.res"


const OEMCHAR xmilversion[] = OEMTEXT(XMILVER_CORE);

	XMILCFG		xmilcfg = { 2, 0, 1,
							1, 0, 0, 0,
							22050, 500, 0, 0, 80,
							0, 0, 0, 0};

	PCCORE		pccore = {250, 0, 1, 0};
	CORESTAT	corestat;
	BYTE		mMAIN[0x10000];
	BYTE		mBIOS[0x8000];
#if defined(SUPPORT_BANKMEM)
	UINT8		mBANK[16][0x8000];
#endif
	UINT		v_cnt;
	UINT		s_cnt;

	BYTE		*RAM0r;
	BYTE		*RAM0w;
	DWORD		h_cntbase;



/***********************************************************************
	IPL-ROM LOAD
***********************************************************************/

static void ipl_load(void) {

	FILEH	hdl;

	ZeroMemory(mBIOS, sizeof(mBIOS));
	CopyMemory(mBIOS, DEFROM, sizeof(DEFROM));

	if (pccore.ROM_TYPE >= 2) {
		if ((hdl = file_open_c(OEMTEXT("IPLROM.X1T"))) != FILEH_INVALID) {
			file_read(hdl, mBIOS, 0x8000);
			file_close(hdl);
		}
	}
	else if (pccore.ROM_TYPE == 1) {
		if ((hdl = file_open_c(OEMTEXT("IPLROM.X1"))) != FILEH_INVALID) {
			file_read(hdl, mBIOS, 0x8000);
			file_close(hdl);
		}
	}
}


/***********************************************************************
	初期化
***********************************************************************/

static BRESULT reset_x1(BYTE ROM_TYPE, BYTE SOUND_SW, BYTE DIP_SW) {

	pccore.HSYNC_CLK = 250;
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

	ipl_load();

	Z80_RESET();
	iocore_reset();

	RAM0r = mBIOS;
	RAM0w = mMAIN;
	h_cntbase = 0;
	sysmng_cpureset();

	calendar_initialize();

	cgrom_reset();
	cmt_reset();
	crtc_reset();
	ctc_reset();
	dmac_reset();
	fdc_reset();
	memio_reset();
	pcg_reset();
	ppi_reset();
	sio_reset();
	sndboard_reset();
	subcpu_reset();
	vramio_reset();

	pal_reset();
	makescrn_reset();
	timing_reset();
	return(SUCCESS);
}



/***********************************************************************
	実行／終了
***********************************************************************/

static	BYTE	keyintcnt = 0;

void pccore_initialize(void) {

	fddfile_initialize();
	sndctrl_initialize();
	makescrn_initialize();

	font_load(NULL, TRUE);

	crtc_initialize();
	pcg_initialize();
	ppi_initialize();
}

void pccore_reset(void) {

	soundmng_stop();
	if (corestat.soundrenewal) {
		corestat.soundrenewal = 0;
		sndctrl_deinitialize();
		sndctrl_initialize();
	}
	sound_reset();

	reset_x1(xmilcfg.ROM_TYPE, xmilcfg.SOUND_SW, xmilcfg.DIP_SW);
	soundmng_play();
}

void pccore_deinitialize(void) {

	sndctrl_deinitialize();

//	keyboard_term();

	fddfile_eject(0);
	fddfile_eject(1);
	fddfile_eject(2);
	fddfile_eject(3);
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

void pccore_exec(BRESULT draw) {

	REG8	inttiming;

	corestat.drawframe = draw;
	soundmng_sync();

	v_cnt = 0;
	s_cnt = 0;
	inttiming = xmilcfg.CPU8MHz & 1;

	while(s_cnt < 266) {
		while(h_cnt < pccore.HSYNC_CLK) {
#if defined(TRACE) && IPTRACE
			treip[trpos & (IPTRACE - 1)] = Z80_PC;
			trpos++;
#endif
			Z80_EXECUTE();
			z80dmap();
		}
		fdcisbusy();
		h_cnt -= pccore.HSYNC_CLK;
		h_cntbase += pccore.HSYNC_CLK;
		inttiming ^= 2;
		if (inttiming != 3) {
			sound_makesample(pcmbufsize[s_cnt]);
//			if (xmilcfg.SOUNDPLY) {
//				juliet2_exec();
//			}
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
		if (crtc.s.CRT_YL == v_cnt) {
			pcg.r.vsync = 1;
			if (xmilcfg.DISPSYNC & 1) {
				scrnupdate();
			}
		}
	}
	scrnupdate();
	calendar_inc();
	sound_sync();
}

