#include	"compiler.h"

void *Callback_print = NULL;
void *Callback_scrnmng = NULL;
UInt32  systicks = 100;
void *Callback_memory = NULL;

typedef struct tagLocals16{
	UInt16 id;
} Locals16;

typedef struct tagLocals3216{
	UInt32	ptr;
	UInt16	data;
} Locals3216;

#define PalmAPI(a,b)	(gCall68KFuncP)(gEmulStateP,PceNativeTrapNo(a),&b,sizeof(b))

void ARM_TYPE_pointerinvoid (void* ptr, UINT trapID) {
	ArgOne params;
	params.ptr = ByteSwap32(ptr);
	PalmAPI(trapID, params);
}

int ARM_TYPE_nullinint (UINT trapID) {
	return((gCall68KFuncP)(gEmulStateP, PceNativeTrapNo(trapID), NULL, 0));
}

void ARM_TYPE_intinvoid (UINT data, UINT trapID) {
	ArgOne params;
	params.ptr = ByteSwap32(data);
	PalmAPI(trapID, params);
}

void ARM_TYPE_ptr16ptrinvoid(void* frm, UINT16 id, void* text, UINT trapID) {

	typedef struct tagLocals{
		UInt32	ptr;
		UINT16	objid;
		UINT16	texthi;
		UINT16	textlo;
	} Locals;
	Locals params;
	
	params.ptr = ByteSwap32(frm);
	params.objid = ByteSwap16(id);
	params.texthi = ByteSwap16(text >> 16);
	params.textlo = ByteSwap16(text & 0xffff);
	
	PalmAPI(trapID, params);
}

void ARM_TYPE_ptr16invoid(void* frm, UINT16 id, UINT trapID) {

	Locals3216 params;

	params.ptr = ByteSwap32(frm);
	params.data = ByteSwap16(id);
	
	PalmAPI(trapID, params);
}

void ARM_TYPE_16invoid(UINT16 id, UINT trapID) {

	Locals16 params;

	params.id = ByteSwap16(id);
	
	PalmAPI(trapID, params);
}

SINT16 ARM_TYPE_ptrin16(void* ptr, UINT trapID) {

	ArgOne params;
	params.ptr = ByteSwap32(ptr);
	return((SINT16)PalmAPI(trapID, params));
}

void* ARM_TYPE_ptrinptr(void* ptr, UINT trapID) {
	ArgOne params;
	params.ptr = ByteSwap32(ptr);
	return((void*)PalmAPI(trapID, params));
}

void palmossub_init(void) {
}


void palmossub_term(void) {
}

#define ARM_TimGetTicks() ARM_TYPE_nullinint(sysTrapTimGetTicks)


SINT16 ARM_FntWidthToOffset(char* chrP, UINT16 length, SINT16 pixelwidth, Boolean* edge, SINT16* trunc) {
	typedef struct tagLocals{
		UInt32	ptr;
		UInt16  len;
		UInt16  width;
		UInt32  edgeP;
		UInt32  res;
	} Locals;
	Locals params;
	
	params.ptr = ByteSwap32(chrP);
	params.len = ByteSwap16(length);
	params.width = ByteSwap16(pixelwidth);
	params.edgeP = ByteSwap32(edge);
	params.res = ByteSwap32(trunc);
	
	return((SINT16)PalmAPI(sysTrapFntWidthToOffset, params));
}

UINT16 ARM_FrmGetObjectIndex(FormType* frm, UINT16 id) {

	Locals3216 params;
	
	params.ptr = ByteSwap32(frm);
	params.data = ByteSwap16(id);
	
	return((UINT16)PalmAPI(sysTrapFrmGetObjectIndex, params));
}

void* ARM_FrmGetObjectPtr(FormType* frm, UINT16 id) {

	Locals3216 params;

	params.ptr = ByteSwap32(frm);
	params.data = ByteSwap16(id);
	
	return ((void*)(gCall68KFuncP)(gEmulStateP, PceNativeTrapNo(sysTrapFrmGetObjectPtr), &params, sizeof(params) | kPceNativeWantA0));
}


