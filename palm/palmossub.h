#include	"pealstub.h"

#define ByteSwap16(n) ( ((((unsigned int) n) << 8) & 0xFF00) | \
((((unsigned int) n) >> 8) & 0x00FF) )

#define ByteSwap32(n) ( ((((unsigned long) n) << 24) & 0xFF000000) | \
((((unsigned long) n) << 8) & 0x00FF0000) | \
((((unsigned long) n) >> 8) & 0x0000FF00) | \
((((unsigned long) n) >> 24) & 0x000000FF) )

typedef struct {
	UInt32 ptr;
} ArgOne;

#ifdef __cplusplus
extern "C" {
#endif

void ARM_TYPE_pointerinvoid (void* ptr, UINT trapID);
int ARM_TYPE_nullinint (UINT trapID);
void ARM_TYPE_intinvoid (UINT data, UINT trapID);
void ARM_TYPE_ptr16ptrinvoid(void* frm, UINT16 id, void* text, UINT trapID);
void ARM_TYPE_ptr16invoid(void* frm, UINT16 id, UINT trapID);
void ARM_TYPE_16invoid(UINT16 id, UINT trapID);
SINT16 ARM_TYPE_ptrin16(void* ptr, UINT trapID);
void* ARM_TYPE_ptrinptr(void* ptr, UINT trapID);

void palmossub_init(void);
void palmossub_term(void);

UINT32 palmos_gettick(void);

Err ARM_SndStreamSetVolume(UInt32 stream, Int32 volume);
void ARM_MemSet(void* destP, Int32 numBytes, UInt8 value);
void ARM_MemMove(void* destP, const void* sourceP, Int32 numBytes);
void* ARM_MemPtrNew(unsigned long sizeLE);
UInt16 ARM_StrLen(const Char* src);
#define ARM_MemPtrFree(a)				ARM_TYPE_pointerinvoid(a,sysTrapMemChunkFree)
void ARM_StrPrintF(Char* s, const Char* formatStr, UInt32 data);
void ARM_StrPrintF_2(Char* s, const Char* formatStr, UInt16 data1, UInt16 data2);
void ARM_WinDrawBitmap(BitmapPtr bitmapP, Coord x, Coord y);
void ARM_TimSecondsToDateTime(UInt32 seconds, DateTimeType* dateTimeP);
#define ARM_BmpDelete(a)				ARM_TYPE_pointerinvoid(a,sysTrapBmpDelete)
void m68k_scrnmng_create(void* ptr);
void ARM_WinDrawChars(char* src, Int16 length, Coord x, Coord y);
void ARM_WinEraseChars(char* src, Int16 length, Coord x, Coord y);
void ARM_RctSetRectangle(RectangleType* rect, Coord x, Coord y, Coord w, Coord h);

#define ARM_WinEraseRectangle(a,b)		ARM_TYPE_ptr16invoid(a,b,sysTrapWinEraseRectangle)

#define ARM_CtlDrawControl(a)			ARM_TYPE_pointerinvoid(a,sysTrapCtlDrawControl)
#define ARM_CtlGetLabel(a)				ARM_TYPE_ptrinptr(a,sysTrapCtlGetLabel)
#define ARM_CtlGetValue(a)				ARM_TYPE_ptrin16(a,sysTrapCtlGetValue)
#define ARM_CtlHideControl(a)			ARM_TYPE_pointerinvoid(a,sysTrapCtlHideControl)
#define ARM_CtlSetValue(a,b)			ARM_TYPE_ptr16invoid(a,b,sysTrapCtlSetValue)
void ARM_CtlSetLabel(ControlType* ctrl, char* text);

char* ARM_LstGetSelectionText(ListType* list, SINT16 num);
#define ARM_LstGetSelection(a)			ARM_TYPE_ptrin16(a,sysTrapLstSetSelection)
#define ARM_LstSetSelection(a,b)		ARM_TYPE_ptr16invoid(a,b,sysTrapLstSetSelection)

#define ARM_FrmCopyLabel(a,b,c)			ARM_TYPE_ptr16ptrinvoid(a,b,c,sysTrapFrmCopyLabel)
#define ARM_FrmDeleteForm(a)			ARM_TYPE_pointerinvoid(a,sysTrapFrmDeleteForm)
#define ARM_FrmDispatchEvent(a)			ARM_TYPE_pointerinvoid(a,sysTrapFrmDispatchEvent)
#define ARM_FrmDoDialog(a)				ARM_TYPE_ptrin16(a,sysTrapFrmDoDialog)
#define ARM_FrmDrawForm(a)				ARM_TYPE_pointerinvoid(a,sysTrapFrmDrawForm)
#define ARM_FrmEraseForm(a)				ARM_TYPE_pointerinvoid(a,sysTrapFrmEraseForm)
#define ARM_FrmGetActiveForm()			(FormType*)(gCall68KFuncP)(gEmulStateP,PceNativeTrapNo(sysTrapFrmGetActiveForm),NULL,kPceNativeWantA0)
#define ARM_FrmGetActiveFormID()		ARM_TYPE_nullinint(sysTrapFrmGetActiveFormID)
#define ARM_FrmGetObjectBounds(a,b,c)	ARM_TYPE_ptr16ptrinvoid(a,b,c,sysTrapFrmGetObjectBounds)
#define ARM_FrmGotoForm(a)				ARM_TYPE_16invoid(a,sysTrapFrmGotoForm)
#define ARM_FrmHideObject(a,b)			ARM_TYPE_ptr16invoid(a,b,sysTrapFrmHideObject)
#define ARM_FrmSetObjectBounds(a,b,c)	ARM_TYPE_ptr16ptrinvoid(a,b,c,sysTrapFrmSetObjectBounds)
#define ARM_FrmSetActiveForm(a)			ARM_TYPE_pointerinvoid(a,sysTrapFrmSetActiveForm)
#define ARM_FrmShowObject(a,b)			ARM_TYPE_ptr16invoid(a,b,sysTrapFrmShowObject)

UINT16 ARM_FrmGetObjectIndex(FormType* frm, UINT16 id);
void* ARM_FrmGetObjectPtr(FormType* frm, UINT16 id);
FormType* ARM_FrmInitForm(UINT16 form);

SINT16 ARM_FntWidthToOffset(char* chrP, UINT16 length, SINT16 pixelwidth, Boolean* edge, SINT16* trunc);
#define ARM_MenuEraseStatus(a)			ARM_TYPE_pointerinvoid(a,sysTrapMenuEraseStatus)
#define ARM_MenuHideItem(a)				ARM_TYPE_16invoid(a,sysTrapMenuHideItem)
#define ARM_MenuShowItem(a)				ARM_TYPE_16invoid(a,sysTrapMenuShowItem)

#define ARM_FldGetTextPtr(a)			ARM_TYPE_ptrinptr(a,sysTrapFldGetTextPtr)
#define ARM_FldGetTextHandle(a)			ARM_TYPE_ptrinptr(a,sysTrapFldGetTextHandle)
#define ARM_MemHandleLock(a)			ARM_TYPE_ptrinptr(a,sysTrapMemHandleLock)
#define ARM_MemHandleUnlock(a)			ARM_TYPE_pointerinvoid(a,sysTrapMemHandleUnlock)

#define ARM_TimGetSeconds()				ARM_TYPE_nullinint(sysTrapTimGetSeconds)

#define ARM_SndStreamDelete(a)			ARM_TYPE_intinvoid(a,sysTrapSndStreamDelete)
#define ARM_SndStreamStart(a)			ARM_TYPE_intinvoid(a,sysTrapSndStreamStart)
#define ARM_SndStreamStop(a)			ARM_TYPE_intinvoid(a,sysTrapSndStreamStop)

void ARM_EvtGetEvent(EventType *event, Int32 timeout);
Err ARM_EvtEnqueueKey(WChar ascii, UInt16 keycode, UInt16 modifier);
Boolean ARM_MenuHandleEvent(MenuBarType *menuP, EventType *event, UInt16 *error);
Boolean ARM_SysHandleEvent(EventType *event);

#define ARM_KeyCurrentState()		ARM_TYPE_nullinint(sysTrapKeyCurrentState)

void print(char *format, ...);

#ifdef __cplusplus
}
#endif

