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


void dialog_writebmp(void) {

	SCRNSAVE	ss;
	char		path[MAX_PATH];
const char		*ext;

	ss = scrnsave_get();
	if (ss == NULL) {
		return;
	}
	if (dlgs_selectwritefile(path, sizeof(path), "xmil.bmp")) {
		ext = file_getext(path);
		if ((ss->type <= SCRNSAVE_8BIT) &&
			(!file_cmpname(ext, "gif"))) {
			scrnsave_writegif(ss, path, SCRNSAVE_AUTO);
		}
		else if (!file_cmpname(ext, str_bmp)) {
			scrnsave_writebmp(ss, path, SCRNSAVE_AUTO);
		}
	}
	scrnsave_trash(ss);
}

