
enum {
	PCGDAT_B		= 0x0000,
	PCGDAT_R		= 0x0800,
	PCGDAT_G		= 0x1000
};

typedef struct {
	UINT8	d[0x1800];
} PCG;


#ifdef __cplusplus
extern "C" {
#endif

void IOOUTCALL pcg_o(UINT port, REG8 value);
REG8 IOINPCALL pcg_i(UINT port);

void pcg_initialize(void);
void pcg_reset(void);

#ifdef __cplusplus
}
#endif

