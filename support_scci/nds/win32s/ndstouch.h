
struct tagTouchPosition
{
	SINT16 x;
	SINT16 y;
	SINT16 px;
	SINT16 py;
	SINT16 z1;
	SINT16 z2;
};
typedef struct tagTouchPosition touchPosition;

extern UINT16 g_wRegKeyXY;
#define REG_KEYXY		g_wRegKeyXY;

touchPosition touchReadXY();

