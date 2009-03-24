#include "compiler.h"
#include "libnds.h"
#include "xmil7.h"
#if defined(_WIN32) && defined(NDS_SIMULATE)
#include "dosio.h"
#endif
#include "nds7psg.h"
#include "softkbd7.h"

static void touch()
{
	static uint16 s_wLastButtons = (uint16)-1;

	touchPosition pos;
	ZeroMemory(&pos, sizeof(pos));

	uint16 wButtons = REG_KEYXY;
	if (!((wButtons ^ s_wLastButtons) & (1 << 6)))
	{
		pos = touchReadXY();
		if ((pos.x == 0) || (pos.y == 0))
		{
			wButtons |= (1 << 6);
			s_wLastButtons = wButtons;
		}
	}
	else
	{
		s_wLastButtons = wButtons;
		wButtons |= (1 << 6);
	}

	IPC->touchX = pos.x;
	IPC->touchY = pos.y;
	IPC->touchXpx = pos.px;
	IPC->touchYpx = pos.py;
	IPC->touchZ1 = pos.z1;
	IPC->touchZ2 = pos.z2;
	IPC->buttons = wButtons;
}


static void VblankHandler()
{
	touch();

	const uint16 wButtons = IPC->buttons;
	if (!(wButtons & (1 << 6)))
	{
		softkbd7_down(IPC->touchXpx, IPC->touchYpx);
	}
	else
	{
		softkbd7_up();
	}
	softkbd7_sync();
}


int nds7main()
{
	// read User Settings from firmware
	readUserSettings();

	//enable sound
	powerON(POWER_SOUND);
	writePowerManagement(PM_CONTROL_REG, (readPowerManagement(PM_CONTROL_REG) & (~PM_SOUND_MUTE)) | PM_SOUND_AMP);
	SOUND_CR = SOUND_ENABLE | SOUND_VOL(0x7F);

	softkbd7_initialize();

	irqInit();

	// Start the RTC tracking IRQ
	initClockIRQ();

	irqSet(IRQ_VBLANK, VblankHandler);
	irqEnable(IRQ_VBLANK);

	NDS7PSG psg;
	nds7psg_reset(&psg);

	while(1)
	{
		swiWaitForVBlank();
		nds7psg_sync(&psg);
	}

	return 0;
}

