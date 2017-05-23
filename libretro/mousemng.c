#include	"compiler.h"
#include	"mousemng.h"


REG8 mousemng_getstat(SINT16 *x, SINT16 *y, int clear) {

	*x = 0;
	*y = 0;
	(void)clear;
	return(0xa0);
}

