
typedef struct {
	UINT8	porta;
	UINT8	portb;
	UINT8	portc;
	UINT8	mode;
} PPI;


#ifdef __cplusplus
extern "C" {
#endif

void IOOUTCALL ppi_o(UINT port, REG8 value);
REG8 IOINPCALL ppi_i(UINT port);

void ppi_initialize(void);
void ppi_reset(void);

#ifdef __cplusplus
}
#endif

