
typedef struct {
	UINT8	Dx[16][8];
	UINT8	Ex[16][8];
	UINT	dataptr;
	UINT8	mode;
	UINT8	cmdcnt;
	UINT8	datacnt;
	UINT8	INT_SW;
	UINT8	IBF;
	UINT8	OBF;
} SUBCPU;


#ifdef __cplusplus
extern "C" {
#endif

void neitem_scpu(UINT id);
BRESULT ieitem_scpu(UINT id);

void IOOUTCALL subcpu_o(UINT port, REG8 dat);		// x1_sub_w
REG8 IOINPCALL subcpu_i(UINT port);					// x1_sub_r

void subcpu_reset(void);

#ifdef __cplusplus
}
#endif

