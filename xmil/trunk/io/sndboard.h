
typedef struct {
	UINT8	psgreg;
	UINT8	opmreg;
	UINT8	rapids;
} SNDBOARD;


#ifdef __cplusplus
extern "C" {
#endif

void IOOUTCALL sndboard_opmreg(UINT port, REG8 value);
void IOOUTCALL sndboard_opmdat(UINT port, REG8 value);
REG8 IOINPCALL sndboard_opmsta(UINT port);

void IOOUTCALL sndboard_psgreg(UINT port, REG8 value);
void IOOUTCALL sndboard_psgdat(UINT port, REG8 value);
REG8 IOINPCALL sndboard_psgsta(UINT port);

void sndboard_reset(void);

#ifdef __cplusplus
}
#endif

