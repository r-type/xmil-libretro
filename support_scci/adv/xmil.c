#include	"compiler.h"
#ifndef __GNUC__
#include	<winnls32.h>
#endif
#include	"xmil.h"
#if defined(ADV_SIMULATE)
#include	"dosio.h"
#endif
#include	"sysmng.h"
#include	"z80core.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"joymng.h"
#include	"timing.h"
#include	"makescrn.h"
#include	"fddfile.h"
#include	"advpsg.h"
#include	"font.h"
#include	"xmilopen.h"


	XMILOSCFG	xmiloscfg = {0, 0};

#define	MAX_FRAMESKIP		4


#if defined(ADV_SIMULATE)
static void diskimgset1(REG8 drv, const char *fname) {

	FILEH	fh;
	UINT	size;
	UINT8	*ptr;

	fh = file_open_rb(fname);
	if (fh == FILEH_INVALID) {
		goto dis_err1;
	}
	size = file_getsize(fh);
	if (size == 0) {
		goto dis_err2;
	}
	ptr = (UINT8 *)_MALLOC(size, "disk image");
	if (ptr == NULL) {
		goto dis_err2;
	}
	if (file_read(fh, ptr, size) != size) {
		goto dis_err3;
	}
	file_close(fh);
	fddfile_set(drv, FTYPE_D88, ptr, size);
	return;

dis_err3:
	_MFREE(ptr);

dis_err2:
	file_close(fh);

dis_err1:
	return;
}

static void diskimgset(void) {

	diskimgset1(0, "d:\\prv_project\\xmil\\bin\\agb\\fdd0.d88");
	diskimgset1(1, "d:\\prv_project\\xmil\\bin\\agb\\fdd1.d88");
}
#else

static const char fdd0[] = "fdd0.d88";
static const char fdd1[] = "fdd1.d88";
static const char fddsig[10] = "abesrludyx";

static void diskimgset(void) {

const UINT8	*ptr;
	UINT	size;

	// デモ用に
#if 1
	UINT16 key;
	key = *REG_KEYINPUT;
	key = ((~key) & 0x3ff);
	if (key) {
		int i;
		char path[10];
		for (i=0; i<9; i++) {
			if (key & (1 << i)) {
				break;
			}
		}
		path[0] = fddsig[i];
		CopyMemory(path + 1, fdd0, 9);
		ptr = FileMapping(path, &size);
		if (ptr) {
			fddfile_set(0, FTYPE_D88, ptr, size);
		}
		path[1 + 3] = '1';
		ptr = FileMapping(path, &size);
		if (ptr) {
			fddfile_set(1, FTYPE_D88, ptr, size);
		}
		return;
	}
#endif

	ptr = FileMapping(fdd0, &size);
	if (ptr) {
		fddfile_set(0, FTYPE_D88, ptr, size);
	}
	ptr = FileMapping(fdd1, &size);
	if (ptr) {
		fddfile_set(1, FTYPE_D88, ptr, size);
	}
}
#endif


// ----


#if 0
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {

	case WM_MOVE:
		if ((!scrnmng_isfullscreen()) &&
			(!(GetWindowLong(hWnd, GWL_STYLE) &
								(WS_MAXIMIZE | WS_MINIMIZE)))) {
			GetWindowRect(hWnd, &rc);
			xmiloscfg.winx = rc.left;
			xmiloscfg.winy = rc.top;
			sysmng_update(SYS_UPDATEOSCFG);
		}
		break;
}
#endif


// ----

typedef struct {
	UINT	framecnt;
	UINT	waitcnt;
	UINT	framemax;
} XMILMAIN;


static void framereset(XMILMAIN *xmm, UINT cnt) {

	xmm->framecnt = 0;
	sysmng_dispclock();
}

static void processwait(XMILMAIN *xmm, UINT cnt) {

	if (timing_getcount() >= cnt) {
		timing_setcount(0);
		framereset(xmm, cnt);
	}
#if defined(ADV_SIMULATE)
	else {
		Sleep(1);
	}
#endif
}

static void exec1frame(XMILMAIN *xmm) {

	joymng_setflags();
	advpsg_sync(&advpsg);
	pccore_exec((BRESULT)(xmm->framecnt == 0));
	xmm->framecnt++;
}

int advmain(int argc, char **argv) {

	XMILMAIN	xmilmain;

#if !defined(ADV_SIMULATE)
	if ((sizeof(z80core) != 0x40) || (sizeof(dma) != 0x40) ||
		(sizeof(iocore) != 0x214) || (sizeof(crtc) != 0x44) ||
		(sizeof(advpsg) != 0x30)) {
		while(1) { }
	}
#endif

	TRACEINIT();

	xmilopen();

	advv_setmode(ADVV_OBJ | ADVV_BG2 | ADVV_MODE4);

	CopyMemory(font_ank, __extromimage.ank, 0x800);

	sysmng_initialize();

	pccore_initialize();
	pccore_reset();

	diskimgset();

	xmilmain.framecnt = 0;
	xmilmain.waitcnt = 0;
	xmilmain.framemax = 1;

	sysmng_clockreset();


#if !defined(ADV_SIMULATE)
	// 純正ROMタイミング
	*((volatile UINT16 *)0x04000204) = 0x4317;
#endif

	// 自己責任で！！ EWRAM cycles 3/3/6 -> 2/2/4
//	*((volatile UINT32 *)0x04000800) = 0x0e000020;

#if 1
	while(advsys_task()) {
		if (xmiloscfg.NOWAIT) {
			exec1frame(&xmilmain);
			if (xmiloscfg.DRAW_SKIP) {		// nowait frame skip
				if (xmilmain.framecnt >= xmiloscfg.DRAW_SKIP) {
					processwait(&xmilmain, 0);
				}
			}
			else {							// nowait auto skip
				if (timing_getcount()) {
					processwait(&xmilmain, 0);
				}
			}
		}
		else if (xmiloscfg.DRAW_SKIP) {		// frame skip
			if (xmilmain.framecnt < xmiloscfg.DRAW_SKIP) {
				exec1frame(&xmilmain);
			}
			else {
				processwait(&xmilmain, xmiloscfg.DRAW_SKIP);
			}
		}
		else {								// auto skip
			if (!xmilmain.waitcnt) {
				UINT cnt;
				exec1frame(&xmilmain);
				cnt = timing_getcount();
				if (xmilmain.framecnt > cnt) {
					xmilmain.waitcnt = xmilmain.framecnt;
					if (xmilmain.framemax > 1) {
						xmilmain.framemax--;
					}
				}
				else if (xmilmain.framecnt >= xmilmain.framemax) {
					if (xmilmain.framemax < MAX_FRAMESKIP) {
						xmilmain.framemax++;
					}
					if (cnt >= MAX_FRAMESKIP) {
						timing_reset();
					}
					else {
						timing_setcount(cnt - xmilmain.framecnt);
					}
					framereset(&xmilmain, 0);
				}
			}
			else {
				processwait(&xmilmain, xmilmain.waitcnt);
				xmilmain.waitcnt = xmilmain.framecnt;
			}
		}
	}
#else
	while(advsys_task()) {
		if (xmilmain.framecnt < 2) {
			exec1frame(&xmilmain);
		}
		else {
			processwait(&xmilmain, 2);
		}
	}
#endif

	pccore_deinitialize();

	TRACETERM();

	return(0);
}

