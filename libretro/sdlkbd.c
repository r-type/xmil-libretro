/**
 *	@file	sdlkbd.c
 *	@brief	Implementation of the keyboard
 */

#include "compiler.h"
#include "sdlkbd.h"
#include "keystat.h"
#include "libretro.h"

/**
 * @brief SDL Key tables
 */
struct TagSdlKey
{
	int code;		/*!< SDL key */
	UINT8 data;				/*!< PC-98 serial data */
};

#define NC			0xff	/*!< N/C */

/** Key table */
static const struct TagSdlKey s_table[] =
{
	{RETROK_ESCAPE,		0x00},	{RETROK_1,			0x01},
	{RETROK_2,			0x02},	{RETROK_3,			0x03},
	{RETROK_4,			0x04},	{RETROK_5,			0x05},
	{RETROK_6,			0x06},	{RETROK_7,			0x07},

	{RETROK_8,			0x08},	{RETROK_9,			0x09},
	{RETROK_0,			0x0a},	{RETROK_MINUS,		0x0b},
	{RETROK_EQUALS,		0x0c},	{RETROK_BACKSLASH,	0x0d},
	{RETROK_BACKSPACE,	0x0e},	{RETROK_TAB,			0x0f},

	{RETROK_q,			0x10},	{RETROK_w,			0x11},
	{RETROK_e,			0x12},	{RETROK_r,			0x13},
	{RETROK_t,			0x14},	{RETROK_y,			0x15},
	{RETROK_u,			0x16},	{RETROK_i,			0x17},

	{RETROK_o,			0x18},	{RETROK_p,			0x19},
	{RETROK_RETURN,		0x1c},	{RETROK_a,			0x1d},
	{RETROK_s,			0x1e},	{RETROK_d,			0x1f},

	{RETROK_f,			0x20},	{RETROK_g,			0x21},
	{RETROK_h,			0x22},	{RETROK_j,			0x23},
	{RETROK_k,			0x24},	{RETROK_l,			0x25},

								{RETROK_z,			0x29},
	{RETROK_x,			0x2a},	{RETROK_c,			0x2b},
	{RETROK_v,			0x2c},	{RETROK_b,			0x2d},
	{RETROK_n,			0x2e},	{RETROK_m,			0x2f},

	{RETROK_COMMA,		0x30},	{RETROK_PERIOD,		0x31},
	{RETROK_SLASH,		0x32},
	{RETROK_SPACE,		0x34},
	{RETROK_PAGEUP,		0x36},	{RETROK_PAGEDOWN,		0x37},

	{RETROK_INSERT,		0x38},	{RETROK_DELETE,		0x39},
	{RETROK_UP,			0x3a},	{RETROK_LEFT,			0x3b},
	{RETROK_RIGHT,		0x3c},	{RETROK_DOWN,			0x3d},
	{RETROK_HOME,			0x3e},	{RETROK_END,			0x3f},

	{RETROK_KP_MINUS,		0x40},	{RETROK_KP_DIVIDE,	0x41},
	{RETROK_KP7,			0x42},	{RETROK_KP8,			0x43},
	{RETROK_KP9,			0x44},	{RETROK_KP_MULTIPLY,	0x45},
	{RETROK_KP4,			0x46},	{RETROK_KP5,			0x47},

	{RETROK_KP6,			0x48},	{RETROK_KP_PLUS,		0x49},
	{RETROK_KP1,			0x4a},	{RETROK_KP2,			0x4b},
	{RETROK_KP3,			0x4c},
	{RETROK_KP0,			0x4e},

	{RETROK_KP_PERIOD,	0x50},

	{RETROK_PAUSE,		0x60},	{RETROK_PRINT,	0x61},
	{RETROK_F1,			0x62},	{RETROK_F2,			0x63},
	{RETROK_F3,			0x64},	{RETROK_F4,			0x65},
	{RETROK_F5,			0x66},	{RETROK_F6,			0x67},

	{RETROK_F7,			0x68},	{RETROK_F8,			0x69},
	{RETROK_F9,			0x6a},	{RETROK_F10,			0x6b},

	{RETROK_RSHIFT,		0x70},	{RETROK_LSHIFT,		0x70},
	{RETROK_CAPSLOCK,		0x71},
	{RETROK_RALT,			0x73},	{RETROK_LALT,			0x73},
	{RETROK_RCTRL,		0x74},	{RETROK_LCTRL,		0x74},

	{RETROK_KP_EQUALS,	0x4d},
};


/**
 * Converts code to serial-data
 * @param[in] code SDL key code
 * @return serial-data
 */
static UINT8 GetKeyData(int code)
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
void sdlkbd_keydown(int code)
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
void sdlkbd_keyup(int code)
{
	UINT8 data;

	data = GetKeyData(code);
	if (data != NC)
	{
		keystat_senddata((UINT8)(data | 0x80));
	}
}
