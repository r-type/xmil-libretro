#include	"compiler.h"
#include	"dosio.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"fddfile.h"
#include	"fdd_2d.h"
#include	"fdd_mtr.h"


static	WORD		sec_now[4] = {0, 0, 0, 0};
static	BYTE		sec_write[4] = {0, 0, 0, 0};
static	BYTE		sec_data[4][256];
static	BYTE		hole = 0;


BYTE changesector2d(void) {

	FDDFILE	fdd;
	FILEH	fh;
	WORD	sec;
	long	seekp;
	short	drv = fdc.drv;

	if (fdc.media) {
		return(1);
	}
	fdd = fddfile + drv;

	sec = (WORD)(((fdc.c) << 5) + (fdc.h << 4) + fdc.r - 1);
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




//**********************************************************************

short fdd_crc_2d(void) {

	fdc.crc_dat[0] = fdc.c;
	fdc.crc_dat[1] = fdc.h;
	fdc.crc_dat[2] = fdc.r;
	fdc.crc_dat[3] = 1;
	fdc.crc_dat[4] = 0;			// CRC(Lo)
	fdc.crc_dat[5] = 0;			// CRC(Hi)
	fdc.rreg = fdc.c;			// ??? ÒÙÍÝ³Þª°Ù
	return(0);
}


BYTE fdd_stat_2d(void) {

	FDDFILE	fdd;
	REG8	cmd;
	BYTE	type;
	BYTE	ans = 0;

	fdd = fddfile + fdc.drv;

	// NOT READY
	if (fdd->type != DISKTYPE_BETA) {
		return(0x80);
	}
	type = fdc.type;
	cmd = (REG8)(fdc.cmd >> 4);

	if (type == 0 || type == 1 || type == 4 ||					// !!!
		cmd == 0x0a || cmd == 0x0b || cmd == 0x0f) {
		if (fdd->protect) {				// WRITE PROTECT
			ans |= 0x40;
		}
	}
	if ((type == 1 || type == 2) &&
		((fdc.c >= 40) || (fdc.h > 1) || (fdc.r > 16) || (fdc.media))) {
		ans |= 0x10;					// SEEK ERROR / RECORD NOT FOUND
	}

	if (type == 1 || type == 4) {
		if (type == 1) {
			ans |= 0x20;					// HEAD ENGAGED (X1 ÃÞÊ ÂÈÆ 1)
			if (fdc.c == 0) {
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
		if ((type == 2) && (fdc.r) && (fdc.off < 256)) {
			ans |= 0x03;					// DATA REQUEST / BUSY
		}
		else if (cmd == 0x0f) {
			ans |= 0x04;					// error!
		}
	}
	return(ans);
}


//**********************************************************************

void fdd_read_2d(void) {

	if (((fdd_stat_2d() & 0xf3) == 3) && (!changesector2d())) {
		fdc.data = sec_data[fdc.drv][fdc.off];
	}
}


void fdd_write_2d(void) {

	if (((fdd_stat_2d() & 0xf3) == 3) && (!changesector2d())) {
		sec_data[fdc.drv][fdc.off] = fdc.data;
		sec_write[fdc.drv] = 1;
	}
}


BYTE fdd_incoff_2d(void) {

	REG8	cmd;

	if (++fdc.off < 256) {
		return(0);
	}
	cmd = (REG8)(fdc.cmd >> 4);
	if ((cmd == 0x09) || (cmd == 0x0b)) {
		if (fdc.r < 16) {
			fdc.r++;
			fdc.rreg++;
			fdc.off = 0;
			return(0);
		}
		fdc.rreg = fdc.r + 1;
	}
	fdc.off = 256;
	return(1);
}



// ----

BRESULT fdd2d_set(FDDFILE fdd, REG8 drv, const OEMCHAR *fname) {

	short	attr;

	attr = file_attr(fname);
	if (attr & 0x18) {
		return(FAILURE);
	}
	file_cpyname(fdd->fname, fname, NELEMENTS(fdd->fname));
	fdd->type = DISKTYPE_BETA;
	fdd->protect = (UINT8)(attr & 1);
	sec_now[drv] = -1;
	sec_write[drv] = 0;
	return(SUCCESS);
}

BRESULT fdd2d_eject(FDDFILE fdd, REG8 drv) {

	FILEH	fh;
	long	seekp;
	BRESULT	ret = 0;

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
	fdd->type = DISKTYPE_NOTREADY;
	sec_now[drv] = -1;
	sec_write[drv] = 0;
	return(ret);
}

