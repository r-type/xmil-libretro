
#ifdef __cplusplus
extern "C" {
#endif

void timing_reset(void);
void timing_setrate(UINT32 clock);
void timing_setcount(UINT value);
UINT timing_getcount(void);

#ifdef __cplusplus
}
#endif

