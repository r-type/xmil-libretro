#include	"compiler.h"
#include	"dosio.h"
#include	"newdisk.h"
#include	"fddfile.h"


// ---- fdd

void newdisk_fdd(const OEMCHAR *fname, REG8 type, const char *label) {

	_D88HEAD	d88head;
	FILEH		fh;

	ZeroMemory(&d88head, sizeof(d88head));
	STOREINTELDWORD(d88head.fd_size, sizeof(d88head));
	milstr_ncpy((char *)d88head.fd_name, label, sizeof(d88head.fd_name));
	d88head.fd_type = type;
	fh = file_create(fname);
	if (fh != FILEH_INVALID) {
		file_write(fh, &d88head, sizeof(d88head));
		file_close(fh);
	}
}

