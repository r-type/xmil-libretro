#include	"compiler.h"

#if defined(SUPPORT_TURBOZ) || defined(SUPPORT_OPM)

#ifndef		PALMOS
#include	<math.h>
#endif
#include	"sound.h"
#include	"sndctrl.h"


#define	OPM_ARRATE		 399128L
#define	OPM_DRRATE		5514396L

#define	EG_STEP	(96.0 / EVC_ENT)					/* dB step */
#define	SC(db)	(SINT32)((db) * ((3.0 / EG_STEP) * (1 << ENV_BITS))) + EC_DECAY
#define	KF(sn)	((sn) << KF_BITS)
#define	D2(value)		(SINT32)(((double)(6 << KF_BITS)*log((double)value) / \
									log(2.0))+0.5)
#define	FREQBASE4096	((double)OPM_CLOCK / rate2 / 64)
#define	FREQ_RATE		(1 << (21 - FREQ_BITS))


		OPMCFG	opmcfg;
static	SINT32	detunetable[8][32];
static	SINT32	attacktable[94];
static	SINT32	decaytable[94];
static	SINT32	keycodetable[8*12*64+610];
static	SINT32	detune2table[4];


static const UINT halftunetable[16] = {
					KF( 0),KF( 1),KF( 2),KF( 3),KF( 3),KF( 4),KF( 5),KF( 6),
					KF( 6),KF( 7),KF( 8),KF( 9),KF( 9),KF(10),KF(11),KF(12)};
static const SINT32	decayleveltable[16] = {
		 			SC( 0),SC( 1),SC( 2),SC( 3),SC( 4),SC( 5),SC( 6),SC( 7),
		 			SC( 8),SC( 9),SC(10),SC(11),SC(12),SC(13),SC(14),SC(31)};
static const UINT8 multipletable[16] = {
			    	1, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30};
