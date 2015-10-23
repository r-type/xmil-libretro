#include "compiler.h"
#include "libnds.h"
#include "iocore.h"
#include "nds9psg.h"
#include "makescrn.h"


enum
{
	JOY_LEFT_BIT	= 0x04,
	JOY_RIGHT_BIT	= 0x08,
	JOY_UP_BIT		= 0x01,
	JOY_DOWN_BIT	= 0x02,
	JOY_BTN1_BIT	= 0x40,
	JOY_BTN2_BIT	= 0x20,
	JOY_BTN3_BIT	= 0x80,
	JOY_BTN4_BIT	= 0x10
};

static UINT s_uLastButton;


void joymng_setflags()
{
	const REG16 wFlag = REG_KEYINPUT;
	REG8 cJoy = 0xff;

	if (!(wFlag & KEY_L))
	{
		const UINT uModify = s_uLastButton & (~wFlag);
		s_uLastButton = wFlag;
		if (uModify & KEY_RIGHT)
		{
			if (scrnpos.x < 80)
			{
				scrnpos.x++;
			}
		}
		if (uModify & KEY_LEFT)
		{
			if (scrnpos.x)
			{
				scrnpos.x--;
			}
		}
		if (uModify & KEY_UP)
		{
			if (scrnpos.y)
			{
				scrnpos.y--;
			}
		}
		if (uModify & KEY_DOWN)
		{
			if (scrnpos.y < 25)
			{
				scrnpos.y++;
			}
		}
		if (uModify & (KEY_RIGHT | KEY_LEFT | KEY_UP | KEY_DOWN))
		{
			crtc.e.scrnallflash = 1;
		}
		return;
	}

	s_uLastButton = 0;

	if (!(wFlag & KEY_A))
	{
		cJoy ^= JOY_BTN1_BIT;
	}
	if (!(wFlag & KEY_B))
	{
		cJoy ^= JOY_BTN2_BIT;
	}
	if (!(wFlag & KEY_RIGHT))
	{
		cJoy ^= JOY_RIGHT_BIT;
	}
	if (!(wFlag & KEY_LEFT))
	{
		cJoy ^= JOY_LEFT_BIT;
	}
	if (!(wFlag & KEY_UP))
	{
		cJoy ^= JOY_UP_BIT;
	}
	if (!(wFlag & KEY_DOWN))
	{
		cJoy ^= JOY_DOWN_BIT;
	}
	// ‚±‚Á‚¿‚©‚çƒZƒbƒg‚µ‚Ä‚â‚é
	nds9psg.reg.r.cIo1 = cJoy;
}

