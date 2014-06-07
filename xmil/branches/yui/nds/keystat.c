#include	"compiler.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"keystat.h"
#include	"keystat.tbl"


/* キーコードがないので PC-98シリーズのキーコードをベースとする */
/* … つまり猫のがそのまま使える */


	KEYSTAT		keystat;


static void keydown(REG8 key) {

	REG8	flag;

	flag = keyflag[key];
	if (flag & 0x40) {
		keystat.shift |= flag & 0x1f;
	}
	else if (flag & 0x20) {
		keystat.keybmp |= (1 << (flag & 0x1f));
	}
	if (key < 0x70) {
		keystat.req_int = TRUE;
		keystat.shift |= 0x40;
		keystat.lastkey = key;
		subcpu_sendkey();
	}
}

static void keyup(REG8 key) {

	REG8	flag;

	flag = keyflag[key];
	if (flag & 0x40) {
		keystat.shift &= ~(flag & 0x1f);
	}
	else if (flag & 0x20) {
		keystat.keybmp &= ~(1 << (flag & 0x1f));
	}
	if (key < 0x70) {
		keystat.req_int = TRUE;
		keystat.shift &= ~0x40;
		subcpu_sendkey();
	}
}

UINT keystat_getflag(void) {

	REG8	ks;
	REG8	kf;
	UINT	tbl;
	REG8	kc;

	ks = keystat.shift;
	kc = 0;
	if (ks & 0x40) {
		kf = keyflag[keystat.lastkey];
		ks |= (kf & 0x80);								/* function */
		if (ks & 0x04) {								/* kana */
			if (!(ks & 0x02)) {							/* shift */
				tbl = 4;
			}
			else {
				tbl = 5;
			}
		}
		else if (ks & 0x01) {							/* ctrl */
			tbl = 3;
		}
		else if (ks & 0x10) {							/* grph */
			tbl = 2;
		}
		else if (ks & 0x02) {							/* shift */
			tbl = 1;
		}
		else {
			tbl = 0;
		}
		kc = keytbl[tbl][keystat.lastkey];
		if (ks & 0x08) {								/* caps */
			if (((kc >= 'A') && (kc <= 'Z')) ||
				((kc >= 'a') && (kc <= 'z'))) {
				kc ^= 0x20;
			}
		}
	}
	return((kc << 8) + (ks ^ 0xff));
}

UINT32 keystat_gete3(void) {

	return(keystat.keybmp);
}

REG8 keystat_getjoy(void) {

	return(~keystat.joykey);
}


/* extension */

void keystat_keydown(REG8 ref) {

	REG8	shift;
	REG8	key;

	if (ref < 0x80) {
		if (xmilcfg.KEY_MODE) {
			shift = joyflag[ref];
			if (shift & 0x40) {
				keystat.joykey |= (1 << (shift & 7));
				return;
			}
		}
		if (!keystat.hit[ref]) {
			keystat.hit[ref] = 1;
			keydown(ref);
		}
	}
	else if (ref < 0x90) {
		key = ref - 0x10;
		keystat.hit[key] ^= 1;
		if (keystat.hit[key]) {
			keydown(key);
		}
		else {
			keyup(key);
		}
	}
}

void keystat_keyup(REG8 ref) {

	REG8	shift;

	if (ref < 0x80) {
		if (xmilcfg.KEY_MODE) {
			shift = joyflag[ref];
			if (shift & 0x40) {
				keystat.joykey &= ~(1 << (shift & 7));
				return;
			}
		}
		if (keystat.hit[ref]) {
			keystat.hit[ref] = 0;
			keyup(ref);
		}
	}
}

void keystat_resetjoykey(void) {

	REG8	i;

	keystat.joykey = 0;
	for (i=0; i<0x80; i++) {
		if ((joyflag[i] & 0x40) && (keystat.hit[i])) {
			keystat.hit[i] = 0;
			keyup(i);
		}
	}
}

void keystat_allrelease(void) {

	REG8	i;

	for (i=0; i<0x80; i++) {
		if (keystat.hit[i]) {
			keystat.hit[i] = 0;
			keyup(i);
		}
	}
}

static REG8 cnvnewcode(REG8 oldcode) {

	switch(oldcode) {
		case 0x71:				/* 通常caps */
			return(0x81);

		case 0x72:				/* 通常カナ */
			return(0x82);

		case 0x79:				/* メカニカルロックcaps */
			return(0x71);

		case 0x7a:				/* メカニカルロックcaps */
			return(0x72);

		case 0x76:
			return(0x90);		/* NKEY_USER + 0 */

		case 0x77:
			return(0x91);		/* NKEY_USER + 1 */

		default:
			return(oldcode);
	}
}

void keystat_senddata(REG8 data) {

	REG8	keycode;

	keycode = cnvnewcode((REG8)(data & 0x7f));
	if (!(data & 0x80)) {
		keystat_keydown(keycode);
	}
	else {
		keystat_keyup(keycode);
	}
}


/*		0x80 --> 0x01
 *		Ｑ  Ｗ  Ｅ  Ａ  Ｄ  Ｚ  Ｘ  Ｃ
 *		[7] [4] [1] [8] [2] [9] [6] [3]
 *		ESC １  [-] [+] [*] TAB ' ' Ent */
