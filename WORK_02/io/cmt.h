
typedef struct {
	UINT8	cmd;
	/* UINT8	stop; */
} CMT;


#ifdef __cplusplus
extern "C" {
#endif

REG8 cmt_read(void);
void cmt_write(REG8 dat);
void cmt_ctrl(REG8 cmd);
REG8 cmt_tape_stat(void);
REG8 cmt_ctrl_stat(void);
REG8 cmt_test(void);

void cmt_reset(void);

#ifdef __cplusplus
}
#endif

