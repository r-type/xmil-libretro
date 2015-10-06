#include "compiler.h"
#include "z80core.h"
#include "pccore.h"
#include "iocore.h"


/* 8255 PPI` */

#if !defined(USE_ARMROUTINE)
static REG8 getportb(void)
{
	REG8	cPpib;
	REG8	cRet;

	cPpib = iocore.s.ppib;
	cRet = cPpib;

	iocore.s.ppib = (UINT8)((cPpib & (~0x40)) | 0x01);

	return cRet;
}
#endif	/* !defined(USE_ARMROUTINE) */

void ppi_setportc(PPI *pPpi, REG8 cValue)
{
	REG8	cModify;

	cModify = pPpi->portc ^ cValue;
	pPpi->portc = cValue;
//	cmt_write((REG8)(cValue & 1));
	if ((cModify & 0x20) && (!(cValue & 0x20)))
	{
		iocore.s.mode = 1;
	}
	if (cModify & 0x40)
	{
		crtc_setwidth((REG8)(cValue & 0x40));
	}
}


// ----

#if !defined(USE_ARMROUTINE)
void IOOUTCALL ppi_o(UINT uPort, REG8 cValue)
{
	REG8	cBit;

	switch(uPort & 0x0f)
	{
		case 0:
			ppi.porta = cValue;
			return;

		case 1:
			ppi.portb = cValue;
			return;

		case 2:
			ppi_setportc(&ppi, cValue);
			break;

		case 3:
			if (cValue & 0x80)
			{
				ppi.mode = cValue;
				return;
			}
			else
			{
				cBit = 1 << ((cValue >> 1) & 7);
				if (cValue & 0x01)
				{
					ppi_setportc(&ppi, (REG8)(ppi.portc | cBit));
				}
				else
				{
					ppi_setportc(&ppi, (REG8)(ppi.portc & (~cBit)));
				}
			}
			break;
	}
}

REG8 IOINPCALL ppi_i(UINT uPort)
{
	switch(uPort & 0x0f)
	{
		case 0:
			return ppi.porta;

		case 1:
			if (!(ppi.mode & 0x02))
			{
				return ppi.portb;
			}
			return getportb();

		case 2:
			/* mode? */
			return ppi.portc;

		case 3:
			return ppi.mode;
	}
	return 0xff;
}
#endif	/* !defined(USE_ARMROUTINE) */


/* initialize & reset */

void ppi_initialize(void)
{
	ppi.porta = 0x00;
	ppi.portb = 0xff;
	ppi.portc = 0xff;
	ppi.mode = 0x82;
}

void ppi_reset(void)
{
	ppi.porta = 0x00;
	ppi.portc |= 0x40;
	ppi.mode = 0x82;
}

