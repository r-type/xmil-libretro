
#if !defined(SUPPORT_TURBOZ)
enum {
	XMILPAL_USE		= 64 + 64,
	XMILPAL_MAX		= XMILPAL_USE
};
#else
enum {
	XMILPAL_USE		= 64 + 64 + 8,
	XMILPAL_4096G	= 192,
	XMILPAL_4096T	= XMILPAL_4096G + 4096,
	XMILPAL_MAX		= XMILPAL_4096T + 8
};
#endif

#ifdef __cplusplus
extern "C" {
#endif

extern	UINT	xmil_palettes;
extern	RGB32	xmil_pal32[XMILPAL_MAX];		// xm_palette
#if defined(SUPPORT_16BPP)
extern	RGB16	xmil_pal16[XMILPAL_MAX];		// xmil_pal16
#endif


void pal_makegrad(RGB32 *pal, int pals, UINT32 bg, UINT32 fg);

#if defined(SUPPORT_TURBOZ)
void pal_settext(REG8 num);							// textpalette1
void pal_setgrph(REG8 bank, REG8 num);				// grphpalette1
void pal_setgrph4096(UINT num);						// grphpal4096
#endif

void pal_update1(const UINT8 *rgbp);
void pal_update(void);

void pal_reset(void);

#ifdef __cplusplus
}
#endif

