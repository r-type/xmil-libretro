/**
 * @file	joymng.cpp
 * @brief	�W���C�p�b�h�̓���̒�`���s���܂�
 */

#include "compiler.h"
#include "joymng.h"
#include "pccore.h"
#include "xmil.h"

#pragma comment(lib, "winmm.lib")

/**
 * �r�b�g
 */
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

//! �L���t���O
static bool s_bEnabled = false;
//! �X�e�[�^�X
static UINT8 s_cJoyFlag = 0;

/**
 * ������
 */
void joymng_initialize()
{
	JOYINFO ji;
	s_bEnabled = ((joyGetNumDevs() != 0) && (joyGetPos(JOYSTICKID1, &ji) == JOYERR_NOERROR));
}

/**
 * �L��?
 * @return �L���t���O
 */
bool joymng_isEnabled()
{
	return s_bEnabled;
}

/**
 * �V���N
 */
void joymng_sync()
{
	s_cJoyFlag = 0;
}

/**
 * �X�e�[�^�X�𓾂�
 * @return �X�e�[�^�X
 */
REG8 joymng_getstat(void)
{
	if (s_cJoyFlag == 0)
	{
		UINT8 cJoyFlag = 0xff;
		JOYINFO ji;
		if ((xmiloscfg.JOYSTICK) && (s_bEnabled) && (joyGetPos(JOYSTICKID1, &ji) == JOYERR_NOERROR))
		{
			if (ji.wXpos < 0x4000U)
			{
				cJoyFlag &= ~JOY_LEFT_BIT;
			}
			else if (ji.wXpos > 0xc000U)
			{
				cJoyFlag &= ~JOY_RIGHT_BIT;
			}
			if (ji.wYpos < 0x4000U)
			{
				cJoyFlag &= ~JOY_UP_BIT;
			}
			else if (ji.wYpos > 0xc000U)
			{
				cJoyFlag &= ~JOY_DOWN_BIT;
			}
			if (ji.wButtons & JOY_BUTTON1)
			{
				cJoyFlag &= ~JOY_BTN1_BIT;
			}
			if (ji.wButtons & JOY_BUTTON2)
			{
				cJoyFlag &= ~JOY_BTN2_BIT;
			}
			if (ji.wButtons & JOY_BUTTON3)
			{
				cJoyFlag &= ~JOY_BTN3_BIT;
			}
			if (ji.wButtons & JOY_BUTTON4)
			{
				cJoyFlag &= ~JOY_BTN4_BIT;
			}
		}
		s_cJoyFlag = cJoyFlag;
	}
	return s_cJoyFlag;
}

