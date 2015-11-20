#include	"compiler.h"
#include	"strres.h"
#include	"bmpdata.h"
#include	"dosio.h"
#include	"dialogs.h"


// ---- file select

BOOL dlgs_selectfile(HWND hWnd, const FILESEL *item,
										OEMCHAR *path, UINT size, int *ro) {

	OPENFILENAME	ofn;

	if ((item == NULL) || (path == NULL) || (size == 0)) {
		return(FALSE);
	}
	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hWnd;
	ofn.lpstrFilter = item->filter;
	ofn.nFilterIndex = item->defindex;
	ofn.lpstrFile = path;
	ofn.nMaxFile = size;
	ofn.Flags = OFN_FILEMUSTEXIST;
	ofn.lpstrDefExt = item->ext;
	ofn.lpstrTitle = item->title;
	if (!GetOpenFileName(&ofn)) {
		return(FALSE);
	}
	if (ro) {
		*ro = ofn.Flags & OFN_READONLY;
	}
	return(TRUE);
}

BOOL dlgs_selectwritefile(HWND hWnd, const FILESEL *item,
										OEMCHAR *path, UINT size) {

	OPENFILENAME	ofn;

	if ((item == NULL) || (path == NULL) || (size == 0)) {
		return(FALSE);
	}
	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hWnd;
	ofn.lpstrFilter = item->filter;
	ofn.nFilterIndex = item->defindex;
	ofn.lpstrFile = path;
	ofn.nMaxFile = size;
	ofn.Flags = OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY;
	ofn.lpstrDefExt = item->ext;
	ofn.lpstrTitle = item->title;
	if (!GetSaveFileName(&ofn)) {
		return(FALSE);
	}
	return(TRUE);
}

BOOL dlgs_selectwritenum(HWND hWnd, const FILESEL *item,
											OEMCHAR *path, UINT size) {

	OEMCHAR	*file;
	OEMCHAR	*p;
	OEMCHAR	*q;
	UINT	i;
	BOOL	r;

	if ((item == NULL) || (path == NULL) || (size == 0)) {
		return(FALSE);
	}
	file = (OEMCHAR *)_MALLOC((size + 16) * sizeof(OEMCHAR), path);
	if (file == NULL) {
		return(FALSE);
	}
	p = file_getname(path);
	milstr_ncpy(file, path, size);
	file_cutname(file);
	q = file + OEMSTRLEN(file);
	for (i=0; i<10000; i++) {
		OEMSPRINTF(q, p, i);
		if (file_attr(file) == (short)-1) {
			break;
		}
	}
	r = dlgs_selectwritefile(hWnd, item, file, size);
	if (r) {
		file_cpyname(path, file, size);
	}
	_MFREE(file);
	return(r);
}
