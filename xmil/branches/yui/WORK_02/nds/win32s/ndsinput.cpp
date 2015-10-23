#include "compiler.h"
#include "libnds.h"


struct tagKeyBind
{
	UINT16 wKey;
	UINT16 wBit;
};

static const tagKeyBind keybind[10] =
{
	{'X',			KEY_A},
	{'Z',			KEY_B},
	{'A',			KEY_SELECT},
	{'S',			KEY_START},
	{VK_RIGHT,		KEY_RIGHT},
	{VK_LEFT,		KEY_LEFT},
	{VK_UP,			KEY_UP},
	{VK_DOWN,		KEY_DOWN},
	{'W',			KEY_R},
	{'Q',			KEY_L},
};

	UINT16 g_wKeyReg;

// ----

void ndsinput_initialize()
{
	REG_KEYINPUT = 0xffff;
}

void ndsinput_keydown(UINT16 wKey)
{
	const tagKeyBind *pKb = keybind;
	const tagKeyBind *pKbEnd = keybind + NELEMENTS(keybind);
	do
	{
		if (wKey == pKb->wKey)
		{
			REG_KEYINPUT &= ~pKb->wBit;
			break;
		}
		pKb++;
	} while(pKb < pKbEnd);
}

void ndsinput_keyup(UINT16 wKey)
{
	const tagKeyBind *pKb = keybind;
	const tagKeyBind *pKbEnd = keybind + NELEMENTS(keybind);
	do
	{
		if (wKey == pKb->wKey)
		{
			REG_KEYINPUT |= pKb->wBit;
			break;
		}
		pKb++;
	} while(pKb < pKbEnd);
}

