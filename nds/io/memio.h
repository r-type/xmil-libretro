
#ifdef __cplusplus
extern "C" {
#endif

extern	UINT8	biosmem[0x8000];
#if defined(SUPPORT_BANKMEM)
extern	UINT8	bankmem[16][0x8000];
#endif

void memio_update(void);

#if defined(SUPPORT_BANKMEM)
void IOOUTCALL memio_bank_o(UINT port, REG8 dat);
REG8 IOINPCALL memio_bank_i(UINT port);
#endif

void IOOUTCALL memio_rom(UINT port, REG8 dat);
void IOOUTCALL memio_ram(UINT port, REG8 dat);

void memio_reset(void);

#ifdef __cplusplus
}
#endif

