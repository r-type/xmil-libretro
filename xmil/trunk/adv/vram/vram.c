#include	"compiler.h"
#include	"vram.h"


#if defined(ADV_SIMULATE)
	TRAM	tram[0x800];
	UINT8	gram[0x1c000];
	UINT8	vramupd[0x800];
#endif

