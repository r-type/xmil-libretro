
enum {
	SIOCH_A		= 0,
	SIOCH_B		= 1
};

#define	SIOFIFO_BUFMAX		80

typedef struct {
	UINT	cnt;
	UINT	top;
	UINT	fno;
	UINT8	buf[SIOFIFO_BUFMAX];
	UINT8	flg[SIOFIFO_BUFMAX];
} SIOFIFO;

typedef struct {
	UINT8	num;
	UINT8	tx_int;
	UINT8	rx_int;
	UINT8	padding;
	UINT8	wr[8];
	UINT8	rr[4];
	SIOFIFO	TxD;			/* ëóêM */
	SIOFIFO	RxD;			/* éÛêM */
} SIOCH;

typedef struct {
	SIOCH	ch[2];
} SIO;


#ifdef __cplusplus
extern "C" {
#endif

void sio_int(void);
void IOOUTCALL sio_o(UINT port, REG8 dat);
REG8 IOINPCALL sio_i(UINT port);

void sio_reset(void);

#ifdef __cplusplus
}
#endif

