
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


extern	RGB32	TEXTPALS[24];
extern	RGB32	GRPHPALS[2][16];
extern	RGB32	GRPHPALS64[2][64];
extern	BYTE	pal_renewal;

void textpalette1(BYTE num);
void grphpalette1(BYTE bank, BYTE num);
void grphpal4096(WORD num);
void reflesh_palette(void);

void pals_cnv16pal(RGB16 *dst, const RGB32 *src, UINT pals);
void pal_makegrad(RGB32 *pal, int pals, DWORD bg, DWORD fg);

#ifdef __cplusplus
}
#endif

