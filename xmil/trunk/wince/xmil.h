
typedef struct {
#if !defined(GX_DLL)
	int		winx;
	int		winy;
#endif

	UINT8	NOWAIT;
	UINT8	DRAW_SKIP;

#if defined(WIN32_PLATFORM_PSPC)
	UINT8	bindcur;
	UINT8	bindbtn;
#endif
} XMILOSCFG;


#if defined(SIZE_QVGA)
#if defined(SUPPORT_SOFTKBD)
enum {
	FULLSCREEN_WIDTH	= 320,
	FULLSCREEN_HEIGHT	= 240
};
#else
enum {
	FULLSCREEN_WIDTH	= 320,
	FULLSCREEN_HEIGHT	= 200
};
#endif
#else
#if defined(SUPPORT_SOFTKBD)
enum {
	FULLSCREEN_WIDTH	= 640,
	FULLSCREEN_HEIGHT	= 480
};
#else
enum {
	FULLSCREEN_WIDTH	= 640,
	FULLSCREEN_HEIGHT	= 400
};
#endif
#endif


extern	XMILOSCFG	xmiloscfg;
extern	HWND		hWndMain;
extern	HINSTANCE	hInst;
extern	HINSTANCE	hPreI;
extern	OEMCHAR		modulefile[MAX_PATH];

