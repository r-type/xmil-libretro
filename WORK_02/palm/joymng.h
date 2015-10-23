
enum {
	JOY_LEFT_BIT	= 0x04,
	JOY_RIGHT_BIT	= 0x08,
	JOY_UP_BIT		= 0x01,
	JOY_DOWN_BIT	= 0x02,
	JOY_BTN1_BIT	= 0x40,
	JOY_BTN2_BIT	= 0x20,
	JOY_BTN3_BIT	= 0x80,
	JOY_BTN4_BIT	= 0x10
};

#define JOY_PADALL (JOY_LEFT_BIT | JOY_RIGHT_BIT | JOY_UP_BIT | JOY_DOWN_BIT)
#define JOY_BTNALL (JOY_BTN1_BIT | JOY_BTN2_BIT | JOY_BTN3_BIT | JOY_BTN4_BIT)


#ifdef __cplusplus
extern "C" {
#endif

REG8 joymng_getstat(void);

#ifdef __cplusplus
}
#endif


// ----


void joymng_sync(REG8 key);
void joymng_initialize(void);
#define joymng_callback();
BRESULT joymng_5way(UINT code);
void joymng_hardkey(UINT32 state);

