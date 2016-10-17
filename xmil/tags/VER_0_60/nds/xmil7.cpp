#include "compiler.h"
#include "libnds.h"
#include "xmil7.h"
#if defined(_WIN32) && defined(NDS_SIMULATE)
#include "dosio.h"
#endif
#include "sound/nds7psg.h"
#include "softkbd7.h"

static void touch()
{
	static uint16 s_wLastButtons = static_cast<uint16>(-1);

	touchPosition pos;
	ZeroMemory(&pos, sizeof(pos));

	uint16 wButtons = REG_KEYXY;
	if (!((wButtons ^ s_wLastButtons) & (1 << 6)))
	{
#if (LIBNDS_VERSION >= 0x010302)
		touchReadXY(&pos);
		const bool bRel = ((pos.rawx == 0) || (pos.rawy == 0));
#else	// (LIBNDS_VERSION >= 0x010302)
		pos = touchReadXY();
		const bool bRel = ((pos.x == 0) || (pos.y == 0));
#endif	// (LIBNDS_VERSION >= 0x010302)
		if (bRel)
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

	TransferRegion volatile* pIpc = getIPC();
#if (LIBNDS_VERSION >= 0x010302)
	pIpc->touchX = pos.rawx;
	pIpc->touchY = pos.rawy;
#else	// (LIBNDS_VERSION >= 0x010302)
	pIpc->touchX = pos.x;
	pIpc->touchY = pos.y;
#endif	// (LIBNDS_VERSION >= 0x010302)
	pIpc->touchXpx = pos.px;
	pIpc->touchYpx = pos.py;
	pIpc->touchZ1 = pos.z1;
	pIpc->touchZ2 = pos.z2;
	pIpc->buttons = wButtons;
}


static void VblankHandler()
{
	touch();

	const TransferRegion volatile* pcIpc = getIPC();
	const uint16 wButtons = pcIpc->buttons;
	if (!(wButtons & (1 << 6)))
	{
		softkbd7_down(pcIpc->touchXpx, pcIpc->touchYpx);
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
#if (LIBNDS_VERSION >= 0x010302)
	powerOn(PM_SOUND_AMP);
#else	//  (LIBNDS_VERSION >= 0x010302)
	powerON(POWER_SOUND);
#endif	// (LIBNDS_VERSION >= 0x010302)
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

