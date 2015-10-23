
#define	SUPPORT_DISKEXT


#include	"d88head.h"

enum {
	MAX_FDDFILE			= 4
};

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

enum {
	FDDSTAT_BUSY		= 0x01,
	FDDSTAT_INDEX		= 0x02,
	FDDSTAT_DRQ			= 0x02,
	FDDSTAT_TRACK00		= 0x04,
	FDDSTAT_LOSTDATA	= 0x04,
	FDDSTAT_CRCERR		= 0x08,
	FDDSTAT_SEEKERR		= 0x10,
	FDDSTAT_RECNFND		= 0x10,
	FDDSTAT_HEADENG		= 0x20,
	FDDSTAT_RECTYPE		= 0x20,
	FDDSTAT_WRITEFAULT	= 0x20,
	FDDSTAT_WRITEP		= 0x40,
	FDDSTAT_NOTREADY	= 0x80
};

typedef struct {
	UINT8	c;
	UINT8	h;
	UINT8	r;
	UINT8	n;
	UINT8	flag;
	UINT8	padding;
	UINT8	size[2];
} TAOSEC;


typedef struct _fddfile		_FDDFILE;
typedef struct _fddfile		*FDDFILE;

typedef REG8 (*FDDSEEK)(FDDFILE fdd, REG8 media, UINT track);
typedef REG8 (*FDDREAD)(FDDFILE fdd, REG8 media, UINT track, REG8 sc,
												UINT8 *ptr, UINT *size);
typedef REG8 (*FDDWRITE)(FDDFILE fdd, REG8 media, UINT track, REG8 sc,
												const UINT8 *ptr, UINT size);
typedef REG8 (*FDDCRC)(FDDFILE fdd, REG8 media, UINT track, UINT num,
												UINT8 *ptr);
#if defined(SUPPORT_DISKEXT)
typedef UINT32 (*FDDSEC)(FDDFILE fdd, REG8 media, UINT track, REG8 sc);
#endif

typedef struct {
	UINT32	headersize;
	UINT8	tracks;
	UINT8	sectors;
	UINT8	n;
	UINT8	media;
} _XDFINFO, *XDFINFO;

typedef struct {
	UINT32		fd_size;
	_D88HEAD	head;
	UINT32		ptr[D88_TRACKMAX];
} _D88INFO, *D88INFO;

struct _fddfile {
	UINT8		type;
	UINT8		protect;
	UINT8		padding[2];
	UINT32		ftype;
	FDDSEEK		seek;
	FDDREAD		read;
	FDDWRITE	write;
	FDDWRITE	wrtrk;
	FDDCRC		crc;
#if defined(SUPPORT_DISKEXT)
	FDDSEC		sec;
#endif
	union {
		_XDFINFO	xdf;
		_D88INFO	d88;
	} inf;
	OEMCHAR		fname[MAX_PATH];
};


#ifdef __cplusplus
extern "C" {
#endif

extern	_FDDFILE	fddfile[MAX_FDDFILE];

void fddfile_initialize(void);

const OEMCHAR *fddfile_diskname(REG8 drv);
BRESULT fddfile_diskready(REG8 drv);
BRESULT fddfile_diskprotect(REG8 drv);

BRESULT fddfile_set(REG8 drv, const OEMCHAR *fname, UINT ftype, int ro);
void fddfile_eject(REG8 drv);

#ifdef __cplusplus
}
#endif

