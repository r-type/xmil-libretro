
#ifdef __cplusplus
extern "C" {
#endif

REG8 MEMCALL Z80_RDMEM(REG16 addr);
void MEMCALL Z80_WRMEM(REG16 addr, REG8 value);
REG16 MEMCALL Z80_RDMEM_W(REG16 addr);
void MEMCALL Z80_WRMEM_W(REG16 addr, REG16 value);

#ifdef __cplusplus
}
#endif

