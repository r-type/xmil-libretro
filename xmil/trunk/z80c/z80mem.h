
#ifndef MEMCALL
#define	MEMCALL
#endif


#ifdef __cplusplus
extern "C" {
#endif

REG8 MEMCALL mem_read8(UINT addr);
REG16 MEMCALL mem_read16(UINT addr);
void MEMCALL mem_write8(UINT addr, REG8 value);
void MEMCALL mem_write16(UINT addr, REG16 value);
SINT MEMCALL mem_read8s(UINT addr);

// REG8 MEMCALL Z80_RDMEM(REG16 addr);
// void MEMCALL Z80_WRMEM(REG16 addr, REG8 value);

#ifdef __cplusplus
}
#endif

