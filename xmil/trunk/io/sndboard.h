
typedef struct {
	UINT8	psgreg;
	UINT8	opmreg;
	UINT8	rapids;
	UINT8	padding;
	UINT8	psgdat[0x10];
	UINT8	opmdat[0x100];
} SNDBOARD;


#ifdef __cplusplus
extern "C" {
#endif

void IOOUTCALL opm_o(UINT port, REG8 dat);
REG8 IOINPCALL opm_i(UINT port);

void IOOUTCALL sndboard_psgreg(UINT port, REG8 dat);
void IOOUTCALL sndboard_psgdat(UINT port, REG8 dat);
REG8 IOINPCALL sndboard_psgsta(UINT port);

void sndboard_reset(void);

#ifdef __cplusplus
}
#endif

