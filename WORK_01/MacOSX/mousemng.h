
enum {
	uPD8255A_LEFTBIT	= 0x01,
	uPD8255A_RIGHTBIT	= 0x02
};


#ifdef __cplusplus
extern "C" {
#endif

REG8 mousemng_getstat(SINT16 *x, SINT16 *y, BRESULT clear);

#ifdef __cplusplus
}
#endif


// ---- for MacOS9

enum {
	MOUSEMNG_LEFTDOWN		= 0,
	MOUSEMNG_LEFTUP,
	MOUSEMNG_RIGHTDOWN,
	MOUSEMNG_RIGHTUP
};

enum {
	MOUSEPROC_SYSTEM		= 0,
	MOUSEPROC_MACUI
};

void mousemng_initialize(void);
void mousemng_callback(HIPoint delta);
BRESULT mousemng_buttonevent(UINT event);
void mousemng_enable(UINT proc);
void mousemng_disable(UINT proc);
void mousemng_toggle(UINT proc);

