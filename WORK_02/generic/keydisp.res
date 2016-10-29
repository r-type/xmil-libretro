
static const UINT8 keybrd1[] = {				// ���ՁB
				28, 14,
				0xc4, 0x6c, 0x44, 0x60,
				0xc4, 0x6c, 0x44, 0x60,
				0xc4, 0x6c, 0x44, 0x60,
				0xc4, 0x6c, 0x44, 0x60,
				0xc4, 0x6c, 0x44, 0x60,
				0xc4, 0x6c, 0x44, 0x60,
				0xc4, 0x6c, 0x44, 0x60,
				0xc4, 0x6c, 0x44, 0x60,
				0xee, 0xee, 0xee, 0xe0,
				0xee, 0xee, 0xee, 0xe0,
				0xee, 0xee, 0xee, 0xe0,
				0xee, 0xee, 0xee, 0xe0,
				0xee, 0xee, 0xee, 0xe0,
				0x00, 0x00, 0x00, 0x00};

static const UINT8 keybrd2[] = {				// ���� G�Ő؂��B
				20, 14,
				0xc4, 0x6c, 0x40,
				0xc4, 0x6c, 0x40,
				0xc4, 0x6c, 0x40,
				0xc4, 0x6c, 0x40,
				0xc4, 0x6c, 0x40,
				0xc4, 0x6c, 0x40,
				0xc4, 0x6c, 0x40,
				0xc4, 0x6c, 0x40,
				0xee, 0xee, 0xe0,
				0xee, 0xee, 0xe0,
				0xee, 0xee, 0xe0,
				0xee, 0xee, 0xe0,
				0xee, 0xee, 0xe0,
				0x00, 0x00, 0x00};

/*! Key pattern: C, F */
static const UINT8 keybrd_s1[] =
{
	3, 13,
	0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0,
	0xe0, 0xe0, 0xe0, 0xe0, 0xe0
};

/*! Key pattern: D, G, A */
static const UINT8 keybrd_s2[] =
{
	3, 13,
	0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,
	0xe0, 0xe0, 0xe0, 0xe0, 0xe0
};

/*! Key pattern: E, B */
static const UINT8 keybrd_s3[] =
{
	3, 13,
	0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60,
	0xe0, 0xe0, 0xe0, 0xe0, 0xe0
};

/*! Key pattern: C+, D+, F+, G+, A+ */
static const UINT8 keybrd_s4[] =
{
	3, 8,
	0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0
};

/*! note pattern */
static const NOTEPATTERN s_notepattern[12] =
{
	{ 0, 0, keybrd_s1}, { 2, 1, keybrd_s4},
	{ 4, 0, keybrd_s2}, { 6, 1, keybrd_s4},
	{ 8, 0, keybrd_s3},
	{12, 0, keybrd_s1}, {14, 1, keybrd_s4},
	{16, 0, keybrd_s2}, {18, 1, keybrd_s4},
	{20, 0, keybrd_s2}, {22, 1, keybrd_s4},
	{24, 0, keybrd_s3}
};
