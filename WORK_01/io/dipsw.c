#include	"compiler.h"
#include	"pccore.h"
#include	"iocore.h"


REG8 IOINPCALL dipsw_i(UINT port) {

	(void)port;
	return(pccore.DIP_SW);
}

