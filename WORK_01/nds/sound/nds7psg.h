
union tagNds7PsgReg
{
	struct
	{
		UINT8 cTune[3][2];		// 0
		UINT8 cNoise;			// 6
		UINT8 cMixer;			// 7
		UINT8 cVol[3];			// 8
		UINT8 cEnvTime[2];		// b
		UINT8 cEnv;				// d
		UINT8 _cIo1;
		UINT8 _cIo2;
	} r;
	UINT8 b[0x10];
};
typedef union tagNds7PsgReg		NDS7PSGREG;

struct tagNds7Psg
{
	NDS7PSGREG reg;
	UINT uClock;

	UINT8 cVol[4];
//	UINT uFreq[4];
	UINT32 uLastClock;
	UINT32 uEnvCount;
	UINT32 uEnvMax;
	UINT8 __mixer;		// unused
	UINT8 cEnvMode;
	UINT8 cEnvVol;
	SINT8 cEnvVolCount;
};
typedef struct tagNds7Psg		NDS7PSG;
typedef struct tagNds7Psg		*PNDS7PSG;


#ifdef __cplusplus
extern "C"
{
#endif

extern NDS7PSG nds7psg;

void nds7psg_reset(PNDS7PSG pPsg);
void nds7psg_sync(PNDS7PSG pPsg);

#ifdef __cplusplus
}
#endif

