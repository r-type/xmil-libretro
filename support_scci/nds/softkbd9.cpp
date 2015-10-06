#include "compiler.h"
#include "libnds.h"
#include "ipcxfer.h"
#include "softkbd9.h"
#include "keystat.h"


void softkbd9_initialize(void)
{
	PNDSKBDXFER pKbd = IPCKEYXFER;
	pKbd->cKey = SOFTKBD_NC;
}

void softkbd9_sync(void)
{
	PNDSKBDXFER pKbd = IPCKEYXFER;

	const UINT8 cKey = pKbd->cKey;
	if (pKbd->cKey != SOFTKBD_NC)
	{
		pKbd->cKey = SOFTKBD_NC;
		const REG8 cCode = cKey & SOFTKBD_KEYCODEMASK;
		const REG8 cFlag = cKey & SOFTKBD_KEYFLAGBIT;
		if (cFlag == SOFTKBD_KEYDOWN)
		{
			keystat_keydown(cCode);
		}
		else
		{
			keystat_keyup(cCode);
		}
	}
}

