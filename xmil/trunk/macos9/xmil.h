
typedef struct {
	int		posx;
	int		posy;

	UINT8	NOWAIT;
	UINT8	DRAW_SKIP;
	UINT8	DISPCLK;

	UINT8	F11KEY;
	UINT8	F12KEY;

	UINT8	Z80SAVE;
} XMILOSCFG;


enum {
	SCREEN_WBASE		= 80,
	SCREEN_HBASE		= 50,
	SCREEN_DEFMUL		= 8,
	FULLSCREEN_WIDTH	= 640,
	FULLSCREEN_HEIGHT	= 480
};


extern	WindowPtr	hWndMain;
extern	BRESULT		xmilrunning;
extern	XMILOSCFG	xmiloscfg;

