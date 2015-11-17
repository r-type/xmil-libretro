#include	"compiler.h"
#include	"pccore.h"
#include	"iocore.h"


REG8 cmt_read(void) {

	return(0);
}

void cmt_write(REG8 dat) {

	(void)dat;
}

void cmt_ctrl(REG8 cmd) {

	switch(cmd) {
		case 0x00:				/* EJECT */
		case 0x01:				/* STOP */
			/* cmt.stop = 1; */
			iocore.s.ppib &= ~0x01;
			break;
#if 0
		case 0x02:				/* PLAY */
		case 0x03:				/* FF */
		case 0x04:				/* REW */
		case 0x05:				/* APSS_FF */
		case 0x06:				/* APSS_REW */
		case 0x0a:				/* REC */
			break;
#endif	/* 0 */
	}
	cmt.cmd = (UINT8)cmd;
}

REG8 cmt_tape_stat(void) {

	return(0);
}

REG8 cmt_ctrl_stat(void) {

	return(cmt.cmd);
}

#if 0
REG8 cmt_test(void) {

	if (cmt.stop) {
		cmt.stop = 0;
		return(0);
	}
	else {
		return(1);
	}
}
#endif


/* reset */

void cmt_reset(void) {

	cmt.cmd = 0;
	/* cmt.stop = 1; */
	iocore.s.ppib &= ~0x01;
}

