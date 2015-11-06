#include	"compiler.h"
#include	"timemng.h"


static const _SYSTIME deftime = {1990, 1, 0, 1, 0, 0, 0, 0};


BRESULT timemng_gettime(_SYSTIME *systime) {

	CopyMemory(systime, &deftime, sizeof(deftime));
	return(SUCCESS);
}

