
#ifdef __cplusplus
extern "C" {
#endif

extern	BRESULT	task_avail;

void taskmng_initialize(void);
void taskmng_exit(void);
void taskmng_rol(void);
#define	taskmng_isavail()		(task_avail)
BRESULT taskmng_sleep(UINT32 tick);


// ---- for wince

#if defined(WIN32_PLATFORM_PSPC)
void taskmng_minimize(void);
#endif

#ifdef __cplusplus
}
#endif

