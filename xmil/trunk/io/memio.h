
typedef struct {
	UINT8	ram;
	UINT8	ems;
} MEMIO;


#ifdef __cplusplus
extern "C" {
#endif

void memio_update(void);

void IOOUTCALL memio_ems_o(UINT port, REG8 dat);
REG8 IOINPCALL memio_ems_i(UINT port);

void IOOUTCALL memio_rom(UINT port, REG8 dat);
void IOOUTCALL memio_ram(UINT port, REG8 dat);

void memio_reset(void);

#ifdef __cplusplus
}
#endif

