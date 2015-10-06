
enum {
	ADVSNDBIT_SWEEPSHIFT	= 0,
	ADVSNDBIT_SWEEPDIR		= 3,
	ADVSNDBIT_SWEEPTIME		= 4,

	ADVSNDBIT_LENG			= 0,
	ADVSNDBIT_DUTYCYCLE		= 6,
	ADVSNDBIT_ENVSTEP		= 8,
	ADVSNDBIT_ENVDIR		= 11,
	ADVSNDBIT_ENV			= 12,

	ADVSNDBIT_FREQ			= 0,
	ADVSNDBIT_LENGEN		= 14,
	ADVSNDBIT_RESET			= 15,

	ADVSNDBIT_BANKTYPE		= 5,
	ADVSNDBIT_BANKSEL		= 6,
	ADVSNDBIT_OUTPUT		= 7,

	ADVSNDBIT_VOLRATIO		= 13,
};

#define REG_SOUND1CNT_L		((UINT16 *)(__adv_reg + 0x060))
#define REG_SOUND1CNT_H		((UINT16 *)(__adv_reg + 0x062))
#define	REG_SOUND1CNT_X		((UINT16 *)(__adv_reg + 0x064))
#define REG_SOUND2CNT_L		((UINT16 *)(__adv_reg + 0x068))
#define	REG_SOUND2CNT_H		((UINT16 *)(__adv_reg + 0x06c))
#define REG_SOUND3CNT_L		((UINT16 *)(__adv_reg + 0x070))
#define REG_SOUND3CNT_H		((UINT16 *)(__adv_reg + 0x072))
#define	REG_SOUND3CNT_X		((UINT16 *)(__adv_reg + 0x074))
#define REG_SOUND4CNT_L		((UINT16 *)(__adv_reg + 0x078))
#define REG_SOUND4CNT_H		((UINT16 *)(__adv_reg + 0x07c))

#define	REG_SOUNDCNT_L		((UINT16 *)(__adv_reg + 0x080))
#define	REG_SOUNDCNT_H		((UINT16 *)(__adv_reg + 0x082))
#define	REG_SOUNDCNT_X		((UINT16 *)(__adv_reg + 0x084))
#define REG_SOUNDBIAS		((UINT16 *)(__adv_reg + 0x088))

#define REG_WAVE_RAM0		((UINT32 *)(__adv_reg + 0x090))
#define REG_WAVE_RAM0_L		((UINT16 *)(__adv_reg + 0x090))
#define REG_WAVE_RAM0_H		((UINT16 *)(__adv_reg + 0x092))
#define REG_WAVE_RAM1		((UINT32 *)(__adv_reg + 0x094))
#define REG_WAVE_RAM1_L		((UINT16 *)(__adv_reg + 0x094))
#define REG_WAVE_RAM1_H		((UINT16 *)(__adv_reg + 0x096))
#define REG_WAVE_RAM2		((UINT32 *)(__adv_reg + 0x098))
#define REG_WAVE_RAM2_L		((UINT16 *)(__adv_reg + 0x098))
#define REG_WAVE_RAM2_H		((UINT16 *)(__adv_reg + 0x09a))
#define REG_WAVE_RAM3		((UINT32 *)(__adv_reg + 0x09c))
#define REG_WAVE_RAM3_L		((UINT16 *)(__adv_reg + 0x09c))
#define REG_WAVE_RAM3_H		((UINT16 *)(__adv_reg + 0x09e))

