
typedef struct {
	UINT8	vsync;
	UINT8	woff1;
	UINT8	woff2;
	UINT8	woff4;
	UINT8	roff0;
	UINT8	roff1;
	UINT8	roff2;
	UINT8	roff4;
} PCGREG;

typedef struct {
	UINT8	b[256][8];
	UINT8	r[256][8];
	UINT8	g[256][8];
} PCGDAT;

typedef struct {
	PCGDAT	d;
	PCGREG	r;
} PCG;


#ifdef __cplusplus
extern "C" {
#endif

void IOOUTCALL pcg_o(UINT port, REG8 value);		// x1_pcg_w
REG8 IOINPCALL pcg_i(UINT port);					// x1_pcg_r

void pcg_initialize(void);
void pcg_reset(void);

#ifdef __cplusplus
}
#endif

