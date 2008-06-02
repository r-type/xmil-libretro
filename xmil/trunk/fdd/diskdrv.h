
#ifdef __cplusplus
extern "C" {
#endif

void diskdrv_setfddex(REG8 drv, const OEMCHAR *fname,
												UINT32 ftype, int readonly);
/* void diskdrv_callback(void); */

#ifdef __cplusplus
}
#endif

#define diskdrv_setfdd(d, f, r)		diskdrv_setfddex(d, f, FTYPE_NONE, r)

