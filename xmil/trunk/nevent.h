
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

struct _neventitem {
	SINT32		clock;
	SINT32		baseclock;
	NEVENTCB	proc;
	UINT32		param;
};

typedef struct {
	UINT		readyevents;
	UINT		level[NEVENT_MAXEVENTS];
	_NEVENTITEM	item[NEVENT_MAXEVENTS];
} _NEVENT, *NEVENT;


#ifdef __cplusplus
extern "C" {
#endif

extern	_NEVENT		nevent;

// ������
void nevent_allreset(void);

// �ŒZ�C�x���g�̃Z�b�g
void nevent_get1stevent(void);

// ���Ԃ�i�߂�
void nevent_progress(void);

// �C�x���g�̎��s
void nevent_execule(void);

// �C�x���g�̒ǉ�
void nevent_set(UINT id, SINT32 eventclock, NEVENTCB proc, BRESULT absolute);
void nevent_repeat(UINT id);
void nevent_setbyms(UINT id, SINT32 ms, NEVENTCB proc, BRESULT absolute);

// �C�x���g�̍폜
void nevent_reset(UINT id);

// �C�x���g�̓����Ԏ擾
BRESULT nevent_iswork(UINT id);

// ���s�����N���b�N���̎擾
SINT32 nevent_getwork(UINT id);

// �C�x���g���s�܂ł̃N���b�N���̎擾
SINT32 nevent_getremain(UINT id);

// NEVENT�̋����E�o
void nevent_forceexit(void);

#ifdef __cplusplus
}
#endif

