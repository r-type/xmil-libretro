#include	"compiler.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"draw.h"


// ---- 8255 PPI�`

void IOOUTCALL ppi_o(UINT port, REG8 value) {

	UINT8	bak_c;
	UINT8	bit;
	UINT8	xl;

	if (crtc.s.TXT_XL == 40) {
		ppi.PORT_C |= 0x40;
	}
	else {
		ppi.PORT_C &= ~0x40;
	}
	bak_c = ppi.PORT_C;

	switch(port & 0x0f) {
		case 0:
			if (!(ppi.MODE & 0x10)) {
				ppi.PORT_A = value;
			}
			return;

		case 1:
			if (!(ppi.MODE & 0x02)) {
				ppi.PORT_B = value;
			}
			return;

		case 2:
			if (!(ppi.MODE & 0x01)) {
				ppi.PORT_C &= 0xf0;
				ppi.PORT_C |= (value & 0x0f);
			}
			if (!(ppi.MODE & 0x08)) {
				ppi.PORT_C &= 0x0f;
				ppi.PORT_C |= (value & 0xf0);
			}
			break;

		case 3:
			if (value & 0x80) {
				ppi.MODE = value;
				return;
			}
			else {
				bit = (UINT8)(1 << ((value >> 1) & 7));
				if (value & 0x01) {
					ppi.PORT_C |= bit;
				}
				else {
					ppi.PORT_C &= ~bit;
				}
			}
			break;

		default:
			return;
	}

//	cmt_write(ppi.PORT_C & 1);
	if ((bak_c & 0x20) && (!(ppi.PORT_C & 0x20))) {
		ppi.IO_MODE = 1;
	}
	xl = ((ppi.PORT_C & 0x40)?40:80);
	if (crtc.s.TXT_XL != xl) {
		crtc.s.TXT_XL = (UINT8)xl;
		crtc.s.GRP_XL = xl << 3;
		vrambank_patch();
		scrnallflash = 1;
	}
}

REG8 IOINPCALL ppi_i(UINT port) {

	ppi.PORT_B = cmt_test(); // | cmt_read();	// THUNDER BALL

	if (v_cnt < crtc.s.CRT_YL) {
		ppi.PORT_B |= 0x80;					// 1:DISP
	}
	if (subcpu.IBF) {
		subcpu.IBF = 0;
		ppi.PORT_B |= 0x40;					// 1:SUB-CPU BUSY
	}
	if (subcpu.OBF) {
		ppi.PORT_B |= 0x20;					// 1:SUB-CPU Data empty
	}
	if (!pccore.ROM_SW) {
		ppi.PORT_B |= 0x10;					// 1:RAM
	}
#if 1
	if (!(v_cnt < crtc.s.CRT_VS)) {
		ppi.PORT_B |= 0x04;					// V-SYNC
	}
#else								// ���v���X�e�X�g�cVYSNC����������炵��
	if (v_cnt == crtc.s.CRT_VS) {
		ppi.PORT_B |= 0x04;
	}
#endif
	if (crtc.s.TXT_XL == 40) {
		ppi.PORT_C |= 0x40;
	}
	else {
		ppi.PORT_C &= ~0x40;
	}
	switch(port & 0x0f) {
		case 0:
			return(ppi.PORT_A);

		case 1:
			return(ppi.PORT_B);

		case 2:
			return(ppi.PORT_C);

		case 3:
			return(ppi.MODE);
	}
	return(0xff);
}


// ----

void ppi_initialize(void) {

	ppi.PORT_A = 0x00;
	ppi.PORT_B = 0xff;
	ppi.PORT_C = 0xff;
	ppi.MODE = 0;
	ppi.IO_MODE = 0;
}

void ppi_reset(void) {

	ppi.MODE = 0;
	ppi.PORT_A = 0;
	ppi.PORT_C |= 0x40;
	ppi.IO_MODE = 0;
}

