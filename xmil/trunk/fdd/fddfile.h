
#define	MAX_FDDFILE		4

#include	"d88head.h"

enum {
	DISKTYPE_NOTREADY	= 0,
	DISKTYPE_BETA,
	DISKTYPE_D88
};

enum {
	DISKTYPE_2D			= 0,
	DISKTYPE_2DD,
	DISKTYPE_2HD
};

typedef struct {
	UINT32		fd_size;
	_D88HEAD	head;
	UINT32		ptr[D88_TRACKMAX];
} _D88INFO, *D88INFO;

typedef struct {
	UINT8	type;
	UINT8	protect;
	UINT8	padding[2];
	union {
		_D88INFO	d88;
	} inf;
	OEMCHAR	fname[MAX_PATH];
} _FDDFILE, *FDDFILE;


#ifdef __cplusplus
extern "C" {
#endif

extern	_FDDFILE	fddfile[MAX_FDDFILE];

void fddfile_initialize(void);

const OEMCHAR *fdd_diskname(REG8 drv);
BRESULT fdd_diskready(REG8 drv);
BRESULT fdd_diskprotect(REG8 drv);

BRESULT fdd_set(REG8 drv, const OEMCHAR *fname, UINT ftype, int ro);
BRESULT fdd_eject(REG8 drv);

#ifdef __cplusplus
}
#endif

