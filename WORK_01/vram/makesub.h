
enum {
	MAKETEXT_ROW	= 16,
	MAKETEXT_STEP	= 64
};


#ifdef __cplusplus
extern "C" {
#endif

extern const UINT8 x2left[256];
extern const UINT8 x2right[256];
#define	PATx2LEFT(p)		x2left[(p)]
#define	PATx2RIGHT(p)		x2right[(p)]

#if (!defined(MEMOPTIMIZE)) || (MEMOPTIMIZE == 0)		// x86
extern	UINT32	to256col0[8][512];
#define	TO256COLL(d, b)		(to256col0[b][d*2+0])
#define	TO256COLR(d, b)		(to256col0[b][d*2+1])
#elif (MEMOPTIMIZE == 1)								// for Mac
extern	UINT32	to256col1[512];
#define	TO256COLL(d, b)		(to256col1[d*2+0] << b)
#define	TO256COLR(d, b)		(to256col1[d*2+1] << b)
#else													// other
extern const UINT32 to256col2[16];
#define	TO256COLL(d, b)		(to256col2[(d >> 4)] << b)
#define	TO256COLR(d, b)		(to256col2[(d & 15)] << b)
#endif

void makesub_initialize(void);


/* make pattern */

typedef void (*MAKETXTFN)(UINT8 *dst, UINT8 *term, const UINT8 *src);

extern const MAKETXTFN maketxt8fn[16];
extern const MAKETXTFN makeknj8fn[16];
extern const MAKETXTFN maketxt16fn[16];
extern const MAKETXTFN makepcg16fn[16];


/* make attributes */

typedef void (*MAKEATRFN)(UINT8 *dst, UINT8 *term);
extern const MAKEATRFN makeatr8[16];

void makeatr_pcg8(UINT8 *dst, UINT cnt,
								const UINT8 *src, REG8 atr, MAKETXTFN fn);
void makeatr_pcg16(UINT8 *dst, UINT cnt,
								const UINT8 *src, REG8 atr, MAKETXTFN fn);


/* make character */

void makechr8(UINT8 *dst, UINT pos, UINT count, REG8 udtmp);
void makechr16(UINT8 *dst, UINT pos, UINT count, REG8 udtmp);


/* make mixer */

void makemix_mixtext(UINT8 *dst, UINT align, const UINT8 *txt, UINT count);
void makemix_mixgrph(UINT8 *dst, UINT align, const UINT8 *grp, UINT count);

void makemix_settext(UINT8 *dst, UINT align, const UINT8 *txt, UINT count);
void makemix_ul20(UINT8 *dst, UINT pos);
void makemix_ul10(UINT8 *dst, UINT pos);

void makemix_cpy200(UINT8 *dst, UINT pos, UINT count);
void makemix_cpy400(UINT8 *dst, UINT pos, UINT count);


/* make screens */

void width80x25_200l(void);						// 80x25 200line
void width80x12_200l(void);						// 80x12 200line
void width80x20l(void);							// 80x20
void width80x10l(void);							// 80x10

void width80x25_200h(void);						// 80x25 200line
void width80x25_400h(void);						// 80x25 400line
void width80x12_200h(void);						// 80x12 200line
void width80x12_400h(void);						// 80x12 400line
void width80x20h(void);							// 80x20
void width80x10h(void);							// 80x10


#ifdef __cplusplus
}
#endif

