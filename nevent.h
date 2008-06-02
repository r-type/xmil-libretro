
enum {
	NEVENT_MAXEVENTS	= 16,
	NEVENT_MAXCLOCK		= 0x400000,

	NEVENT_FRAMES		= 0,
	NEVENT_RTC			= 1,
	NEVENT_FDC			= 2,
	NEVENT_CTC0			= 3,
	NEVENT_CTC1			= 4,
	NEVENT_CTC2			= 5,
	NEVENT_SUBCPU		= 6,

	NEVENT_RELATIVE		= 0,
	NEVENT_ABSOLUTE		= 1
};

struct _neventitem;
typedef	struct _neventitem	_NEVENTITEM;
typedef	struct _neventitem	*NEVENTITEM;
typedef void (*NEVENTCB)(UINT id);

#define	NEVENTITEM_NONE		((NEVENTITEM)-1)
#define	NEVENTITEM_TERM		((NEVENTITEM)0)


struct _neventitem {
	NEVENTITEM	next;
	SINT32		clock;
	SINT32		baseclock;
	NEVENTCB	proc;
};

typedef struct {
	_NEVENTITEM	item[NEVENT_MAXEVENTS];
	NEVENTITEM	first;
} _NEVENT, *NEVENT;


#ifdef __cplusplus
extern "C" {
#endif

extern	_NEVENT		nevent;

/* 初期化 */
void nevent_allreset(void);

/* 最短イベントのセット */
void nevent_get1stevent(void);

/* 時間を進める */
void nevent_progress(void);

/* イベントの実行 */
void nevent_execule(void);

/* イベントの追加 */
void nevent_set(UINT id, SINT32 eventclock, NEVENTCB proc, BRESULT absolute);
void nevent_repeat(UINT id);
void nevent_setbyms(UINT id, SINT32 ms, NEVENTCB proc, BRESULT absolute);

/* イベントの削除 */
void nevent_reset(UINT id);

/* イベントの動作状態取得 */
BRESULT nevent_iswork(UINT id);

/* 実行したクロック数の取得 */
SINT32 nevent_getwork(UINT id);

/* イベント実行までのクロック数の取得 */
SINT32 nevent_getremain(UINT id);

/* NEVENTの強制脱出 */
void nevent_forceexit(void);

#ifdef __cplusplus
}
#endif

