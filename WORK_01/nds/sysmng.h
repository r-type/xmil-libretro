
#define DISPCLOCK

// Ç«Å[Ç≈Ç‡Ç¢Ç¢í ímån

enum
{
	SYS_UPDATECFG		= 0x0001,
	SYS_UPDATEOSCFG		= 0x0002,
	SYS_UPDATEFDD		= 0x0004
};

#define	sysmng_cpureset()

#if defined(DISPCLOCK) && (!defined(NDS_SIMULATE))

void sysmng_initialize();
void sysmng_clockreset();
void sysmng_dispclock();

#else	// defined(DISPCLOCK) && (!defined(NDS_SIMULATE))

#define	sysmng_initialize()		do { } while(0)
#define	sysmng_clockreset()		do { } while(0)
#define	sysmng_dispclock()		do { } while(0)

#endif	// defined(DISPCLOCK) && (!defined(NDS_SIMULATE))


#if !defined(TRACE)

#ifdef __cplusplus
extern "C"
{
#endif
void sysmng_fddaccess(UINT8 cDrv);
void sysmng_fddsync(UINT uFrames);
#ifdef __cplusplus
}
#endif

#else	// !defined(TRACE)

#define sysmng_fddaccess(cDrv)		do { } while(0)
#define sysmng_fddsync(uFrames)		do { } while(0)

#endif	// !defined(TRACE)

