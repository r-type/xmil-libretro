
typedef struct {
	UINT8	porta;
	UINT8	portb;
	UINT8	portc;
	UINT8	mode;
} PPI;


#ifdef __cplusplus
extern "C" {
#endif

void IOOUTCALL ppi_o(UINT port, REG8 value);		// x1_8255_w
REG8 IOINPCALL ppi_i(UINT port);					// x1_8255_r

void ppi_initialize(void);
void ppi_reset(void);

#ifdef __cplusplus
}
#endif

