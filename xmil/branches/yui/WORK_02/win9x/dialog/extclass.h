
// �؂��݂�A�g���N���X�B


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
void extclass_enablemenu(HWND hWnd, BOOL enable);
void extclass_frametype(HWND hWnd, UINT8 thick);
HMENU extclass_gethmenu(HWND hWnd);
void extclass_setclientsize(HWND hwnd, int width, int height);
