
#ifdef STRICT
#define	SUBCLASSPROC	WNDPROC
#else
#define	SUBCLASSPROC	FARPROC
#endif

typedef struct {
const OEMCHAR	*title;
const OEMCHAR	*ext;
const OEMCHAR	*filter;
	int			defindex;
} FILESEL;

#define	SetDlgItemCheck(a, b, c)	\
			SendDlgItemMessage((a), (b), BM_SETCHECK, (c), 0)

#define	GetDlgItemCheck(a, b)		\
			(((int)SendDlgItemMessage((a), (b), BM_GETCHECK, 0, 0))?1:0)

#define	AVE(a, b)					\
			(((a) + (b)) / 2)

#define	SETLISTSTR(a, b, c)			\
			dlgs_setliststr((a), (b), (c), NELEMENTS((c)))

#define	SETnLISTSTR(a, b, c, d)		\
			dlgs_setliststr((a), (b), (c), (d))

#define	SETLISTUINT32(a, b, c)		\
			dlgs_setlistuint32((a), (b), (c), NELEMENTS((c)))


BOOL dlgs_selectfile(HWND hWnd, const FILESEL *item,
										OEMCHAR *path, UINT size, int *ro);
BOOL dlgs_selectwritefile(HWND hWnd, const FILESEL *item,
										OEMCHAR *path, UINT size);
BOOL dlgs_selectwritenum(HWND hWnd, const FILESEL *item,
										OEMCHAR *path, UINT size);

// void dlgs_setliststr(HWND hWnd, WORD res, const char **item, UINT items);
void dlgs_setlistuint32(HWND hWnd, WORD res, const UINT32 *item, UINT items);
// void dlgs_drawbmp(HDC hdc, BYTE *bmp);

