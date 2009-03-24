

// ---- PSG

union tagNdsPsgReg
{
	struct
	{
		UINT8 cTune[3][2];		// 0
		UINT8 cNoise;			// 6
		UINT8 cMixer;			// 7
		UINT8 cVol[3];			// 8
		UINT8 cEnvTime[2];		// b
		UINT8 cEnv;				// d
	} r;
	UINT8 b[0x0e];
};
typedef union tagNdsPsgReg		NDSPSGREG;

struct tagNdsPsgXfer
{
	NDSPSGREG reg;
	UINT8 cUpdateReg;
	UINT8 cUpdateEnv;
	UINT32 uClock;
};
typedef struct tagNdsPsgXfer		NDSPSGXFER;
typedef struct tagNdsPsgXfer		*PNDSPSGXFER;


// --- Keyboard

#define SOFTKBD_KEYCODEMASK		0x7f
#define SOFTKBD_KEYDOWN			0x00
#define SOFTKBD_KEYUP			0x80
#define SOFTKBD_KEYFLAGBIT		0x80
#define SOFTKBD_NC				0xff

struct tagNdsKbdXfer
{
	UINT8 cKey;
};
typedef struct tagNdsKbdXfer		NDSKBDXFER;
typedef struct tagNdsKbdXfer		*PNDSKBDXFER;


// ---- Master

struct tagNdsIpcXfer
{
	NDSPSGXFER psg;
	NDSKBDXFER kbd;
};
typedef struct tagNdsIpcXfer		NDSIPCXFER;
typedef struct tagNdsIpcXfer		*PNDSIPCXFER;

#define	IPCXFERBASE			((PNDSIPCXFER)(IPCBASE))



#define	IPCPSGXFER			(&IPCXFERBASE->psg)
#define	IPCKEYXFER			(&IPCXFERBASE->kbd)

