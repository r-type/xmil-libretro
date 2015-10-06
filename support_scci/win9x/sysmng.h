
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

#if 0
void sysmng_initialize(void);
void sysmng_update(UINT bitmap);
void sysmng_cpureset(void);
void sysmng_scrnwidth(REG8 mode);
void sysmng_fddaccess(REG8 drv);
#else

// マクロ(単に関数コールしたくないだけ)
#define	sysmng_initialize()												\
			sys_updates = 0

#define	sysmng_update(a)												\
			sys_updates |= (a);											\
			if ((a) & SYS_UPDATEFDD) sysmng_updatecaption(1)

#define	sysmng_cpureset()												\
			sys_updates &= (SYS_UPDATECFG | SYS_UPDATEOSCFG);			\
			sysmng_workclockreset()

void sysmng_scrnwidth(REG8 mode);

#define	sysmng_fddaccess(a)

#endif


// ---- あとはOS依存部

void sysmng_workclockreset(void);
BRESULT sysmng_workclockrenewal(void);
void sysmng_updatecaption(REG8 flag);

#ifdef __cplusplus
}
#endif

