
#ifdef __cplusplus
extern "C" {
#endif

void IOOUTCALL tram_o(UINT port, REG8 dat);			// x1_txt_w
REG8 IOINPCALL tram_i(UINT port);					// x1_txt_r

void IOOUTCALL gram_o(UINT port, REG8 dat);			// x1_grp_w
REG8 IOINPCALL gram_i(UINT port);					// x1_grp_r
void IOOUTCALL gram2_o(UINT port, REG8 dat);		// x1_grp_w2

void vramio_reset(void);

#ifdef __cplusplus
}
#endif

