
void palmkbd_initialize(void);
void palmkbd_callback(void);
BOOL palmkbd_keydown(int keycode, BOOL cmd);
BOOL palmkbd_keyup(int keycode);
void palmkbd_modifiers(UINT ctrl);

enum {
	SHIFT_CODE   =   0x70,
	CAPS_CODE,
	KANA_CODE,
	GRAPH_CODE,
	CTRL_CODE
};
