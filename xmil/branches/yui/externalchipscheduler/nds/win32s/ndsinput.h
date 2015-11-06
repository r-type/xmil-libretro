
enum
{
	KEY_A			= 1 << 0,
	KEY_B			= 1 << 1,
	KEY_SELECT		= 1 << 2,
	KEY_START		= 1 << 3,
	KEY_RIGHT		= 1 << 4,
	KEY_LEFT		= 1 << 5,
	KEY_UP			= 1 << 6,
	KEY_DOWN		= 1 << 7,
	KEY_R			= 1 << 8,
	KEY_L			= 1 << 9,
	KEY_X			= 1 << 10,
	KEY_Y			= 1 << 11,
	KEY_TOUCH		= 1 << 12,
	KEY_LID			= 1 << 13,
};

extern UINT16 g_wKeyReg;

#define	REG_KEYINPUT		g_wKeyReg


void ndsinput_initialize();
void ndsinput_keydown(UINT16 wKey);
void ndsinput_keyup(UINT16 wKey);

