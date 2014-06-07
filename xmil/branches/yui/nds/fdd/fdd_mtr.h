
#if !defined(DISABLE_SOUND)

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	UINT8	curevent;
	UINT8	curdrv;
	UINT8	padding[2];
	UINT32	nextevent;
	UINT8	head[4];
	UINT32	eventtime[4];
} _FDDMTR, *FDDMTR;

extern	_FDDMTR		fddmtr;

void fddmtr_initialize(void);
void fddmtr_motormove(void);
void fddmtr_callback(UINT32 time);
void fddmtr_waitsec(REG8 value);
void fddmtr_drvset(void);
#define	fddmtr_isbusy()		(fddmtr.curevent)

#ifdef __cplusplus
}
#endif

#else	/* !defined(DISABLE_SOUND) */

#define fddmtr_initialize()
#define fddmtr_motormove()
#define fddmtr_callback(t)
#define fddmtr_waitsec(v)
#define fddmtr_drvset()
#define	fddmtr_isbusy()		(FALSE)

#endif	/* !defined(DISABLE_SOUND) */

