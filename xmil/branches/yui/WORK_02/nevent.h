/**
 * @file	nevent.h
 * @brief	Interface of the event
 */

#pragma once

enum
{
	NEVENT_MAXCLOCK		= 0x400000
};

/**
 * NEvent ID
 */
enum tagNEventId
{
	NEVENT_FRAMES		= 0,
	NEVENT_RTC			= 1,
	NEVENT_FDC			= 2,
	NEVENT_CTC0			= 3,
	NEVENT_CTC1			= 4,
	NEVENT_CTC2			= 5,
	NEVENT_SUBCPU		= 6,
	/* ---- */
	NEVENT_MAXEVENTS	= 8
};
typedef enum tagNEventId NEVENTID;

/**
 * event position
 */
enum tagNEventPosition
{
	NEVENT_RELATIVE		= 0,		/*!< relative */
	NEVENT_ABSOLUTE		= 1			/*!< absolute */
};
typedef enum tagNEventPosition NEVENTPOSITION;		/*!< the defines of position */

struct _neventitem;
typedef	struct _neventitem	_NEVENTITEM;
typedef	struct _neventitem	*NEVENTITEM;
typedef void (*NEVENTCB)(NEVENTID id);

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

/* ������ */
void nevent_allreset(void);

/* �ŒZ�C�x���g�̃Z�b�g */
void nevent_get1stevent(void);

/* ���Ԃ�i�߂� */
void nevent_progress(void);

/* �C�x���g�̎��s */
void nevent_execule(void);

/* �C�x���g�̒ǉ� */
void nevent_set(NEVENTID id, SINT32 eventclock, NEVENTCB proc, NEVENTPOSITION absolute);
void nevent_repeat(NEVENTID id);
void nevent_setbyms(NEVENTID id, SINT32 ms, NEVENTCB proc, NEVENTPOSITION absolute);

/* �C�x���g�̍폜 */
void nevent_reset(NEVENTID id);

/* �C�x���g�̓����Ԏ擾 */
BOOL nevent_iswork(NEVENTID id);

/* ���s�����N���b�N���̎擾 */
SINT32 nevent_getwork(NEVENTID id);

/* �C�x���g���s�܂ł̃N���b�N���̎擾 */
SINT32 nevent_getremain(NEVENTID id);

/* NEVENT�̋����E�o */
void nevent_forceexit(void);

#ifdef __cplusplus
}
#endif

