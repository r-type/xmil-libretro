#include	"compiler.h"

#if defined(SUPPORT_TURBOZ) || defined(SUPPORT_OPM)

#include	"sound.h"
#include	"sndctrl.h"

extern	OPMCFG	opmcfg;

#define	CALCENV(e, c, s)													\
	(c)->slot[(s)].freq_cnt += (c)->slot[(s)].freq_inc;						\
	(c)->slot[(s)].env_cnt += (c)->slot[(s)].env_inc;						\
	if ((c)->slot[(s)].env_cnt >= (c)->slot[(s)].env_end) {					\
		switch((c)->slot[(s)].env_mode) {									\
			case EM_ATTACK:													\
				(c)->slot[(s)].env_mode = EM_DECAY1;						\
				(c)->slot[(s)].env_cnt = EC_DECAY;							\
				(c)->slot[(s)].env_end = (c)->slot[(s)].decaylevel;			\
				(c)->slot[(s)].env_inc = (c)->slot[(s)].env_inc_decay1;		\
				break;														\
			case EM_DECAY1:													\
				(c)->slot[(s)].env_mode = EM_DECAY2;						\
				(c)->slot[(s)].env_cnt = (c)->slot[(s)].decaylevel;			\
				(c)->slot[(s)].env_end = EC_OFF;							\
				(c)->slot[(s)].env_inc = (c)->slot[(s)].env_inc_decay2;		\
				break;														\
			case EM_RELEASE:												\
				(c)->slot[(s)].env_mode = EM_OFF;							\
			case EM_DECAY2:													\
				(c)->slot[(s)].env_cnt = EC_OFF;							\
				(c)->slot[(s)].env_end = EC_OFF + 1;						\
				(c)->slot[(s)].env_inc = 0;									\
				(c)->playing &= ~(1 << (s));								\
				break;														\
		}																	\
	}																		\
	(e) = (c)->slot[(s)].totallevel -										\
					opmcfg.envcurve[(c)->slot[(s)].env_cnt >> ENV_BITS];

#define SLOTOUT(s, e, c)													\
		((opmcfg.sintable[(((s).freq_cnt + (c)) >>							\
							(FREQ_BITS - SIN_BITS)) & (SIN_ENT - 1)] *		\
				opmcfg.envtable[(e)]) >> (ENVTBL_BIT+SINTBL_BIT-TL_BITS))


static void calcratechannel(OPMCH *ch) {

	SINT32	envout;
	SINT32	opout;

	opmgen.feedback2 = 0;
	opmgen.feedback3 = 0;
	opmgen.feedback4 = 0;

	/* SLOT 1 */
	CALCENV(envout, ch, 0);
	if (envout > 0) {
		if (ch->feedback) {
			/* with self feed back */
			opout = ch->op1fb;
			ch->op1fb = SLOTOUT(ch->slot[0], envout,
											(ch->op1fb >> ch->feedback));
			opout = (opout + ch->op1fb) / 2;
		}
		else {
			/* without self feed back */
			opout = SLOTOUT(ch->slot[0], envout, 0);
		}
		/* output slot1 */
		if (!ch->connect1) {
			opmgen.feedback2 = opmgen.feedback3 = opmgen.feedback4 = opout;
		}
		else {
			*ch->connect1 += opout;
		}
	}
	/* SLOT 2 */
	CALCENV(envout, ch, 1);
	if (envout > 0) {
		*ch->connect2 += SLOTOUT(ch->slot[1], envout, opmgen.feedback2);
	}
	/* SLOT 3 */
	CALCENV(envout, ch, 2);
	if (envout > 0) {
		*ch->connect3 += SLOTOUT(ch->slot[2], envout, opmgen.feedback3);
	}
	/* SLOT 4 */
	CALCENV(envout, ch, 3);
	if (envout > 0) {
		*ch->connect4 += SLOTOUT(ch->slot[3], envout, opmgen.feedback4);
	}
}

void SOUNDCALL opmgen_getpcm(void *hdl, SINT32 *pcm, UINT count) {

	OPMCH	*ch;
	UINT	i;
	UINT	playing;
	SINT32	samp_l;
	SINT32	samp_r;

	if ((!opmgen.playing) || (!count)) {
		return;
	}
	ch = opmch;
	do {
		samp_l = opmgen.outdl * (opmgen.calcremain * -1);
		samp_r = opmgen.outdr * (opmgen.calcremain * -1);
		opmgen.calcremain += FMDIV_ENT;
		while(1) {
			opmgen.outdc = 0;
			opmgen.outdl = 0;
			opmgen.outdr = 0;
			playing = 0;
			for (i=0; i<OPMCH_MAX; i++) {
				if (ch[i].playing & ch[i].outslot) {
					calcratechannel(ch + i);
					playing++;
				}
			}
			opmgen.outdl += opmgen.outdc;
			opmgen.outdr += opmgen.outdc;
			opmgen.outdl >>= FMVOL_SFTBIT;
			opmgen.outdr >>= FMVOL_SFTBIT;
			if (opmgen.calcremain > opmcfg.calc1024) {
				samp_l += opmgen.outdl * opmcfg.calc1024;
				samp_r += opmgen.outdr * opmcfg.calc1024;
				opmgen.calcremain -= opmcfg.calc1024;
			}
			else {
				break;
			}
		}
		samp_l += opmgen.outdl * opmgen.calcremain;
		samp_l >>= 8;
		samp_l *= opmcfg.fmvol;
		samp_l >>= (OPM_OUTSB + FMDIV_BITS + 1 + 6 - FMVOL_SFTBIT - 8);
		pcm[0] += samp_l;
		samp_r += opmgen.outdr * opmgen.calcremain;
		samp_r >>= 8;
		samp_r *= opmcfg.fmvol;
		samp_r >>= (OPM_OUTSB + FMDIV_BITS + 1 + 6 - FMVOL_SFTBIT - 8);
		pcm[1] += samp_r;
		opmgen.calcremain -= opmcfg.calc1024;
		pcm += 2;
	} while(--count);
	opmgen.playing = playing;
	(void)hdl;
}

#endif
