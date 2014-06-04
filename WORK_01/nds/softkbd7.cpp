#include "compiler.h"
#include "libnds.h"
#include "ipcxfer.h"
#include "softkbd7.h"


static UINT8 getKey(int x, int y)
{
	x = x - 10;
	y = y - 118;

	if ((y < 0) || (y >= 72))
	{
		return SOFTKBD_NC;
	}

	const int nPosY = y / 12;

	if ((x >= 0) && (x < 186))
	{
		// full-key
		if (nPosY == 0)
		{
			if (x < 90)
			{
				return 0x62 + (x / 18);		/* Function */
			}
			else if (x < 93)
			{
				return 0x37;	/* ROLLUP */
			}
			else if (x < 123)
			{
				return 0x36;	/* ROLLDOWN */
			}
			else if (x < 125)
			{
				return SOFTKBD_NC;
			}
			else if (x < 140)
			{
				return 0x3f;	/* HELP */
			}
			else if (x < 155)
			{
				return 0x61;	/* COPY */
			}
		}
		else if (nPosY == 5)
		{
			if (x < 24)
			{
				return SOFTKBD_NC;
			}
			else if (x < 36)
			{
				return 0x73;	/* GRPH */
			}
			else if (x < 48)
			{
				return 0x71;	/* CAPS */
			}
			else if (x < 138)
			{
				return 0x34;	/* SPACE */
			}
			else if (x < 150)
			{
				return 0x72;	/* KANA */
			}
			else if (x < 162)
			{
				return 0x35;	/* XFER */
			}
		}
		else
		{
			struct tagFullKey
			{
				UINT8 cWidth;
				UINT8 cCtrl;
				UINT8 cKey[14];
			};
			static const tagFullKey fkTable[4] =
			{
				/*	 ESC   1    2    3    4    5    6    7    8    9    0    -    ^    \    BRK*/
				{12,0x00,{0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x60}},
				/*	HTAB  Q    W    E    R    T    Y    U    I    O    P    @    [    INS  RET*/
				{18,0x0f,{0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1a,0x1b,0x38,0x1c}},
				/*	CTRL  A    S    D    F    G    H    J    K    L    ;    :    ]    RET  RET*/
				{20,0x0f,{0x1d,0x1e,0x1f,0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x1c,0x1c}},
				/*	SFT   Z    X    C    V    B    N    M    ,    .    /    _    SFT  SFT  SFT*/
				{24,0x70,{0x29,0x2a,0x2b,0x2c,0x2d,0x2e,0x2f,0x30,0x31,0x32,0x33,0x70,0x70,0x70}},
			};
			const tagFullKey *pcFK = fkTable + (nPosY - 1);
			int nPosX = x - pcFK->cWidth;
			if (x < 0)
			{
				return pcFK->cCtrl;
			}
			else
			{
				return pcFK->cKey[nPosX / 12];
			}
		}
	}
	else if ((x >= 191) && (x < 249))
	{
		const int nPosX = (x - 191) / 12;
		static const UINT8 sTenKey[6][4] = 
		{
			/*	 HOME /    *    -    */
				{0x3e,0x41,0x45,0x40},
			/*	 7    8    9    +	*/
				{0x42,0x43,0x44,0x49},
			/*	 4    5    6    =	*/
				{0x46,0x47,0x48,0x4d},
			/*	 1    2    3    .	*/
				{0x4a,0x4b,0x4c,0x50},
			/*	 0    ,    U    RET	*/
				{0x4e,0x4f,0x3a,0x1c},
			/*	 L    R    D    RET	*/
				{0x3b,0x3c,0x3d,0x1c},
		};
		return sTenKey[nPosY][nPosX];
	}

	return SOFTKBD_NC;
}


// ----

#define KEYBUFFERS		(1 << 4)

struct tagSoftKeyBuffer
{
	int nPos;
	int nCount;
	UINT8 cLastKey;
	UINT8 sBuffer[KEYBUFFERS];
};

static tagSoftKeyBuffer s_skb;

void softkbd7_initialize(void)
{
	tagSoftKeyBuffer *pSkb = &s_skb;
	ZeroMemory(pSkb, sizeof(*pSkb));
	pSkb->cLastKey = SOFTKBD_NC;
}

bool softkbd7_down(int x, int y)
{
	tagSoftKeyBuffer *pSkb = &s_skb;
	const UINT8 cKey = getKey(x, y);
	const bool bResult = ((cKey != SOFTKBD_NC) && (cKey != pSkb->cLastKey) && ((pSkb->nCount + 2) < KEYBUFFERS));
	if (bResult)
	{
		softkbd7_up();
		pSkb->cLastKey = cKey;
		pSkb->sBuffer[(pSkb->nPos + pSkb->nCount) % KEYBUFFERS] = cKey | SOFTKBD_KEYDOWN;
		pSkb->nCount++;
	}
	return bResult;
}

bool softkbd7_up(void)
{
	tagSoftKeyBuffer *pSkb = &s_skb;
	const UINT8 cLastKey = pSkb->cLastKey;
	const bool bResult = (cLastKey != SOFTKBD_NC);
	if (bResult)
	{
		pSkb->cLastKey = SOFTKBD_NC;
		pSkb->sBuffer[(pSkb->nPos + pSkb->nCount) % KEYBUFFERS] = cLastKey | SOFTKBD_KEYUP;
		pSkb->nCount++;
	}
	return bResult;
}

void softkbd7_sync(void)
{
	tagSoftKeyBuffer *pSkb = &s_skb;
	PNDSKBDXFER pKbd = IPCKEYXFER;

	if ((pSkb->nCount) && (pKbd->cKey == SOFTKBD_NC))
	{
		pKbd->cKey = pSkb->sBuffer[pSkb->nPos];
		pSkb->nPos = (pSkb->nPos + 1) % KEYBUFFERS;
		pSkb->nCount--;
	}
}

