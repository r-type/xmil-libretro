#include	"compiler.h"
#include	"xmil.h"
#include	"dosio.h"
#include	"sysmng.h"
#include	"menu.h"
#include	"dialog.h"
#include	"dialogs.h"
#include	"x1f.h"


#if defined(SUPPORT_X1F)
static const OEMCHAR x1fui_file[] = OEMTEXT("xmil%04d.x1f");
static const OEMCHAR x1fui_title[] = OEMTEXT("Save as OPM log");
static const OEMCHAR x1fui_ext[] = OEMTEXT("x1f");
static const OEMCHAR x1fui_filter[] = OEMTEXT("OPM log (*.x1f)\0*.x1f\0");
static const FILESEL x1fui = {x1fui_title, x1fui_ext, x1fui_filter, 1};

void dialog_x1f(HWND hWnd) {

	BRESULT	check;
	OEMCHAR	path[MAX_PATH];

	x1f_close();
	check = FALSE;
	file_cpyname(path, bmpfilefolder, NELEMENTS(path));
	file_cutname(path);
	file_catname(path, x1fui_file, NELEMENTS(path));
	if ((dlgs_selectwritenum(hWnd, &x1fui, path, NELEMENTS(path))) &&
		(x1f_open(path) == SUCCESS)) {
		file_cpyname(bmpfilefolder, path, NELEMENTS(bmpfilefolder));
		sysmng_update(SYS_UPDATEOSCFG);
		check = TRUE;
	}
	menu_opmlog(check);
}
#endif

