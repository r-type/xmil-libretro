#include	"compiler.h"


typedef struct {
	UINT16	key;
	UINT16	bit;
} KEYBIND;

static const KEYBIND keybind[10] = {
			{'X',			ADVKEY_A},
			{'Z',			ADVKEY_B},
			{'A',			ADVKEY_SELECT},
			{'S',			ADVKEY_START},
			{VK_RIGHT,		ADVKEY_RIGHT},
			{VK_LEFT,		ADVKEY_LEFT},
			{VK_UP,			ADVKEY_UP},
			{VK_DOWN,		ADVKEY_DOWN},
			{'W',			ADVKEY_R},
			{'Q',			ADVKEY_L}};


// ----

void advkey_initialize(void) {

	*REG_KEYINPUT = 0xffff;
}

void advkey_keydown(UINT16 key) {

const KEYBIND	*kb;
	UINT		r;

	kb = keybind;
	r = NELEMENTS(keybind);
	do {
		if (key == kb->key) {
			*REG_KEYINPUT &= ~kb->bit;
			break;
		}
		kb++;
	} while(--r);
}

void advkey_keyup(UINT16 key) {

const KEYBIND	*kb;
	UINT		r;

	kb = keybind;
	r = NELEMENTS(keybind);
	do {
		if (key == kb->key) {
			*REG_KEYINPUT |= kb->bit;
			break;
		}
		kb++;
	} while(--r);
}

