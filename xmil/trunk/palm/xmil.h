
typedef struct {
	int		posx;
	int		posy;

	UINT8	NOWAIT;
	UINT8	DRAW_SKIP;
	UINT8	DISPCLK;

	UINT8	HARDKEY1;
	UINT8	HARDKEY2;
	UINT8	HARDKEY4;
	UINT8	HARDKEY3;

#ifdef SUPPORT_RESUME
	UINT8	resume;
#endif
	UINT8	graffiti;
	UINT8   FIVEWAY;
	UINT8	Z80SAVE;
} XMILOSCFG;


enum {
	SCREEN_WBASE		= 80,
	SCREEN_HBASE		= 50,
	SCREEN_DEFMUL		= 8,
	FULLSCREEN_WIDTH	= 640,
	FULLSCREEN_HEIGHT	= 480
};


//extern	WindowPtr	hWndMain;
extern	BRESULT		xmilrunning;
extern	XMILOSCFG	xmiloscfg;
