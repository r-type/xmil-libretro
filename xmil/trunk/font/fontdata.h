
enum {
	FONT_ANK8		= 0x01,
	FONT_ANK16a		= 0x02,
	FONT_ANK16b		= 0x04,
	FONT_KNJ1		= 0x08,
	FONT_KNJ2		= 0x10,

	FONTLOAD_KNJ	= (FONT_KNJ1 | FONT_KNJ2),
	FONTLOAD_ANK	= (FONT_ANK16a | FONT_ANK16b),
	FONTLOAD_16		= (FONTLOAD_ANK | FONTLOAD_KNJ),
	FONTLOAD_ALL	= (FONT_ANK8 | FONTLOAD_16)
};


#ifdef __cplusplus
extern "C" {
#endif

extern const OEMCHAR x1ank1name[];
extern const OEMCHAR x1ank2name[];
extern const OEMCHAR x1knjname[];
extern const OEMCHAR x1ank2tmp[];
extern const OEMCHAR x1knjtmp[];

extern const UINT8 defrom_ank[0x800];
extern const UINT8 defrom_txt[];

REG8 x1fontread(const OEMCHAR *filename, REG8 loading);

#ifdef __cplusplus
}
#endif

