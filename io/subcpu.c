#include	"compiler.h"
#include	"timemng.h"
#include	"z80core.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"nevent.h"
#include	"ievent.h"
#include	"keystat.h"
#include	"calendar.h"


//							    e3 e4 e5 e6 e7 e8 e9 ea eb ec ed ee ef
static const UINT8 cmdtbl[] = { 0, 1, 0, 0, 1, 0, 1, 0, 0, 3, 0, 3, 0};
static const UINT8 dattbl[] = { 3, 0, 0, 2, 0, 1, 0, 1, 1, 0, 3, 0, 3};


// ----

void neitem_scpu(UINT id) {

	nevent_repeat(id);
	if (keystat.req_int) {
		keystat.req_int = 0;
		ievent_set(IEVENT_SUBCPU);
	}
}

BRESULT ieitem_scpu(UINT id) {

	UINT	key;

	if ((subcpu.cmdcnt) || (subcpu.datacnt)) {
		keystat.req_int = 1;			// 再送しる
		return(FALSE);
	}
	if (!subcpu.Ex[4][0]) {				// 割り込み不要だったら捨てる
		return(FALSE);
	}
	if (keystat.shift & 0x40) {					// キーが押された場合
		key = keystat_getflag();
		subcpu.Ex[0x06][0] = (UINT8)(key >> 0);
		subcpu.Ex[0x06][1] = (UINT8)(key >> 8);
		if (!subcpu.Ex[0x06][1]) {		// 無効なキーだったら捨てる
			return(FALSE);
		}
		subcpu.INT_SW = 1;
	}
	else {
		if (!subcpu.INT_SW) {			// 何も押されてなかったら割り込まない
			return(FALSE);
		}
		subcpu.INT_SW = 0;
		key = keystat_getflag();
		subcpu.Ex[0x06][0] = (UINT8)(key >> 0);
		subcpu.Ex[0x06][1] = (UINT8)(key >> 8);
	}
	subcpu.mode = 0xe6;
	subcpu.cmdcnt = 0;
	subcpu.datacnt = 2;
	subcpu.dataptr = offsetof(SUBCPU, Ex[0xe6 - 0xe0][0]);
	subcpu.OBF = 0;
	subcpu.IBF = 1;

	Z80_INTERRUPT(subcpu.Ex[4][0]);
	(void)id;
	return(TRUE);
}


// ----

void IOOUTCALL subcpu_o(UINT port, REG8 value) {

	UINT32	key;

	if (subcpu.IBF) {
		return;
	}
	if (!subcpu.cmdcnt) {
		subcpu.mode = (UINT8)value;
		subcpu.cmdcnt = 0;
		subcpu.datacnt = 0;
		if ((value >= 0xd0) && (value <= 0xd7)) {
			subcpu.cmdcnt = 6;
			subcpu.datacnt = 0;
		}
		else if ((value >= 0xd8) && (value <= 0xdf)) {
			subcpu.cmdcnt = 0;
			subcpu.datacnt = 6;
		}
		else if ((value >= 0xe3) && (value <= 0xef)) {
			subcpu.cmdcnt = cmdtbl[value - 0xe3];
			subcpu.datacnt = dattbl[value - 0xe3];
		}
		if (value < 0xe0) {
			subcpu.dataptr = offsetof(SUBCPU, Dx[value - 0xd0][0]);
		}
		else {
			subcpu.dataptr = offsetof(SUBCPU, Ex[value - 0xe0][0]);
		}
		switch(subcpu.mode) {
			case 0xe3:
				key = keystat_gete3();
				subcpu.Ex[0x03][0] = (UINT8)(key >> 0);
				subcpu.Ex[0x03][1] = (UINT8)(key >> 8);
				subcpu.Ex[0x03][2] = (UINT8)(key >> 16);
				break;

			case 0xe6:
				key = keystat_getflag();
				subcpu.Ex[0x06][0] = (UINT8)(key >> 0);
				subcpu.Ex[0x06][1] = (UINT8)(key >> 8);
				break;

			case 0xed:
				calendar_getdate(subcpu.Ex[0x0d]);
				break;

			case 0xef:
				calendar_gettime(subcpu.Ex[0x0f]);
				break;
		}
	}
	else {
		((UINT8 *)(&subcpu))[subcpu.dataptr] = value;
		subcpu.dataptr++;
		if (--subcpu.cmdcnt == 0) {
			switch(subcpu.mode) {
				case 0xe9:
					cmt_ctrl(subcpu.Ex[0x9][0]);
					break;

				case 0xec:
					calendar_setdate(subcpu.Ex[0x0c]);
					break;

				case 0xee:
					calendar_settime(subcpu.Ex[0x0e]);
					break;
			}
		}
	}
	subcpu.OBF = (UINT8)(subcpu.datacnt?0:1);
	subcpu.IBF = (UINT8)(subcpu.datacnt?1:0);
	(void)port;
}

REG8 IOINPCALL subcpu_i(UINT port) {

	UINT32	key;
	REG8	ret;

#if 1										// D-SIDE で通るように…
	if (!subcpu.datacnt) {					// ポインタは変らない？
		subcpu.datacnt++;

		switch(subcpu.mode) {					// 990225 puni
			case 0xe3:
				key = keystat_gete3();
				subcpu.Ex[0x03][0] = (UINT8)(key >> 0);
				subcpu.Ex[0x03][1] = (UINT8)(key >> 8);
				subcpu.Ex[0x03][2] = (UINT8)(key >> 16);
				break;

			case 0xe6:
				key = keystat_getflag();
				subcpu.Ex[0x06][0] = (UINT8)(key >> 0);
				subcpu.Ex[0x06][1] = (UINT8)(key >> 8);
				break;

			case 0xed:
				calendar_getdate(subcpu.Ex[0x0d]);
				break;

			case 0xef:
				calendar_gettime(subcpu.Ex[0x0f]);
				break;
		}
	}

#else
	if (subcpu.OBF) {
		return(0);
	}
#endif
	ret = 0;
	switch(subcpu.mode) {
		case 0xd8:
		case 0xd9:
		case 0xda:
		case 0xdb:
		case 0xdc:
		case 0xdd:
		case 0xde:
		case 0xdf:
			ret = subcpu.Dx[subcpu.mode - 0xd8][6 - subcpu.datacnt];
			break;

		case 0xe3:
			ret = subcpu.Ex[0x03][3 - subcpu.datacnt];
			break;

		case 0xe6:
			ret = subcpu.Ex[0x06][2 - subcpu.datacnt];
			break;

		case 0xe8:
			ret = subcpu.Ex[0x07][0];
			break;

		case 0xea:
			ret = cmt_ctrl_stat();
			break;

		case 0xeb:
			ret = cmt_tape_stat();
			break;

		case 0xed:
			ret = subcpu.Ex[0x0d][3 - subcpu.datacnt];
			break;

		case 0xef:
			ret = subcpu.Ex[0x0f][3 - subcpu.datacnt];
			break;
	}
	subcpu.datacnt--;
	subcpu.OBF = (UINT8)(subcpu.datacnt?0:1);
	subcpu.IBF = (UINT8)(subcpu.datacnt?1:0);
	(void)port;
	return(ret);
}


// ----

void subcpu_reset(void) {

	ZeroMemory(&subcpu, sizeof(subcpu));
	subcpu.OBF = 1;
	nevent_set(NEVENT_SUBCPU, pccore.realclock / 480,
												neitem_scpu, NEVENT_ABSOLUTE);
}

