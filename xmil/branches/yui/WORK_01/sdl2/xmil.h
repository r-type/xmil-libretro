
typedef struct {
	BYTE	NOWAIT;
	BYTE	DRAW_SKIP;
} XMILOSCFG;


#if defined(SIZE_QVGA)
enum {
	FULLSCREEN_WIDTH	= 320,
	FULLSCREEN_HEIGHT	= 240
};
#else
enum {
	FULLSCREEN_WIDTH	= 640,
	FULLSCREEN_HEIGHT	= 400
};
#endif

extern	XMILOSCFG	xmiloscfg;

extern int xmil_main(int argc, char *argv[]);
