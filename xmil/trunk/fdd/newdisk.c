#include	"compiler.h"
#include	"dosio.h"
#include	"newdisk.h"
#include	"fddfile.h"
#if defined(OSLANG_UTF8) || defined(OSLANG_UCS2)
#include	"oemtext.h"
#endif


/* fdd */

void newdisk_fdd(const OEMCHAR *fname, REG8 type, const OEMCHAR *label) {

	_D88HEAD	d88head;
	FILEH		fh;
	UINT8		tmp[D88_TRACKMAX][4];

	ZeroMemory(&d88head, sizeof(d88head));
	ZeroMemory(tmp, sizeof(tmp));
	STOREINTELDWORD(d88head.fd_size, D88_HEADERSIZE);
#if defined(OSLANG_UTF8) || defined(OSLANG_UCS2)
	oemtext_oem2sjis((char *)d88head.fd_name, sizeof(d88head.fd_name),
															label, (UINT)-1);
#else
	milstr_ncpy((char *)d88head.fd_name, label, sizeof(d88head.fd_name));
#endif
	d88head.fd_type = type;
	fh = file_create(fname);
	if (fh != FILEH_INVALID) {
		file_write(fh, &d88head, sizeof(d88head));
		file_write(fh, tmp, sizeof(tmp));
		file_close(fh);
	}
}

