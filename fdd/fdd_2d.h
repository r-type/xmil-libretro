
#ifdef __cplusplus
extern "C" {
#endif

BRESULT fdd2d_eject(REG8 drv);
BRESULT fdd2d_set(REG8 drv, const OEMCHAR *fname);

short fdd_crc_2d(void);
BYTE fdd_stat_2d(void);

void fdd_read_2d(void);
void fdd_write_2d(void);

BYTE fdd_incoff_2d(void);

#ifdef __cplusplus
}
#endif

