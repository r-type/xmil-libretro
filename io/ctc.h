
typedef struct {
	UINT8	cmd[4];
	UINT8	basecnt[4];
	SINT32	countmax[4];
	SINT32	count[4];
	UINT8	vector;
	UINT8	int_flag;
} CTCCH;

typedef struct {
	CTCCH	ch[3];
} CTC;


#ifdef __cplusplus
extern "C" {
#endif

void x1_ctc_int(void);
void IOOUTCALL ctc_o(UINT port, REG8 value);		// x1_ctc_w
REG8 IOINPCALL ctc_i(UINT port);					// x1_ctc_r

void ctc_reset(void);

#ifdef __cplusplus
}
#endif

