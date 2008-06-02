
/* Profile Base */

enum {
	PFTYPE_STR			= 0,
	PFTYPE_BOOL,
	PFTYPE_BITMAP,
	PFTYPE_BIN,
	PFTYPE_SINT8,
	PFTYPE_SINT16,
	PFTYPE_SINT32,
	PFTYPE_UINT8,
	PFTYPE_UINT16,
	PFTYPE_UINT32,
	PFTYPE_HEX8,
	PFTYPE_HEX16,
	PFTYPE_HEX32,
	PFTYPE_BYTE3,
	PFTYPE_USER,
	PFITYPE_MASK		= 0xff,

	PFFLAG_RO			= 0x0100,
	PFFLAG_MAX			= 0x0200,
	PFFLAG_AND			= 0x0400
};

typedef struct {
	char	item[10];
	UINT16	itemtype;
	void	*value;
	UINT32	arg;
} PFTBL;

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*PFREAD)(const PFTBL *item, const OEMCHAR *string);
typedef OEMCHAR *(*PFWRITE)(const PFTBL *item, OEMCHAR *string, UINT size);

void profile_iniread(const OEMCHAR *path, const char *app,
								const PFTBL *tbl, UINT count, PFREAD cb);
void profile_iniwrite(const OEMCHAR *path, const char *app,
								const PFTBL *tbl, UINT count, PFWRITE cb);

#ifdef __cplusplus
}
#endif


/* Profile File I/O */

enum {
	PFILEH_READONLY		= 0x0001,
	PFILEH_MODIFY		= 0x0002,
	PFILEH_UTF8			= 0x0004
};

typedef struct {
	char	*buffer;
	UINT	buffers;
	UINT	size;
	UINT	flag;
	OEMCHAR	path[MAX_PATH];
} _PFILEH, *PFILEH;


#ifdef __cplusplus
extern "C" {
#endif

PFILEH profile_open(const OEMCHAR *filename, UINT flag);
void profile_close(PFILEH hdl);
BRESULT profile_read(const char *app, const char *key, const OEMCHAR *def,
										OEMCHAR *ret, UINT size, PFILEH hdl);
BRESULT profile_write(const char *app, const char *key,
										const OEMCHAR *data, PFILEH hdl);

#ifdef __cplusplus
}
#endif

