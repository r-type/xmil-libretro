
#define		LICENCED_LENG		64
#define		LICENCED_POS		12

extern	char	licenced[];

LRESULT CALLBACK CfgDialogProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);
LRESULT CALLBACK AboutDialogProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);

void dialog_changefdd(HWND hWnd, REG8 drv);
void dialog_newdisk(HWND hWnd);
void dialog_writebmp(HWND hWnd);
void dialog_x1f(HWND hWnd);


#if 0
void dialog_scropt(HWND hWnd);
void dialog_sndopt(HWND hWnd);
void dialog_serial(HWND hWnd);
void dialog_font(HWND hWnd);
#endif

