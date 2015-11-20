
typedef struct {
const OEMCHAR	*title;
const OEMCHAR	*ext;
const OEMCHAR	*filter;
	int			defindex;
} FILESEL;

BOOL dlgs_selectfile(HWND hWnd, const FILESEL *item,
										OEMCHAR *path, UINT size, int *ro);
BOOL dlgs_selectwritefile(HWND hWnd, const FILESEL *item,
										OEMCHAR *path, UINT size);
BOOL dlgs_selectwritenum(HWND hWnd, const FILESEL *item,
										OEMCHAR *path, UINT size);
