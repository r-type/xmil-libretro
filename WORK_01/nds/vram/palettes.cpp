#include "compiler.h"
#include "libnds.h"
#include "pccore.h"
#include "iocore.h"
#include "palettes.h"
#include "makescrn.h"


// ----

typedef struct
{
	UINT16	text[8];
} PALS;

static	PALS	pals;


static void pal_settext(REG8 num)
{
	REG16	rgb;

	rgb = (num & 1)?(0x1f << 10):0;
	rgb += (num & 4)?(0x1f << 5):0;
	rgb += (num & 2)?(0x1f << 0):0;
	pals.text[num] = rgb;
	crtc.e.palandply = 1;
}


// ----

void pal_update1(const UINT8 *rgbp)
{
	static UINT16 pal[64];
	UINT16	*advpal;
	UINT	i;
	UINT	j;
	REG8	bit;
	UINT	c;
	UINT	bcnt;
	REG16	txtpal;

	advpal = pal;
	for (i=0, bit=1; i<8; i++, bit<<=1) {
		c = 0;
		if (rgbp[CRTC_PALB] & bit) {
			c += 1;
		}
		if (rgbp[CRTC_PALR] & bit) {
			c += 2;
		}
		if (rgbp[CRTC_PALG] & bit) {
			c += 4;
		}
		txtpal = pals.text[c];
		*advpal++ = (UINT16)txtpal;
		if (rgbp[CRTC_PLY] & bit) {
			for (j=1; j<8; j++) {
				*advpal++ = (UINT16)txtpal;
			}
		}
		else {
			bcnt = (rgbp[CRTC_BLACK] & 15) - 8;
			for (j=1; j<8; j++) {
				bcnt--;
				if (bcnt) {
					c = j;
				}
				else {
					c = 0;
				}
				*advpal++ = pals.text[c];
			}
		}
	}
	for (i=0; i<64; i++)
	{
		BG_PALETTE[i] = pal[i];
	}
}

void pal_update()
{
	pal_update1(crtc.s.rgbp);
}


// ----

void pal_reset()
{
	for (REG8 i=0; i<8; i++)
	{
		pal_settext(i);
	}
}

