
union tagNds9PsgReg
{
	struct
	{
		UINT8 cTune[3][2];		// 0
		UINT8 cNoise;			// 6
		UINT8 cMixer;			// 7
		UINT8 cVol[3];			// 8
		UINT8 cEnvTime[2];		// b
		UINT8 cEnv;				// d
		UINT8 cIo1;
		UINT8 cIo2;
	} r;
	UINT8 b[0x10];
};
typedef union tagNds9PsgReg		NDS9PSGREG;

struct tagNds9Psg
{
	NDS9PSGREG reg;
	UINT32 uClock;
	UINT8 cAddr;
	UINT8 cUpdateReg;
	UINT8 cUpdateEnv;
};
typedef struct tagNds9Psg		NDS9PSG;
typedef struct tagNds9Psg		*PNDS9PSG;


#ifdef __cplusplus
extern "C"
{
#endif

extern NDS9PSG nds9psg;

void nds9psg_reset(PNDS9PSG pPsg);
void nds9psg_sync(PNDS9PSG pPsg);
void nds9psg_setreg(PNDS9PSG pPsg, REG8 cReg, REG8 cValue);
REG8 nds9psg_getreg(PNDS9PSG pPsg, REG8 cReg);

#ifdef __cplusplus
}
#endif

