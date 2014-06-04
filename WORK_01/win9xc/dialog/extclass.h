
// ぺけみれ、拡張クラス。


#if 0
extern const OEMCHAR extclass[];

void extclass_initialize(HINSTANCE hinst);
void extclass_deinitialize(void);
void extclass_move(HWND hWnd, int posx, int posy, int cx, int cy);
int CALLBACK extclass_propetysheet(HWND hWndDlg, UINT uMsg, LPARAM lParam);
#endif


// ----

enum {
	EXTGWL_HMENU	= 0,
	EXTGWL_SIZE		= 4
};

void extclass_wmcreate(HWND hWnd);
void extclass_wmdestroy(HWND hWnd);
void extclass_enablemenu(HWND hWnd, BRESULT enable);
HMENU extclass_gethmenu(HWND hWnd);

