
#define	OPM_CLOCK		4000000L			/* X1 & X68000 OPM 4MHz */
#define	OPMCH_MAX		8

enum {
	FMDIV_BITS		= 9,
	FMDIV_ENT		= (1 << FMDIV_BITS),
	FMVOL_SFTBIT	= 5
};

#if !defined(_WIN32_WCE)
#define	SIN_BITS		10
#define	EVC_BITS		10
#else
#define	SIN_BITS		8
#define	EVC_BITS		7
#endif
#define	ENV_BITS		16
#define	KF_BITS			6
#define	FREQ_BITS		20
#define	ENVTBL_BIT		14
#define	SINTBL_BIT		14

#define	TL_BITS			(FREQ_BITS + 2)
#define	OPM_OUTSB		(TL_BITS + 2 - 16)			/* OPM output 16bit */

#define	SIN_ENT			(1 << SIN_BITS)
#define	EVC_ENT			(1 << EVC_BITS)

#define	EC_ATTACK		0								/* ATTACK start */
#define	EC_DECAY		(EVC_ENT << ENV_BITS)			/* DECAY start  */
#define	EC_OFF			((2 * EVC_ENT) << ENV_BITS)		/* OFF          */

#define	TL_MAX			(EVC_ENT * 2)

enum {
	OPMSLOT1		= 0,
	OPMSLOT2		= 2,
	OPMSLOT3		= 1,
	OPMSLOT4		= 3,

	EM_ATTACK		= 4,
	EM_DECAY1		= 3,
	EM_DECAY2		= 2,
	EM_RELEASE		= 1,
	EM_OFF			= 0
};

typedef struct {
	SINT32		*detune1;			/* detune1               */
	SINT32		detune2;			/* detune2               */
	SINT32		totallevel;			/* total level           */
	SINT32		decaylevel;			/* decay level           */
const SINT32	*attack;			/* attack ratio          */
const SINT32	*decay1;			/* decay1 ratio          */
const SINT32	*decay2;			/* decay2 ratio          */
const SINT32	*release;			/* release ratio         */
	SINT32		freq_cnt;			/* frequency count       */
	SINT32		freq_inc;			/* frequency step        */
	UINT8		keyscale;			/* key scale             */
	UINT8		multiple;			/* multiple              */
	UINT8		env_mode;			/* envelope mode         */
	UINT8		envratio;			/* envelope raito        */

	SINT32		env_cnt;			/* envelope count        */
	SINT32		env_end;			/* envelope end count    */
	SINT32		env_inc;			/* envelope step         */
	SINT32		env_inc_attack;		/* envelope attack step  */
	SINT32		env_inc_decay1;		/* envelope decay1 step  */
	SINT32		env_inc_decay2;		/* envelope decay2 step  */
	SINT32		env_inc_release;	/* envelope release step */
} OPMSLOT;


typedef struct {
	OPMSLOT	slot[4];
	UINT8	algorithm;			/* algorythm          */
	UINT8	feedback;			/* self feedback      */
	UINT8	playing;
	UINT8	outslot;
	SINT32	op1fb;				/* operator1 feedback */
	SINT32	*connect1;			/* operator1 connect  */
	SINT32	*connect3;			/* operator3 connect  */
	SINT32	*connect2;			/* operator2 connect  */
	SINT32	*connect4;			/* operator4 connect  */
	UINT32	keynote;			/* key note           */

	UINT8	keyfunc;			/* key function       */
	UINT8	kcode;				/* key code           */
	UINT8	pan;				/* pan                */
	UINT8	padding;
} OPMCH;

typedef struct {
	UINT	playing;
	UINT8	mode;
	UINT8	padding[3];
	SINT32	feedback2;
	SINT32	feedback3;
	SINT32	feedback4;
	SINT32	outdl;
	SINT32	outdc;
	SINT32	outdr;
	SINT32	calcremain;
	UINT8	keyreg[OPMCH_MAX];
} _OPMGEN, *OPMGEN;

typedef struct {
	SINT32	calc1024;
	SINT32	fmvol;

	SINT32	sintable[SIN_ENT];
	SINT32	envtable[EVC_ENT];
	SINT32	envcurve[(EVC_ENT * 2) + 1];
} OPMCFG;


#ifdef __cplusplus
extern "C" {
#endif

void opmgen_initialize(UINT rate);
void opmgen_setvol(UINT vol);
void opmgen_reset(void);
void opmgen_setreg(REG8 reg, REG8 value);

void SOUNDCALL opmgen_getpcm(void *hdl, SINT32 *pcm, UINT count);

#ifdef __cplusplus
}
#endif

