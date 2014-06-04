
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

#define	z80mem_read8(a)			mem_read8(a)
#define	z80mem_write8(a, v)		mem_write8(a, v)

#ifdef __cplusplus
}
#endif

