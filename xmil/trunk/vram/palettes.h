
enum {
	XMILPAL_TOTAL	= 64 + 64 + 8
};


#ifdef __cplusplus
extern "C" {
#endif

extern	RGB32	x1n_pal32[256];			// xm_palette
extern	RGB32	x1z_pal32[4096+8];		// GRPHPAL4096

#if defined(SUPPORT_16BPP)
extern	RGB16	x1n_pal16[256];			// xmil_pal16
extern	RGB16	x1z_pal16[4096+8];		// GRPHPAL16bit
#endif

extern	int		xm_palettes;

void pal_makegrad(RGB32 *pal, int pals, UINT32 bg, UINT32 fg);

void pal_settext(REG8 num);							// textpalette1
void pal_setgrph(REG8 bank, REG8 num);				// grphpalette1
void pal_setgrph4096(UINT num);						// grphpal4096
void pal_update(void);

void pal_reset(void);

#ifdef __cplusplus
}
#endif

