
#ifdef __cplusplus
extern "C" {
#endif

BRESULT fdd2d_set(FDDFILE fdd, REG8 drv, const OEMCHAR *fname);
BRESULT fdd2d_eject(FDDFILE fdd, REG8 drv);

short fdd_crc_2d(void);
BYTE fdd_stat_2d(void);

void fdd_read_2d(void);
void fdd_write_2d(void);

BYTE fdd_incoff_2d(void);

#ifdef __cplusplus
}
#endif

