
#ifndef SCRNCALL
#define	SCRNCALL
#endif

enum {
	SURFACE_WIDTH		= 640,
	SURFACE_HEIGHT		= 400,
	SURFACE_SIZE		= (SURFACE_WIDTH * SURFACE_HEIGHT)
};

enum {
	SCRNWIDTHMODE_WIDTH80		= 0,
	SCRNWIDTHMODE_WIDTH40		= 1,
	SCRNWIDTHMODE_4096			= 2
};

typedef struct {
	UINT8	widthmode;
} SCRN;


#ifdef __cplusplus
extern "C" {
#endif

extern	SCRN	scrn;
extern	UINT8	renewalline[SURFACE_HEIGHT+4];
extern	UINT8	screenmap[SURFACE_SIZE];

void scrndraw_initialize(void);
void scrndraw_changewidth(REG8 widthmode);	// ddraws_change_xmode
void scrndraw_changepalette(void);			// ddraws_change_palette
REG8 scrndraw_draw(REG8 redraw);
void scrndraw_redraw(void);					// ddraws_redraw

#ifdef __cplusplus
}
#endif

