#include "compiler.h"
#include "joymng.h"

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

static SDL_Joystick *s_joystick = NULL;		/*!< SDL Joystick handle */
static REG8 s_joyflag = 0xff;				/*!< Joystick bits */

static REG8 s_joybtn[4] = {JOY_BTN1_BIT, JOY_BTN2_BIT, JOY_BTN3_BIT, JOY_BTN4_BIT};

/**
 * Initialize
 */
void joymng_initialize(void)
{
	if (SDL_Init(SDL_INIT_JOYSTICK) >= 0)
	{
		s_joystick = SDL_JoystickOpen(0);
	}
}

/**
 * Deinitialize
 */
void joymng_deinitialize(void)
{
	if (s_joystick)
	{
		SDL_JoystickClose(s_joystick);
	}
}

void joymng_axismotion(const SDL_Event *event)
{
	REG8 bit;

	if (event->jaxis.which == 0)
	{
		if (event->jaxis.axis == 0)
		{
			bit = s_joyflag & (~(JOY_LEFT_BIT | JOY_RIGHT_BIT));
			if (event->jaxis.value > -16384)
			{
				bit |= JOY_LEFT_BIT;
			}
			if (event->jaxis.value < 16384)
			{
				bit |= JOY_RIGHT_BIT;
			}
			s_joyflag = bit;
		}
		else if (event->jaxis.axis == 1)
		{
			bit = s_joyflag & (~(JOY_UP_BIT | JOY_DOWN_BIT));
			if (event->jaxis.value > -16384)
			{
				bit |= JOY_UP_BIT;
			}
			if (event->jaxis.value < 16384)
			{
				bit |= JOY_DOWN_BIT;
			}
			s_joyflag = bit;

		}
	}
}

void joymng_buttondown(const SDL_Event *event)
{
	if (event->jbutton.which == 0)
	{
		if ((event->jbutton.button >= 0) && (event->jbutton.button < SDL_arraysize(s_joybtn)))
		{
			s_joyflag &= ~s_joybtn[event->jbutton.button];
		}
	}
}

void joymng_buttonup(const SDL_Event *event)
{
	if (event->jbutton.which == 0)
	{
		if ((event->jbutton.button >= 0) && (event->jbutton.button < SDL_arraysize(s_joybtn)))
		{
			s_joyflag |= s_joybtn[event->jbutton.button];
		}
	}
}

REG8 joymng_getstat(void)
{
	return s_joyflag;
}
