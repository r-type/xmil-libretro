
#ifdef __cplusplus
extern "C" {
#endif

BRESULT fddd88_eject(FDDFILE fdd, REG8 drv);
BRESULT fddd88_set(FDDFILE fdd, REG8 drv, const OEMCHAR *fname);

BRESULT fddd88_crc(FDDFILE fdd);

BYTE fdd_stat_d88(void);

void fdd_read_d88(void);
void fdd_write_d88(void);

BYTE fdd_incoff_d88(void);

void init_tao_d88(void);
void fdd_wtao_d88(BYTE data);

#ifdef __cplusplus
}
#endif

