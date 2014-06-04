
struct tagTransferSoundData
{
	const void *data;
	UINT32 len;
	UINT32 rate;
	UINT8 vol;
	UINT8 pan;
	UINT8 format;
	UINT8 PADDING;
};
typedef struct tagTransferSoundData		TransferSoundData;
typedef struct tagTransferSoundData		*pTransferSoundData;


struct tagTransferSound
{
	TransferSoundData data[16];
	UINT8 count;
	UINT8 PADDING[3];
};
typedef struct tagTransferSound		TransferSound;
typedef struct tagTransferSound		*pTransferSound;


struct tagTransferRegion
{
	SINT16 touchX,   touchY;	// TSC X, Y
	SINT16 touchXpx, touchYpx;	// TSC X, Y pixel values
	SINT16 touchZ1,  touchZ2;	// TSC x-panel measurements
	UINT16 tdiode1,  tdiode2; 	// TSC temperature diodes
	UINT32 temperature;			// TSC computed temperature

	UINT16 buttons;				// X, Y, /PENIRQ buttons

	union
	{
		UINT8 curtime[8];		// current time response from RTC

		struct
		{
			UINT8 command;
			UINT8 year;		//add 2000 to get 4 digit year
			UINT8 month;		//1 to 12
			UINT8 day;		//1 to (days in month)

			UINT8 weekday;	// day of week
			UINT8 hours;		//0 to 11 for AM, 52 to 63 for PM
			UINT8 minutes;	//0 to 59
			UINT8 seconds;	//0 to 59
		} rtc;
	} time;
	SINT32	unixTime;

	uint16 battery;			// battery life ??  hopefully.  :)
	uint16 aux;				// i have no idea...

	pTransferSound soundData;

	UINT32 mailAddr;
	UINT32 mailData;
	UINT8 mailRead;
	UINT8 mailBusy;
	UINT32 mailSize;
};
typedef struct tagTransferRegion		TransferRegion;
typedef struct tagTransferRegion		*pTransferRegion;


extern UINT8 g_sIPC[0x1000];

static TransferRegion *getIPC() { return reinterpret_cast<TransferRegion *>(g_sIPC); }

#define IPC getIPC()

