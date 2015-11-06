#include "compiler.h"
#include "libnds.h"
#include "makescrn.h"
#include "vsyncff.h"

#if MAKESCRN_VRAMFF

static	UINT8	s_cEnable = 0;
static	UINT8	s_cPage = 0;

void vsyncff_init()
{
	s_cEnable = 0;
	s_cPage = 0;
}

void vsyncff_int()
{
	if (s_cEnable)
	{
		BG3_XDX = 1 << 8;
		BG3_XDY = 0;
		BG3_YDX = 0;
		BG3_YDY = 1 << 8;

		BG3_CX = (s_cPage * 256) << 8;
		BG3_CY = 0 << 8;
		s_cPage ^= 1;
	}
}

void vsyncff_turn(REG8 en)
{
	en = en & 1;
	s_cEnable = en;
	if (!en)
	{
		BG3_XDX = 1 << 8;
		BG3_XDY = 0;
		BG3_YDX = 0;
		BG3_YDY = 1 << 8;

		BG3_CX = 0 << 8;
		BG3_CY = 0 << 8;
	}
}

#else	// MAKESCRN_VRAMFF

// ƒ_ƒ~[
void vsyncff_init() { }
void vsyncff_int() { }
void vsyncff_turn(REG8 en) { }

#endif	// MAKESCRN_VRAMFF

