
#ifdef __cplusplus
extern "C" {
#endif

void diskdrv_setfdd(REG8 drv, const OEMCHAR *fname, int readonly);
void diskdrv_setfddex(REG8 drv, const OEMCHAR *fname,
												UINT ftype, int readonly);
// void diskdrv_callback(void);

#ifdef __cplusplus
}
#endif

