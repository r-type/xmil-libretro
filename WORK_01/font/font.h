
#define	FONTX1_LR	0x10000
#define	FONTX1T_LR	0x20000

#ifdef __cplusplus
extern "C" {
#endif

extern	UINT8	font_ank[0x0800];
extern	UINT8	font_txt[0x1000];
extern	UINT8	font_knjx1[0x20000];
extern	UINT8	font_knjx1t[0x40000];

UINT8 *font_adrsx1t(UINT jis);
REG8 font_load(const OEMCHAR *filename, BRESULT force);

#ifdef __cplusplus
}
#endif

