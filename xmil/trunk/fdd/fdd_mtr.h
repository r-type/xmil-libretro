
#if 0

#define	FDDMTR_INIT			;
#define	FDDMTR_BUSY			0
#define	FDDMTR_MOVE			;
#define	FDDMTR_WAITSEC(v)	;
#define	FDDMTR_CALLBACK(t)	;
#define	FDDMTR_DRVSET		;

#else

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

void fddmtr_init(void);
void fddmtr_motormove(void);
void fddmtr_callback(UINT32 time);
void fddmtr_waitsec(UINT8 value);
void fddmtr_drvset(void);

#ifdef __cplusplus
}
#endif

#define	FDDMTR_INIT			fddmtr_init()
#define	FDDMTR_BUSY 		fddmtr.curevent
#define	FDDMTR_MOVE 		fddmtr_motormove()
#define	FDDMTR_WAITSEC(t)	fddmtr_waitsec(t)
#define	FDDMTR_CALLBACK(t)	fddmtr_callback(t)
#define	FDDMTR_DRVSET		fddmtr_drvset()
#endif