Err ARM_SndStreamSetVolume(UInt32 stream, Int32 volume) {
	typedef struct tagLocals{
		UInt32	ref;
		UInt32  set;
	} Locals;
	Locals params;
	
	params.ref = ByteSwap32(stream);
	params.set = ByteSwap32(volume);
	
	return((Err)PalmAPI(sysTrapSndStreamSetVolume, params));
}

void ARM_MemSet(void* destP, Int32 numBytes, UInt8 value) {
	Int32 i;
	char *p=(char *)destP;

	if (!destP) return;
	for (i=0; i<numBytes; i++)
		*p++ = value;

}

void ARM_MemMove(void* destP, const void* sourceP, Int32 numBytes) {
	typedef struct tagLocals{
		UInt32	dst;
		UInt32  src;
		UInt32  size;
	} Locals;
	Locals params;
	
	params.dst = ByteSwap32(destP);
	params.src = ByteSwap32(sourceP);
	params.size = ByteSwap32(numBytes);
	
	PalmAPI(sysTrapMemMove, params);
}

void* ARM_MemPtrNew(unsigned long sizeLE) {
	unsigned long   sizeBE = ByteSwap32(sizeLE);
	return(
	(void*)(gCall68KFuncP)(gEmulStateP, (unsigned long)Callback_memory, &sizeBE, 4|kPceNativeWantA0)
	);
}

UInt16 ARM_StrLen(const Char* src) {
	typedef struct tagLocals{
		 UInt32 chars;
	} Locals;
	
	Locals params;
	
	params.chars = ByteSwap32(src);
	
	return ((UInt16)PalmAPI(sysTrapStrLen, params));
}

void ARM_StrPrintF_2(Char* s, const Char* formatStr, UInt16 data1, UInt16 data2) {
	typedef struct tagLocals{
		UInt32  dst;
		UInt32  fmt;
		UInt16  num1;
		UInt16  num2;
	} Locals;
	Locals params;
	
	params.dst = ByteSwap32(s);
	params.fmt = ByteSwap32(formatStr);
	params.num1 = ByteSwap16(data1);
	params.num2 = ByteSwap16(data2);
	
	PalmAPI(sysTrapStrPrintF, params);
}

void ARM_StrPrintF(Char* s, const Char* formatStr, UInt32 data) {
	ARM_StrPrintF_2(s, formatStr, (UInt16)data, (UInt16)(data >> 16));
}

void ARM_WinDrawBitmap(BitmapPtr bitmapP, Coord x, Coord y) {
	typedef struct tagLocals{
		UInt32   ptr;
		UInt16  left;
		UInt16   top;
	} Locals;
	Locals params;
	
	params.ptr = ByteSwap32( bitmapP );
	params.left = ByteSwap16( x );
	params.top = ByteSwap16( y );
	
	PalmAPI(sysTrapWinDrawBitmap, params);
}

void ARM_WinDrawChars(char* src, Int16 length, Coord x, Coord y) {
	typedef struct tagLocals{
		UInt32	ptr;
		UInt16	len;
		UInt16	left;
		UInt16	top;
	} Locals;
	Locals params;
	
	params.ptr = ByteSwap32( src );
	params.len = ByteSwap16( length );
	params.left = ByteSwap16( x );
	params.top = ByteSwap16( y );
	
	PalmAPI(sysTrapWinDrawChars, params);
}

void ARM_WinEraseChars(char* src, Int16 length, Coord x, Coord y) {
	typedef struct tagLocals{
		UInt32	ptr;
		UInt16	len;
		UInt16	left;
		UInt16	top;
	} Locals;
	Locals params;
	
	params.ptr = ByteSwap32( src );
	params.len = ByteSwap16( length );
	params.left = ByteSwap16( x );
	params.top = ByteSwap16( y );
	
	PalmAPI(sysTrapWinEraseChars, params);
}

