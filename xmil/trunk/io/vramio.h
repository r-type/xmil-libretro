
#ifdef __cplusplus
extern "C" {
#endif

void IOOUTCALL tram_atr_o(UINT port, REG8 value);
void IOOUTCALL tram_ank_o(UINT port, REG8 value);
void IOOUTCALL tram_knj_o(UINT port, REG8 value);
REG8 IOOUTCALL tram_atr_i(UINT port);
REG8 IOOUTCALL tram_ank_i(UINT port);
REG8 IOOUTCALL tram_knj_i(UINT port);

void IOOUTCALL gram_o(UINT port, REG8 dat);
REG8 IOINPCALL gram_i(UINT port);
void IOOUTCALL gram2_o(UINT port, REG8 dat);

void vramio_reset(void);

#ifdef __cplusplus
}
#endif

