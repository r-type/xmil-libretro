#include	"compiler.h"
#include	"dosio.h"
#include	"pccore.h"
#include	"x1_io.h"
#include	"x1_fdc.h"
#include	"fddfile.h"
#include	"fdd_mtr.h"


extern	BYTE		WRITEPT[];
extern	BYTE		DISKNUM[];
static	WORD		sec_now[4] = {0, 0, 0, 0};
static	BYTE		sec_write[4] = {0, 0, 0, 0};
static	BYTE		sec_data[4][256];
static	BYTE		hole = 0;


BYTE changesector2d(void) {

	FDDFILE	fdd;
	FILEH	fh;
	WORD	sec;
	long	seekp;
	short	drv = FDC.drv;

	if (FDC.media) {
		return(1);
	}
	fdd = fddfile + drv;

	sec = (WORD)(((FDC.c) << 5) + (FDC.h << 4) + FDC.r - 1);
	if (sec_now[drv] == sec) {
		return(0);
	}
	fh = file_open(fdd->fname);
	if (fh == FILEH_INVALID) {
		sec_now[drv] = -1;
		return(1);
	}
	if ((sec_write[drv]) && (sec_now[drv] != -1)) {
		seekp = (long)sec_now[drv] << 8;
		if ((file_seek(fh, seekp, FSEEK_SET) != seekp) ||
			(file_write(fh, sec_data[drv], 256) != 256)) {
			file_close(fh);
			sec_now[drv] = -1;
			return(1);
		}
	}
	sec_write[drv] = 0;
	sec_now[drv] = sec;
	seekp = (long)sec << 8;
	if ((file_seek(fh, seekp, FSEEK_SET) != seekp) ||
		(file_read(fh, sec_data[drv], 256) != 256)) {
		file_close(fh);
		sec_now[drv] = -1;
		return(1);
	}
	file_close(fh);
	return(0);
}

BRESULT fdd_eject_2d(REG8 drv) {

	FDDFILE	fdd;
	FILEH	fh;
	long	seekp;
	BRESULT	ret = 0;

	fdd = fddfile + drv;
	while(1) {
		if ((!sec_write[drv]) || (sec_now[drv] == -1)) {
			break;
		}
		seekp = (long)sec_now[drv] << 8;
		fh = file_open(fdd->fname);
		if (fh == FILEH_INVALID) {
			ret = 1;
			break;
		}
		if ((file_seek(fh, seekp, FSEEK_SET) != seekp) ||
			(file_write(fh, sec_data[drv], 256) != 256)) {
			ret = 1;
		}
		if (file_close(fh)) {
			ret = 1;
		}
		break;
	}
	fdd->fname[0] = '\0';
	DISKNUM[drv] = 0;
	sec_now[drv] = -1;
	sec_write[drv] = 0;
	return(ret);
}


BRESULT fdd_set_2d(REG8 drv, const OEMCHAR *fname) {

	FDDFILE	fdd;
	WORD	attr;

	fdd_eject_2d(drv);
	fdd = fddfile + drv;
	if ((drv < 0 || drv > 3) ||
		((attr = file_attr(fname)) & 0x18)) {	// ´×°¥ÃÞ¨Ú¸ÄØ¥×ÍÞÙ
		return(1);
	}
	DISKNUM[drv] = DRV_FMT2D;
	file_cpyname(fdd->fname, fname, NELEMENTS(fdd->fname));
	WRITEPT[drv] = (BYTE)(attr & 1);
	return(0);
}

//**********************************************************************

short fdd_crc_2d(void) {

	FDC.crc_dat[0] = FDC.c;
	FDC.crc_dat[1] = FDC.h;
	FDC.crc_dat[2] = FDC.r;
	FDC.crc_dat[3] = 1;
	FDC.crc_dat[4] = 0;			// CRC(Lo)
	FDC.crc_dat[5] = 0;			// CRC(Hi)
	FDC.rreg = FDC.c;			// ??? ÒÙÍÝ³Þª°Ù
	return(0);
}


BYTE fdd_stat_2d(void) {

	BYTE	type, cmnd;
	BYTE	ans = 0;

	// NOT READY
	if (DISKNUM[FDC.drv] != DRV_FMT2D) {
		return(0x80);
	}
	type = FDC.type;
	cmnd = (BYTE)(FDC.cmnd >> 4);

	if (type == 0 || type == 1 || type == 4 ||					// !!!
		cmnd == 0x0a || cmnd == 0x0b || cmnd == 0x0f) {
		if (WRITEPT[FDC.drv]) {				// WRITE PROTECT
			ans |= 0x40;
		}
	}
	if ((type == 1 || type == 2) &&
		((FDC.c >= 40) || (FDC.h > 1) || (FDC.r > 16) || (FDC.media))) {
		ans |= 0x10;					// SEEK ERROR / RECORD NOT FOUND
	}

	if (type == 1 || type == 4) {
		if (type == 1) {
			ans |= 0x20;					// HEAD ENGAGED (X1 ÃÞÊ ÂÈÆ 1)
			if (FDC.c == 0) {
				ans |= 0x04;					// TRACK00
			}
		}
		if (++hole < 8) {
			ans |= 0x02;					// INDEX
		}
	}
	else if (!(ans & 0xf0)) {
		if (FDDMTR_BUSY) {
			ans |= 0x01;
		}
		if ((type == 2) && (FDC.r) && (FDC.off < 256)) {
			ans |= 0x03;					// DATA REQUEST / BUSY
		}
		else if (cmnd == 0x0f) {
			ans |= 0x04;					// error!
		}
	}
	return(ans);
}


//**********************************************************************

void fdd_read_2d(void) {

	if (((fdd_stat_2d() & 0xf3) == 3) && (!changesector2d())) {
		FDC.data = sec_data[FDC.drv][FDC.off];
	}
}


void fdd_write_2d(void) {

	if (((fdd_stat_2d() & 0xf3) == 3) && (!changesector2d())) {
		sec_data[FDC.drv][FDC.off] = FDC.data;
		sec_write[FDC.drv] = 1;
	}
}


BYTE fdd_incoff_2d(void) {

	BYTE cmnd;

	if (++FDC.off < 256) {
		return(0);
	}
	cmnd = (BYTE)(FDC.cmnd >> 4);
	if (cmnd == 0x09 || cmnd == 0x0b) {
		if (FDC.r < 16) {
			FDC.r++;
			FDC.rreg++;
			FDC.off = 0;
			return(0);
		}
		FDC.rreg = FDC.r + 1;
	}
	FDC.off = 256;
	return(1);
}

