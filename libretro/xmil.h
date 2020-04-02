
typedef struct {
	BYTE	NOWAIT;
	BYTE	DRAW_SKIP;
} XMILOSCFG;


#if defined(SIZE_QVGA)
enum {
	FULLSCREEN_WIDTH	= 320,
	FULLSCREEN_HEIGHT	= 200
};
#else
enum {
	FULLSCREEN_WIDTH	= 640,
	FULLSCREEN_HEIGHT	= 400
};
#endif

#define SCREEN_PITCH (FULLSCREEN_WIDTH * 2)

extern	XMILOSCFG	xmiloscfg;

extern int xmil_main(const char *floppy);
int xmil_end();

