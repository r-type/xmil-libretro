
#if defined(ADV_SIMULATE)

#define	xmilopen()

#else

#ifdef __cplusplus
extern "C" {
#endif

void xmilopen(void);

#ifdef __cplusplus
}
#endif

#endif

