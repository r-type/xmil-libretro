
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

void IOOUTCALL cgrom_o(UINT port, REG8 value);		// x1_knj_w
REG8 IOINPCALL cgrom_i(UINT port);					// x1_knj_r

void cgrom_reset(void);

#ifdef __cplusplus
}
#endif