static const SINT32 nulltable[] = {
					0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
					0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
static const UINT8 dttable[] = {
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2,
					2, 3, 3, 3, 4, 4, 4, 5, 5, 6, 6, 7, 8, 8, 8, 8,
					1, 1, 1, 1, 2, 2, 2, 2, 2, 3, 3, 3, 4, 4, 4, 5,
					5, 6, 6, 7, 8, 8, 9,10,11,12,13,14,16,16,16,16,
					2, 2, 2, 2, 2, 3, 3, 3, 4, 4, 4, 5, 5, 6, 6, 7,
					8, 8, 9,10,11,12,13,14,16,17,19,20,22,22,22,22};
static const UINT8 fmslot[4] = {0, 2, 1, 3};


void opmgen_initialize(UINT rate) {

	int		i;
	int		j;
	double	pom;
	double	freq;
	UINT	rate1;
	UINT	rate2;

	rate1 = rate;	/* 44100 */
	rate2 = 62500;
	while((rate1 * 2) < rate2) {
		rate2 >>= 1;
	}
	opmcfg.calc1024 = (FMDIV_ENT * rate1) / rate2;

	for (i=0; i<EVC_ENT; i++) {
		pom = (double)(1 << ENVTBL_BIT) / pow(10, EG_STEP*(EVC_ENT-i)/20);
		opmcfg.envtable[i] = (SINT32)pom;
	}
	for (i=0; i<SIN_ENT; i++) {
		pom = (double)(3 << (SINTBL_BIT - 2)) * sin(2*PI*i/SIN_ENT);
		opmcfg.sintable[i] = (SINT32)pom;
	}
	for (i=0; i<EVC_ENT; i++) {
		pom = pow(((double)(EVC_ENT-1-i)/EVC_ENT), 8) * EVC_ENT;
		opmcfg.envcurve[i] = (SINT32)pom;
		opmcfg.envcurve[EVC_ENT + i] = i;
	}
	opmcfg.envcurve[EVC_ENT * 2] = EVC_ENT;

	freq = (double)(1L << FREQ_BITS) / (3579545.0 / OPM_CLOCK * rate2);
	for (i=0; i<NELEMENTS(keycodetable); i++) {
		pom = 6.875 * pow(2, ((i+4*64)*1.5625/1200.0));
		keycodetable[i] = (SINT32)(pom * freq);
	}
	for (i=0; i<4; i++) {
		for (j=0; j<32; j++) {
			freq = FREQBASE4096 * dttable[i*32 + j] / FREQ_RATE;
			detunetable[i][j]   = (SINT32)freq;
			detunetable[i+4][j] = (SINT32)-freq;
		}
	}
	for (i=0; i<4; i++) {
		attacktable[i] = decaytable[i] = 0;
	}
	for (i=4; i<64; i++) {
		freq = (double)(EVC_ENT << ENV_BITS) * FREQBASE4096;
		if (i < 8) {							/* –Y‚ê‚Ä‚Ü‚·B */
			freq *= 1.0 + (i & 2) * 0.25;
		}
		else if (i < 60) {
			freq *= 1.0 + (i & 3) * 0.25;
		}
		freq *= 1L << ((i>>2) - 1);
		attacktable[i] = (SINT32)(freq / OPM_ARRATE);
		decaytable[i] = (SINT32)(freq / OPM_DRRATE);
	}
	attacktable[62] = EC_DECAY - 1;
	attacktable[63] = EC_DECAY - 1;
	for (i=64; i<94; i++) {
		attacktable[i] = attacktable[63];
		decaytable[i] = decaytable[63];
	}

	detune2table[0] = D2(1.0);
	detune2table[1] = D2(2.0);
	detune2table[2] = D2(2.5);
	detune2table[3] = D2(3.0);
}

void opmgen_setvol(UINT vol) {

	opmcfg.fmvol = vol;
}


/* ---- */

static void keyon(OPMCH *ch, REG8 value) {

	OPMSLOT	*slot;
	REG8	bit;
	UINT	i;

	opmgen.playing++;
	ch->playing |= (value >> 3) & 0x0f;
	slot = ch->slot;
	bit = 0x08;
	for (i=0; i<4; i++) {
		if (value & bit) {							/* keyon */
			if (slot->env_mode <= EM_RELEASE) {
				slot->freq_cnt = 0;
				if (i == OPMSLOT1) {
					ch->op1fb = 0;
				}
				slot->env_mode = EM_ATTACK;
				slot->env_inc = slot->env_inc_attack;
				slot->env_cnt = EC_ATTACK;
				slot->env_end = EC_DECAY;
			}
		}
		else {										/* keyoff */
			if (slot->env_mode > EM_RELEASE) {
				slot->env_mode = EM_RELEASE;
				if (!(slot->env_cnt & EC_DECAY)) {
					slot->env_cnt = (opmcfg.envcurve[slot->env_cnt
										>> ENV_BITS] << ENV_BITS) + EC_DECAY;
				}
				slot->env_end = EC_OFF;
				slot->env_inc = slot->env_inc_release;
			}
		}
		bit <<= 1;
		slot++;
	}
}

static void set_algorithm(OPMCH *ch, REG8 value) {

	UINT8	feed;
	SINT32	*outd;
	UINT8	outslot;

	ch->algorithm = (UINT8)(value & 7);
	feed = (value >> 3) & 7;
	if (feed) {
		ch->feedback = 8 - feed;
	}
	else {
		ch->feedback = 0;
	}

	switch((value >> 6) & 3) {
		case 0:
			outd = &opmgen.feedback4;
			break;

		case 1:
			outd = &opmgen.outdl;
			break;

		case 2:
			outd = &opmgen.outdr;
			break;

		case 3:
			outd = &opmgen.outdc;
			break;
	}

	switch(value & 7) {
		case 0:
			ch->connect1 = &opmgen.feedback2;
			ch->connect2 = &opmgen.feedback3;
			ch->connect3 = &opmgen.feedback4;
			outslot = 0x08;
			break;

		case 1:
			ch->connect1 = &opmgen.feedback3;
			ch->connect2 = &opmgen.feedback3;
			ch->connect3 = &opmgen.feedback4;
			outslot = 0x08;
			break;

		case 2:
			ch->connect1 = &opmgen.feedback4;
			ch->connect2 = &opmgen.feedback3;
			ch->connect3 = &opmgen.feedback4;
			outslot = 0x08;
			break;

		case 3:
			ch->connect1 = &opmgen.feedback2;
			ch->connect2 = &opmgen.feedback4;
			ch->connect3 = &opmgen.feedback4;
			outslot = 0x08;
			break;

		case 4:
			ch->connect1 = &opmgen.feedback2;
			ch->connect2 = outd;
			ch->connect3 = &opmgen.feedback4;
			outslot = 0x0a;
			break;

		case 5:
			ch->connect1 = 0;
			ch->connect2 = outd;
			ch->connect3 = outd;
			outslot = 0x0e;
			break;

		case 6:
			ch->connect1 = &opmgen.feedback2;
			ch->connect2 = outd;
			ch->connect3 = outd;
			outslot = 0x0e;
			break;

		case 7:
		default:
			ch->connect1 = outd;
			ch->connect2 = outd;
			ch->connect3 = outd;
			outslot = 0x0f;
			break;
	}
	ch->connect4 = outd;
	ch->outslot = outslot;
}

static void set_dt1_mul(OPMSLOT *slot, REG8 value) {

	slot->multiple = multipletable[value & 0x0f];
	slot->detune1 = detunetable[(value >> 4) & 7];
}

static void set_tl(OPMSLOT *slot, REG8 value) {

	slot->totallevel = ((~value) & 0x007f) << (EVC_BITS - 7);
}

static void set_ks_ar(OPMSLOT *slot, REG8 value) {

	slot->keyscale = (UINT8)(((~value) >> 6) & 3);
	value &= 0x1f;
	slot->attack = (value)?(attacktable + (value << 1)):nulltable;
	slot->env_inc_attack = slot->attack[slot->envratio];
	if (slot->env_mode == EM_ATTACK) {
		slot->env_inc = slot->env_inc_attack;
	}
}

static void set_d1r(OPMSLOT *slot, REG8 value) {

	value &= 0x1f;
	slot->decay1 = (value)?(decaytable + (value << 1)):nulltable;
	slot->env_inc_decay1 = slot->decay1[slot->envratio];
	if (slot->env_mode == EM_DECAY1) {
		slot->env_inc = slot->env_inc_decay1;
	}
}

static void set_dt2_d2r(OPMSLOT *slot, REG8 value) {

	slot->detune2 = detune2table[value >> 6];
	value &= 0x1f;
	slot->decay2 = (value)?(decaytable + (value << 1)):nulltable;
	slot->env_inc_decay2 = slot->decay2[slot->envratio];
	if (slot->env_mode == EM_DECAY2) {
		slot->env_inc = slot->env_inc_decay2;
	}
}

static void set_d1l_rr(OPMSLOT *slot, REG8 value) {

	slot->decaylevel = decayleveltable[(value >> 4)];
	slot->release = decaytable + ((value & 0x0f) << 2) + 2;
	slot->env_inc_release = slot->release[slot->envratio];
	if (slot->env_mode == EM_RELEASE) {
		slot->env_inc = slot->env_inc_release;
#if 0
		if (value == 0xff) {
			slot->env_mode = EM_OFF;
			slot->env_cnt = EC_OFF;
			slot->env_end = EC_OFF + 1;
			slot->env_inc = 0;
		}
#endif
	}
}

static void channelupdate(OPMCH *ch) {

	int			i;
	UINT32		fc;
	UINT8		kc;
	UINT		evr;
	OPMSLOT		*slot;

	fc = ch->keynote;
	kc = ch->kcode;
	slot = ch->slot;
	for (i=0; i<4; i++, slot++) {
		slot->freq_inc = (keycodetable[fc + slot->detune2] + slot->detune1[kc]) * slot->multiple;
		evr = kc >> slot->keyscale;
		if (slot->envratio != evr) {
			slot->envratio = evr;
			slot->env_inc_attack = slot->attack[evr];
			slot->env_inc_decay1 = slot->decay1[evr];
			slot->env_inc_decay2 = slot->decay2[evr];
			slot->env_inc_release = slot->release[evr];
		}
	}
}


/*----------------------------------------------------------------------------- */

void opmgen_reset(void) {

	OPMCH	*ch;
	UINT	i;
	OPMSLOT	*slot;
	UINT	j;

	opmgen.mode = 0;
	ch = opmch;
	for(i=0; i<OPMCH_MAX; i++) {
		ch->keynote = 0;
		slot = ch->slot;
		for(j=0; j<4; j++) {
			slot->env_mode = EM_OFF;
			slot->env_cnt = EC_OFF;
			slot->env_end = EC_OFF + 1;
			slot->env_inc = 0;
			slot->detune1 = detunetable[0];
			slot->attack = nulltable;
			slot->decay1 = nulltable;
			slot->decay2 = nulltable;
			slot->release = decaytable;
			slot++;
		}
		ch++;
	}
	for (i=0x20; i<0x100; i++) {
		opmgen_setreg((REG8)i, 0);
	}
}

void opmgen_setreg(REG8 reg, REG8 value) {

	UINT		c;
	UINT		i;
	OPMCH		*ch;
	OPMSLOT		*slot;

	sound_sync();
	c = reg & 7;
	ch = opmch + c;
	slot = ch->slot + fmslot[(reg >> 3) & 3];
	switch(reg & 0xe0) {
		case 0x00:
			switch(reg) {
				case 0x08:				/* key on/off */
					keyon(opmch + (value & 7), value);
					break;

				case 0x14:				/* mode */
					opmgen.mode = value;
					if ((value & 0x81) == 0x81) {
						opmgen.playing++;
						ch = opmch;
						for (c=0; c<8; c++) {
							ch->playing = 0x0f;
							ch->op1fb = 0;
							slot = ch->slot;
							for (i=0; i<4; i++) {
								slot->freq_cnt = 0;
								slot->env_mode = EM_ATTACK;
								slot->env_inc = slot->env_inc_attack;
								slot->env_cnt = EC_ATTACK;
								slot->env_end = EC_DECAY;
								slot++;
							}
							ch++;
						}
					}
					break;
			}
			break;

		case 0x20:
			switch(reg & 0x18) {
				case 0x00: 			/* pan feedback connection */
					set_algorithm(ch, value);
					break;

				case 0x08:			/* keycode */
					ch->kcode = (UINT8)((value >> 2) & 0x1f);
					ch->keynote = (((value >> 4) & 7) * (12 << KF_BITS)) +
									halftunetable[value & 0x0f] + ch->keyfunc;
					channelupdate(ch);
					break;

				case 0x10:			/* keyfunction */
					ch->keyfunc = (UINT8)(value >> (8 - KF_BITS));
					ch->keynote &= (((UINT32)-1) << KF_BITS);
					ch->keynote += ch->keyfunc;
					channelupdate(ch);
					break;
			}
			break;

		case 0x40:					/* DT1 MUL */
			set_dt1_mul(slot, value);
			channelupdate(ch);
			break;

		case 0x60:					/* TL */
			set_tl(slot, value);
			break;

		case 0x80:					/* KS AR */
			set_ks_ar(slot, value);
			channelupdate(ch);
			break;

		case 0xa0:					/* D1R */
			set_d1r(slot, value);
			break;

		case 0xc0:					/* DT2 D2R */
			set_dt2_d2r(slot, value);
			channelupdate(ch);
			break;

		case 0xe0:					/* D1L RR */
			set_d1l_rr(slot, value);
			break;
	}
}

#endif

