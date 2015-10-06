
typedef struct {
	UINT8	tune[3][2];		// 0
	UINT8	noise;			// 6
	UINT8	mixer;			// 7
	UINT8	vol[3];			// 8
	UINT8	envtime[2];		// b
	UINT8	env;			// d
	UINT8	io1;
	UINT8	io2;
} PSGREG;

typedef struct {
	PSGREG		reg;
	UINT8		addr;
	UINT8		envbmp;
	UINT8		ch3bank;
	UINT8		padding;
	UINT8		vol[4];
	UINT16		freq[4];
	UINT32		lastclock;
	UINT32		envcnt;
	UINT32		envmax;
	UINT8		__mixer;		// unused
	UINT8		envmode;
	UINT8		envvol;
	SINT8		envvolcnt;
} _ADVPSG, *ADVPSG;


#ifdef __cplusplus
extern "C" {
#endif

extern	_ADVPSG	advpsg;

void advpsg_reset(ADVPSG psg);
void advpsg_sync(ADVPSG psg);

void advpsg_setreg(ADVPSG psg, REG8 reg, REG8 dat);
REG8 advpsg_getreg(ADVPSG psg, REG8 reg);

#ifdef __cplusplus
}
#endif

