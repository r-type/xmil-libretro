
enum {
	MAKETEXT_ROW	= 8,
	MAKETEXT_STEP	= 24
};


#ifdef __cplusplus
extern "C" {
#endif

extern const UINT8 patx2[256];
extern const UINT32 to256colex[256];

#define	PATx2LEFT(p)		(makescrn.patx2[(p) >> 4])
#define	PATx2RIGHT(p)		(makescrn.patx2[(p) >> 0])


// ---- make pattern

typedef void (LONG_CALL * MAKETXTFN)(UINT8 *dst, UINT8 *term, const UINT8 *src);
extern const MAKETXTFN maketxtlfn[32];

void LONG_CALL maketxt8_make(UINT8 *dst, UINT8 *term, const UINT8 *src, REG8 type, REG8 attr);
void LONG_CALL makepcg8_make(UINT8 *dst, UINT8 *term, const UINT8 *src, REG8 type, REG8 attr);


// ---- make attributes

typedef void (LONG_CALL * MAKEATRFN)(UINT8 *dst, UINT8 *term);
extern const MAKEATRFN makeatr8[16];


// ---- make character

void makechr8(UINT8 *dst, const MAKESCRN *m, UINT32 pos_cy, REG8 udtmp);
void makechr16(UINT8 *dst, UINT pos, UINT count, REG8 udtmp);


// ---- make mixer

void LONG_CALL makemix_mixstep(BRESULT x2);
void LONG_CALL makemix_mixtext40(UINT8 *dst, const UINT8 *txt, UINT count);
void LONG_CALL makemix_mixgrph40(UINT8 *dst, const UINT8 *grp, UINT count);
void LONG_CALL makemix_mixtext80(UINT8 *dst, const UINT8 *txt, UINT count);
void LONG_CALL makemix_mixgrph80(UINT8 *dst, const UINT8 *grp, UINT count);


// ---- make screens

void LONG_CALL width40x25_200l(UINT pos);			// 40x25 200line
void LONG_CALL width80x25_200l(UINT pos);			// 80x25 200line
void LONG_CALL width40x12_200l(UINT pos);			// 40x12 200line
void LONG_CALL width80x12_200l(UINT pos);			// 80x12 200line

#if 0
void width80x20l(void);								// 80x20
void width80x10l(void);								// 80x10
void width80x25_400h(void);							// 80x25 400line
void width80x12_400h(void);							// 80x12 400line
void width80x20h(void);								// 80x20
void width80x10h(void);								// 80x10
#endif

#ifdef __cplusplus
}
#endif

