
#ifndef MEMCALL
#define	MEMCALL
#endif


#ifdef __cplusplus
extern "C" {
#endif

REG8 MEMCALL z80mem_read8(UINT addr);
void MEMCALL z80mem_write8(UINT addr, REG8 value);

#ifdef __cplusplus
}
#endif

