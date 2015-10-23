#include	"compiler.h"
#include	"dosio.h"
#include	"iocore.h"
#include	"sound.h"
#include	"sndctrl.h"
#include	"x1f.h"


#if defined(SUPPORT_X1F)

typedef struct {
	UINT	enable;
	FILEH	fh;
	UINT	counter;
	UINT	pos;
	UINT8	buf[512];
} _X1F, *X1F;

static	_X1F	x1f;


static void writedata(X1F hdl, REG16 value) {

	UINT8	*ptr;

	ptr = hdl->buf + hdl->pos;
	STOREINTELWORD(ptr, value);
	hdl->pos += 2;
	if (hdl->pos >= sizeof(hdl->buf)) {
		file_write(hdl->fh, hdl->buf, sizeof(hdl->buf));
		hdl->pos = 0;
	}
}

static void setdata(X1F hdl, REG16 value) {

	UINT	counter;

	counter = hdl->counter;
	if (counter) {
		hdl->counter = 0;
		if (counter >= 256) {
			writedata(hdl, 0);
		}
		writedata(hdl, (REG16)counter);
	}
	writedata(hdl, value);
}


/* ---- */

BRESULT x1f_open(const OEMCHAR *filename) {

	FILEH	fh;
	UINT8	buf[256];
	UINT	i;

	fh = file_create(filename);
	if (fh == FILEH_INVALID) {
		return(FAILURE);
	}
#if defined(SUPPORT_TURBOZ) || defined(SUPPORT_OPM)
	CopyMemory(buf, sndboard.opmdat, 256);
#else
	ZeroMemory(buf, 256);
#endif
	CopyMemory(buf, "X1F", 4);
	file_write(fh, buf, 256);

	x1f.enable = 1;
	x1f.fh = fh;
	x1f.counter = 0;
	x1f.pos = 0;
	for (i=0; i<14; i++) {
		x1f_psg((REG8)i, ((UINT8 *)&psggen.reg)[i]);
	}
	return(SUCCESS);
}

void x1f_close(void) {

	if (x1f.enable) {
		x1f.enable = 0;
		if (x1f.pos) {
			file_write(x1f.fh, x1f.buf, x1f.pos);
		}
		file_close(x1f.fh);
	}
}

#if defined(SUPPORT_TURBOZ) || defined(SUPPORT_OPM)
void x1f_opm(REG8 reg, REG8 dat) {

	if (x1f.enable) {
		if ((reg == 1) || (reg >= 8)) {
			setdata(&x1f, (REG16)((reg << 8) + dat));
		}
	}
}
#endif

void x1f_psg(REG8 reg, REG8 dat) {

	if (x1f.enable) {
		setdata(&x1f, (REG16)(0x0200 | reg));
		setdata(&x1f, (REG16)(0x0300 | dat));
	}
}

void x1f_sync(void) {

	if (x1f.enable) {
		x1f.counter++;
	}
}

#endif