void ARM_RctSetRectangle(RectangleType* rect, Coord x, Coord y, Coord w, Coord h) {
	typedef struct tagLocals{
		UInt32	ptr;
		UInt16  left;
		UInt16  top;
		UInt16  width;
		UInt16  height;
	} Locals;
	Locals params;
	
	params.ptr = ByteSwap32( rect );
	params.left = ByteSwap16( x );
	params.top = ByteSwap16( y );
	params.width = ByteSwap16( w );
	params.height = ByteSwap16( h );
	
	PalmAPI(sysTrapRctSetRectangle, params);
}

void ARM_CtlSetLabel(ControlType* ctrl, char* label) {
	typedef struct tagLocals{
		UInt32	ctrlptr;
		UInt32	text;
	} Locals;
	Locals params;
	
	params.ctrlptr = ByteSwap32( ctrl );
	params.text = ByteSwap32( label );
	
	PalmAPI(sysTrapCtlSetLabel, params);
}

char* ARM_LstGetSelectionText(ListType* list, SINT16 num) {
	Locals3216 params;

	params.ptr = ByteSwap32(list);
	params.data = ByteSwap16(num);
	
	return ((char*)(gCall68KFuncP)(gEmulStateP, PceNativeTrapNo(sysTrapLstGetSelectionText), &params, sizeof(params) | kPceNativeWantA0));
}

void ARM_TimSecondsToDateTime(UInt32 seconds, DateTimeType* dateTimeP) {
	typedef struct tagLocals{
		UInt32	data;
		UInt32	date;
	} Locals;
	Locals params;
	
	params.data = ByteSwap32( seconds );
	params.date = ByteSwap32( dateTimeP );
	
	PalmAPI(sysTrapTimSecondsToDateTime, params);
}

UINT32 palmos_gettick(void) {
	return( ARM_TimGetTicks() * (1000 / systicks));
}

void ARM_EvtGetEvent(EventType *event, Int32 timeout) {
	typedef struct tagLocals{
		UInt32 event;
		Int32 timeout;
	} Locals;
	
	Locals params;
	
	params.event = ByteSwap32(event);
	params.timeout = ByteSwap32(timeout);
	
	PalmAPI(sysTrapEvtGetEvent, params);
}

Err ARM_EvtEnqueueKey(WChar ascii, UInt16 keycode, UInt16 modifier) {
	typedef struct tagLocals{
		UInt16 chr;
		UInt16 key;
		UInt16 mod;
	} Locals;
	
	Locals params;
	
	params.chr = ByteSwap16(ascii);
	params.key = ByteSwap16(keycode);
	params.mod = ByteSwap16(modifier);
	
	return((Err)PalmAPI(sysTrapEvtEnqueueKey, params));
}

Boolean ARM_MenuHandleEvent(MenuBarType *menuP, EventType *event, UInt16 *error) {
	typedef struct tagLocals{
		UInt32 menu;
		UInt32 eve;
		UInt32 err;
	} Locals;
	
	Locals params;
	
	params.menu = ByteSwap32(menuP);
	params.eve = ByteSwap32(event);
	params.err = ByteSwap32(error);
	
	return((Boolean)PalmAPI(sysTrapMenuHandleEvent, params));
}

Boolean ARM_SysHandleEvent(EventType *event) {
	typedef struct tagLocals{
		UInt32 event;
	} Locals;
	
	Locals params;
	
	params.event = ByteSwap32(event);
	
	return((Boolean)PalmAPI(sysTrapSysHandleEvent, params));
}

FormType* ARM_FrmInitForm(UINT16 form) {

	Locals16 params;
	
	params.id = ByteSwap16(form);
	return((FormType*)(gCall68KFuncP)(gEmulStateP, PceNativeTrapNo(sysTrapFrmInitForm), &params, sizeof(params) | kPceNativeWantA0));
}

void m68k_scrnmng_create(void* ptr) {
	UInt32 arg;
	arg = ByteSwap32(ptr);

	(gCall68KFuncP)(gEmulStateP, (unsigned long)Callback_scrnmng, &arg, 4);
}

void print(char *format, ...) {
	UInt32 arg;
	arg = ByteSwap32(format);

	(gCall68KFuncP)(gEmulStateP, (unsigned long)Callback_print, &arg, 4);
}

