#include	"compiler.h"
#include	"strres.h"
#include	"resource.h"
#include	"xmil.h"
#include	"dosio.h"
#include	"sysmng.h"
#include	"dialog.h"
#include	"dialogs.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"scrnsave.h"
#include	"font.h"


static const OEMCHAR bmpui_file[] = OEMTEXT("XMIL%04d.BMP");
static const OEMCHAR bmpui_title[] = OEMTEXT("Save as bitmap file");
static const OEMCHAR bmpui_filter1[] = OEMTEXT("1bit-bitmap (*.bmp)\0*.bmp\0Graphics Interchange Format (*.gif)\0*.gif\0");
static const OEMCHAR bmpui_filter4[] = OEMTEXT("4bit-bitmap (*.bmp)\0*.bmp\0Graphics Interchange Format (*.gif)\0*.gif\0");
static const OEMCHAR bmpui_filter8[] = OEMTEXT("8bit-bitmap (*.bmp)\0*.bmp\0Graphics Interchange Format (*.gif)\0*.gif\0");
static const OEMCHAR bmpui_filter24[] = OEMTEXT("24bit-bitmap (*.bmp)\0*.bmp\0");
static const OEMCHAR *bmpui_filter[4] = {
				bmpui_filter1, bmpui_filter4, bmpui_filter8, bmpui_filter24};


void dialog_writebmp(HWND hWnd) {

	SCRNSAVE	ss;
	FILESEL		bmpui;
	OEMCHAR		path[MAX_PATH];
const OEMCHAR	*ext;

	ss = scrnsave_get();
	if (ss == NULL) {
		return;
	}
	bmpui.title = bmpui_title;
	bmpui.ext = str_bmp;
	bmpui.filter = bmpui_filter[ss->type];
	bmpui.defindex = 1;
	file_cpyname(path, bmpfilefolder, _countof(path));
	file_cutname(path);
	file_catname(path, bmpui_file, _countof(path));
	if (dlgs_selectwritenum(hWnd, &bmpui, path, _countof(path))) {
		file_cpyname(bmpfilefolder, path, _countof(bmpfilefolder));
		sysmng_update(SYS_UPDATEOSCFG);
		ext = file_getext(path);
		if ((ss->type <= SCRNSAVE_8BIT) &&
			(!file_cmpname(ext, OEMTEXT("gif")))) {
			scrnsave_writegif(ss, path, SCRNSAVE_AUTO);
		}
		else if (!file_cmpname(ext, str_bmp)) {
			scrnsave_writebmp(ss, path, SCRNSAVE_AUTO);
		}
	}
	scrnsave_trash(ss);
}

