#include	"compiler.h"
#include	"z80core.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"dosio.h"
#include	"debugsub.h"

#include	"x1_io.h"
#include	"x1_crtc.h"
#include	"x1_fdc.h"
// #include	"ddraws.h"


//	èÛë‘ìfÇ´èoÇµÅ@Ç∞ÇÎÇËÇÒÇ±

static	int		filenum = 0;

void debugsub_status(void) {

	FILEH	fh;
	OEMCHAR	work[512];

	SPRINTF(work, OEMTEXT("z80reg.%.3d"), filenum);
	fh = file_create_c(work);
	if (fh != FILEH_INVALID) {
		SPRINTF(work,	"PC = %04x\n"	\
						"AF = %04x\n"	\
						"BC = %04x\n"	\
						"DE = %04x\n"	\
						"HL = %04x\n"	\
						"IX = %04x\n"	\
						"IY = %04x\n"	\
						"SP = %04x\n"	\
						"AF'= %04x\n"	\
						"BC'= %04x\n"	\
						"DE'= %04x\n"	\
						"HL'= %04x\n"	\
						"\n"			\
						"IM = %2d\n"	\
						"ADRS = %02x%02x\n\n"	\
						"CRT_YL = %3d\n"		\
						"CRT_VS = %3d\n"		\
						"\n"					\
						"FNT_YL = %3d\n"		\
						"TXT_YL = %3d\n"		\
						"SCRN_b =  %02x\n",
						Z80_PC, Z80_AF, Z80_BC, Z80_DE,
						Z80_HL, Z80_IX, Z80_IY, Z80_SP,
						Z80_AF2, Z80_BC2, Z80_DE2, Z80_HL2,
						Z80_IM, Z80_I, subcpu.Ex[4][0],
						crtc.CRT_YL, crtc.CRT_VS,
						crtc.FNT_YL, crtc.TXT_YL, crtc.SCRN_BITS);
		file_write(fh, work, strlen(work));
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
		file_write(fh, TXT_RAM, 0x1800);
		file_close(fh);
	}
	filenum++;
}

