
enum {
	ADVV_WIDTH		= 240,
	ADVV_HEIGHT		= 160
};

enum {
	ADVV_MODE0		= 0x0000,
	ADVV_MODE1		= 0x0001,
	ADVV_MODE2		= 0x0002,
	ADVV_MODE3		= 0x0003,
	ADVV_MODE4		= 0x0004,
	ADVV_MODE5		= 0x0005,
	ADVV_PLANE		= 0x0010,
	ADVV_MAPLINEAR  = 0x0040,
	ADVV_BG0		= 0x0100,
	ADVV_BG1		= 0x0200,
	ADVV_BG2		= 0x0400,
	ADVV_BG3		= 0x0800,
	ADVV_OBJ		= 0x1000,
	ADVV_WIN0		= 0x2000,
	ADVV_WIN1		= 0x4000,
	ADVV_OBJWIN		= 0x8000
};


#ifdef __cplusplus
extern "C" {
#endif

extern	UINT16	*ADVVIDEOPAL;
extern	UINT8	*ADVVIDEOBUFFER;

void advv_setmode(UINT16 mode);
void advv_palupdate(void);
void advv_bufferupdate(void);


// ---- for win

BRESULT advv_initialize(HWND hWnd);
void advv_deinitialize(void);
void advv_update(void);
void advv_topwinui(void);
void advv_clearwinui(void);

#ifdef __cplusplus
}
#endif

