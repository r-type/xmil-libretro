#include	"compiler.h"
#include	"dosio.h"
#include	"z80core.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"vram.h"
#include	"debugsub.h"


//	èÛë‘ìfÇ´èoÇµÅ@Ç∞ÇÎÇËÇÒÇ±

static	int		filenum = 0;

void debugsub_status(void) {

	FILEH	fh;
	OEMCHAR	work[512];

	SPRINTF(work, OEMTEXT("z80reg.%.3d"), filenum);
	fh = file_create_c(work);
	if (fh != FILEH_INVALID) {
		SPRINTF(work,	OEMTEXT("PC = %04x\nAF = %04x\nBC = %04x\n")	\
						OEMTEXT("DE = %04x\nHL = %04x\nIX = %04x\n")	\
						OEMTEXT("IY = %04x\nSP = %04x\nAF'= %04x\n")	\
						OEMTEXT("BC'= %04x\nDE'= %04x\nHL'= %04x\n\n")	\
						OEMTEXT("IM = %2d\nADRS = %02x%02x\n\n")		\
						OEMTEXT("CRT_YL = %3d\nCRT_VS = %3d\n\n")		\
						OEMTEXT("FNT_YL = %3d\nTXT_YL = %3d\nSCRN_b = %02x\n"),
						Z80_PC, Z80_AF, Z80_BC, Z80_DE,
						Z80_HL, Z80_IX, Z80_IY, Z80_SP,
						Z80_AF2, Z80_BC2, Z80_DE2, Z80_HL2,
						Z80_IM, Z80_I, subcpu.Ex[4][0],
						crtc.s.CRT_YL, crtc.s.CRT_VS,
						crtc.s.FNT_YL, crtc.s.TXT_YL, crtc.s.SCRN_BITS);
		file_write(fh, work, STRLEN(work) * sizeof(OEMCHAR));
		file_close(fh);
	}

	SPRINTF(work, OEMTEXT("z80ram.%.3d"), filenum);
	fh = file_create_c(work);
	if (fh != FILEH_INVALID) {
		file_write(fh, mMAIN, 0x10000);
		file_close(fh);
	}
	SPRINTF(work, OEMTEXT("x1vram1.%.3d"), filenum);
	fh = file_create_c(work);
	if (fh != FILEH_INVALID) {
		file_write(fh, &GRP_RAM[GRAM_BANK0 + 0x0000], 0x8000);
		file_write(fh, &GRP_RAM[GRAM_BANK0 + 0x8000], 0x8000);
		file_close(fh);
	}
	SPRINTF(work, OEMTEXT("x1vram2.%.3d"), filenum);
	fh = file_create_c(work);
	if (fh != FILEH_INVALID) {
		file_write(fh, &GRP_RAM[GRAM_BANK1 + 0x0000], 0x8000);
		file_write(fh, &GRP_RAM[GRAM_BANK1 + 0x8000], 0x8000);
		file_close(fh);
	}
	SPRINTF(work, OEMTEXT("x1tram.%.3d"), filenum);
	fh = file_create_c(work);
	if (fh != FILEH_INVALID) {
		file_write(fh, tram, 0x1800);
		file_close(fh);
	}
	filenum++;
}

