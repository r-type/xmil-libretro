
enum tagNdsIrq
{
	IRQ_VBLANK			= 1 << 0,
	IRQ_TIMER3			= 1 << 6,

	IRQ_ALL				= (~0)
};

// Power management registers
#define PM_CONTROL_REG		0
#define PM_BATTERY_REG		1
#define PM_AMPLIFIER_REG	2
#define PM_READ_REGISTER	(1 << 7)

// PM control register bits - power control
#define PM_SOUND_AMP		(1 << 0)
#define PM_SOUND_MUTE		(1 << 1)
#define PM_BACKLIGHT_BOTTOM	(1 << 2)
#define PM_BACKLIGHT_TOP	(1 << 3)
#define PM_SYSTEM_PWR		(1 << 6)
#define PM_POWER_DOWN		(1 << 6)

void irqInit();
void irqSet(tagNdsIrq nMask, void (*handler)());
void irqEnable(uint32 uMask);

static inline void initClockIRQ() { }

// Read/write a power management register
static inline int writePowerManagement(int reg, int command) { return 0; }
static inline int readPowerManagement(int reg) { return 0; }

