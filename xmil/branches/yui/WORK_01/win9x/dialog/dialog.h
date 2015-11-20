
#define		LICENCED_LENG		64
#define		LICENCED_POS		12

extern	char	licenced[];

void dialog_changefdd(HWND hWnd, REG8 drv);
void dialog_newdisk(HWND hWnd);
void dialog_writebmp(HWND hWnd);
#if defined(SUPPORT_X1F)
void dialog_x1f(HWND hWnd);
#endif	// defined(SUPPORT_X1F)

#if 0
void dialog_scropt(HWND hWnd);
void dialog_sndopt(HWND hWnd);
void dialog_serial(HWND hWnd);
void dialog_font(HWND hWnd);
#endif

