
typedef struct {
	UINT8	reg[2];
	UINT8	flag;
	UINT8	padding;
	UINT	face;
	UINT	count;
} CGROM;


#ifdef __cplusplus
extern "C" {
#endif

void IOOUTCALL cgrom_o(UINT port, REG8 dat);
REG8 IOINPCALL cgrom_i(UINT port);

void cgrom_reset(void);

#ifdef __cplusplus
}
#endif

