
#define	DISPCLOCK

// どーでもいい通知系

enum {
	SYS_UPDATECFG		= 0x0001,
	SYS_UPDATEOSCFG		= 0x0002,
	SYS_UPDATEFDD		= 0x0004
};


#ifdef __cplusplus
extern "C" {
#endif

extern	UINT	sys_updates;

#ifdef __cplusplus
}
#endif


// マクロ(単に関数コールしたくないだけ)
#if defined(DISPCLOCK) && (!defined(ADV_SIMULATE))
void sysmng_initialize(void);
#else
#define	sysmng_initialize()	sys_updates = 0
#endif
#define	sysmng_update(a)	sys_updates |= (a)
#define	sysmng_cpureset()	sys_updates	&= (SYS_UPDATECFG | SYS_UPDATEOSCFG)
#define sysmng_scrnwidth(m)
#define	sysmng_fddaccess(a)


#if defined(DISPCLOCK) && (!defined(ADV_SIMULATE))
void sysmng_clockreset(void);
void sysmng_dispclock(void);
#else
#define	sysmng_clockreset()
#define	sysmng_dispclock()
#endif

