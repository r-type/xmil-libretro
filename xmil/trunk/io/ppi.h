
typedef struct {
	UINT8	PORT_A;
	UINT8	PORT_B;
	UINT8	PORT_C;
	UINT8	MODE;
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

