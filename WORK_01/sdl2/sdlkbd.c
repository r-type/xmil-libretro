/**
 *	@file	sdlkbd.c
 *	@brief	Implementation of the keyboard
 */

#include "compiler.h"
#include "sdlkbd.h"
#include "keystat.h"

/**
 * @brief SDL Key tables
 */
struct TagSdlKey
{
	SDL_Keycode code;		/*!< SDL key */
	UINT8 data;				/*!< PC-98 serial data */
};

#define NC			0xff	/*!< N/C */

/** Key table */
static const struct TagSdlKey s_table[] =
{
	{SDLK_ESCAPE,		0x00},	{SDLK_1,			0x01},
	{SDLK_2,			0x02},	{SDLK_3,			0x03},
	{SDLK_4,			0x04},	{SDLK_5,			0x05},
	{SDLK_6,			0x06},	{SDLK_7,			0x07},

	{SDLK_8,			0x08},	{SDLK_9,			0x09},
	{SDLK_0,			0x0a},	{SDLK_MINUS,		0x0b},
	{SDLK_EQUALS,		0x0c},	{SDLK_BACKSLASH,	0x0d},
	{SDLK_BACKSPACE,	0x0e},	{SDLK_TAB,			0x0f},

	{SDLK_q,			0x10},	{SDLK_w,			0x11},
	{SDLK_e,			0x12},	{SDLK_r,			0x13},
	{SDLK_t,			0x14},	{SDLK_y,			0x15},
	{SDLK_u,			0x16},	{SDLK_i,			0x17},

	{SDLK_o,			0x18},	{SDLK_p,			0x19},
	{SDLK_RETURN,		0x1c},	{SDLK_a,			0x1d},
	{SDLK_s,			0x1e},	{SDLK_d,			0x1f},

	{SDLK_f,			0x20},	{SDLK_g,			0x21},
	{SDLK_h,			0x22},	{SDLK_j,			0x23},
	{SDLK_k,			0x24},	{SDLK_l,			0x25},

								{SDLK_z,			0x29},
	{SDLK_x,			0x2a},	{SDLK_c,			0x2b},
	{SDLK_v,			0x2c},	{SDLK_b,			0x2d},
	{SDLK_n,			0x2e},	{SDLK_m,			0x2f},

	{SDLK_COMMA,		0x30},	{SDLK_PERIOD,		0x31},
	{SDLK_SLASH,		0x32},
	{SDLK_SPACE,		0x34},
	{SDLK_PAGEUP,		0x36},	{SDLK_PAGEDOWN,		0x37},

	{SDLK_INSERT,		0x38},	{SDLK_DELETE,		0x39},
	{SDLK_UP,			0x3a},	{SDLK_LEFT,			0x3b},
	{SDLK_RIGHT,		0x3c},	{SDLK_DOWN,			0x3d},
	{SDLK_HOME,			0x3e},	{SDLK_END,			0x3f},

	{SDLK_KP_MINUS,		0x40},	{SDLK_KP_DIVIDE,	0x41},
	{SDLK_KP_7,			0x42},	{SDLK_KP_8,			0x43},
	{SDLK_KP_9,			0x44},	{SDLK_KP_MULTIPLY,	0x45},
	{SDLK_KP_4,			0x46},	{SDLK_KP_5,			0x47},

	{SDLK_KP_6,			0x48},	{SDLK_KP_PLUS,		0x49},
	{SDLK_KP_1,			0x4a},	{SDLK_KP_2,			0x4b},
	{SDLK_KP_3,			0x4c},
	{SDLK_KP_0,			0x4e},

	{SDLK_KP_PERIOD,	0x50},

	{SDLK_PAUSE,		0x60},	{SDLK_PRINTSCREEN,	0x61},
	{SDLK_F1,			0x62},	{SDLK_F2,			0x63},
	{SDLK_F3,			0x64},	{SDLK_F4,			0x65},
	{SDLK_F5,			0x66},	{SDLK_F6,			0x67},

	{SDLK_F7,			0x68},	{SDLK_F8,			0x69},
	{SDLK_F9,			0x6a},	{SDLK_F10,			0x6b},

	{SDLK_RSHIFT,		0x70},	{SDLK_LSHIFT,		0x70},
	{SDLK_CAPSLOCK,		0x71},
	{SDLK_RALT,			0x73},	{SDLK_LALT,			0x73},
	{SDLK_RCTRL,		0x74},	{SDLK_LCTRL,		0x74},

	{SDLK_KP_EQUALS,	0x4d},
};


/**
 * Converts code to serial-data
 * @param[in] code SDL key code
 * @return serial-data
 */
static UINT8 GetKeyData(SDL_Keycode code)
{
	size_t i;

	for (i = 0; i < SDL_arraysize(s_table); i++)
	{
		if (s_table[i].code == code)
		{
			return s_table[i].data;
		}
	}
	return NC;
}

/**
 * This method is called by the framework when a key is pressed
 * @param[in] code SDL keycode
 */
void sdlkbd_keydown(SDL_Keycode code)
{
	UINT8 data;

	data = GetKeyData(code);
	if (data != NC)
	{
		keystat_senddata(data);
	}
}

/**
 * This method is called by the framework when a key is released
 * @param[in] code SDL keycode
 */
void sdlkbd_keyup(SDL_Keycode code)
{
	UINT8 data;

	data = GetKeyData(code);
	if (data != NC)
	{
		keystat_senddata((UINT8)(data | 0x80));
	}
}
