
#define POWER_LCD			(1 << 0)
#define POWER_2D_A			(1 << 1)
#define POWER_MATRIX		(1 << 2)
#define POWER_3D_CORE		(1 << 3)
#define POWER_2D_B			(1 << 9)
#define POWER_SWAP_LCDS		(1 << 15)

#define POWER_ALL_2D     (POWER_LCD |POWER_2D_A |POWER_2D_B)
#define POWER_ALL		 (POWER_ALL_2D | POWER_3D_CORE | POWER_MATRIX)

#define POWER_SOUND       (1 << 0)
#define POWER_UNKNOWN     (1 << 1)

#define	powerON(on)			do { } while(0)

