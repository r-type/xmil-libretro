/**********
 * Copyright (c) 2004 Greg Parker.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY GREG PARKER ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 **********/

#include <PalmOS.h>
#include <PalmCompatibility.h>
#include <PalmOSGlue.h>
#include <VfsMgr.h>
#include <stdint.h>
#include <stdarg.h>
#include "peal.h"
#include "resource.h"
#include "UFD.h"


#define ByteSwap16(n) ( ((((unsigned int) n) << 8) & 0xFF00) | \
((((unsigned int) n) >> 8) & 0x00FF) )

#define ByteSwap32(n) ( ((((unsigned long) n) << 24) & 0xFF000000) | \
((((unsigned long) n) << 8) & 0x00FF0000) | \
((((unsigned long) n) >> 8) & 0x0000FF00) | \
((((unsigned long) n) >> 24) & 0x000000FF) )

static  UInt16 volRefNum;
enum {
	SEEK_SET	= 0,
	SEEK_CUR	= 1,
	SEEK_END	= 2
};
#define	FILEH_INVALID	((UInt32)-1)


void print(char *format, ...)
{
    RectangleType r, empty;
    char buf[200];

    va_list args;
    va_start(args, format);
    StrVPrintF(buf, format, args);
    va_end(args);

    r.topLeft.x = 0;
    r.topLeft.y = 0;
    r.extent.x = 160;
    r.extent.y = 160;
    WinScrollRectangle(&r, winDown, 12, &empty);
    WinEraseRectangle(&empty, 0);

    WinDrawChars(buf, StrLen(buf), 1, 1);
}

PealModule *load(void)
{
    return PealLoadFromResources('armc', 1000);
}


void unload(PealModule *m)
{
    PealUnload(m);
}

static BOOL checkHR(void) {

// HighDensity機能の有無をチェック
	UInt32 version;
	Err err;
	UInt32 density;
	UInt32 depth;
	Boolean enableColor;

// Window Managerのバージョンが4以降ならHighDensity機能は有効
// HighDensityが有効＝画面がハイレゾ　ではないことに注意
//（例：treo600はHighDenstity対応でかつ画面は標準解像度）		
	err = FtrGet(sysFtrCreator,sysFtrNumWinVersion, &version);
	if (version < 4){
		return(false);
	}
	
// HighDensityが有効な場合の画面のピクセル数の取得
// 	画面のサイズを取得		
	WinScreenGetAttribute( winScreenDensity, &density);
	if (density != kDensityDouble) {
		return(false);
	}
	
//　画面のカラーを取得
	WinScreenMode (winScreenModeGet, NULL, NULL, &depth, &enableColor);

//　16bit Color以外のときは16bitに変更しようと試みる
	if (enableColor == false) {
		return(false);
	}
	if (depth != 16) {
		depth = 16;
		err = WinScreenMode (winScreenModeSet, NULL, NULL, &depth, NULL);
		if (err) {
			return(false);
		}
	}

	return(true);
}

BOOL dlgs_savefile_68k(void* name) {

	struct _UFD_Params ufdParams;
	BOOL	ret = false;
	FormType*	form;
	
	form = FrmGetActiveForm();
	
	ufdParams.fshow_type = UFD_SHOW_ALL;
	ufdParams.skin = UFD_SKIN_DEFAULT;
	ufdParams.fattr = UFD_ATTR_RAM;
	ufdParams.ffilter[0] = '\0';
	AppCallWithCommand(UFD_CREATOR_ID, UFD_SAVE_COMMAND, &ufdParams);

	if (ufdParams.error ==  UFD_ERROR_NONE) {
		if (StrCompare(ufdParams.fpath, "RAM")) {
			StrCopy(name, ufdParams.fpath);
			if(StrCompare(ufdParams.fpath, "/")) {
				StrCat(name, "/");
			}
			StrCat(name, ufdParams.fname);
			StrCat(name, ufdParams.fextension);
			ret = true;
		}
	}
	FrmSetActiveForm(form);
	FrmDrawForm(form);
	return(ret);
}

BOOL dlgs_selectfile_68k(void* name) {

	struct _UFD_Params ufdParams;
	BOOL	ret = false;
	FormType*	form;
	
	form = FrmGetActiveForm();
	
	ufdParams.fshow_type = UFD_SHOW_ALL;
	ufdParams.skin = UFD_SKIN_DEFAULT;
	ufdParams.fattr = UFD_ATTR_RAM;
	ufdParams.ffilter[0] = '\0';
	
	AppCallWithCommand(UFD_CREATOR_ID, UFD_LOAD_COMMAND, &ufdParams);  

	if (ufdParams.error ==  UFD_ERROR_NONE) {
		StrCopy(name, ufdParams.fpath);
		if(StrCompare(ufdParams.fpath, "/")) {
			StrCat(name, "/");
		}
		StrCat(name, ufdParams.fname);
		StrCat(name, ufdParams.fextension);
		ret = true;
	}
	FrmSetActiveForm(form);
	FrmDrawForm(form);
	return(ret);
}

FileRef file_open_68k(void* path) {

	UInt32  error;
	FileRef ref;
	
	error = VFSFileOpen ( volRefNum, path, vfsModeReadWrite, &ref );
	if (!error) {
		return(ref);
	}
	error = VFSFileOpen ( volRefNum, path, vfsModeRead, &ref );
	if (!error) {
		return(ref);
	}
	return(FILEH_INVALID);
}

FileRef file_create_68k(void *path) {

	UInt32  error;
	
	error = VFSFileCreate ( volRefNum, path );
	if (!error) {
		return(file_open_68k(path));
	}

	if (error == vfsErrFileAlreadyExists) {
		if (!VFSFileDelete(volRefNum, path)) {
			if (!VFSFileCreate ( volRefNum, path )) {
				return(file_open_68k(path));
			}
		}
	}
	return(FILEH_INVALID);
}

long file_seek_68k(UInt32 handle, Int32 pointer, UInt32 method) {

	UInt32		error;
	FileOrigin	setp;
	Int32		pos;

	switch(method) {
		case SEEK_SET:
			setp = vfsOriginBeginning;
			break;
		case SEEK_CUR:
			setp = vfsOriginCurrent;
			break;
		case SEEK_END:
			setp = vfsOriginEnd;
			break;
		default:
			return(-1);
	}
	VFSFileSeek ( handle, setp, pointer );
	error = VFSFileTell ( handle, &pos );
	if (error) {
		return(-1);
	}
	return(pos);
}

UInt32 file_read_68k(UInt32 handle, void *data, UInt32 length) {
	
	UInt32 result;
	UInt32 error;

	error = VFSFileRead ( handle, length, data, &result );
	if (error) {
		return(0);
	}
	return(result);
}

UInt32 file_write_68k(UInt32 handle, const void *data, UInt32 length) {

	UInt32  result;
	UInt32  error;
	
	error = VFSFileWrite ( handle, length, data, &result );
	if (error) {
		return(0);
	}
	return(result);
}

void file_close_68k(UInt32 f) {
	VFSFileClose ( f );
}

UInt32 file_getsize_68k(UInt32 handle) {

	UInt32 result;
	UInt32 error;

	error = VFSFileSize ( handle, &result );
	if (error) {
		return(0);
	}
	return(result);
}

UInt32 file_getdatetime_68k(UInt32 handle) {

	UInt32 result;
	UInt32 error;

	error = VFSFileGetDate ( handle, vfsFileDateModified, &result );
	if (error) {
		return(0);
	}
	return (result);
}

UInt32 file_delete_68k(const void *path) {

	UInt32  error;
	
	error = VFSFileDelete(volRefNum, path);
	if (error) {
		return(-1);
	}
	return(0);
}

UInt32 file_attr_68k(UInt32 handle) {

	UInt32  error;
	UInt32  attr;
	
	error = VFSFileGetAttributes(handle, &attr);
	if (error) {
		return(-1);
	}
	return(attr);
}

short file_dircreate_68k(const void *path) {

	UInt32  error;
	
	error = VFSDirCreate(volRefNum, path);
	if (!error) {
		return(0);
	}

	return(-1);
}

void resume_error(char* text) {

	FrmCustomAlert(IDD_ALERT, text, NULL, NULL);
}

typedef struct {
	UInt32			width;
	UInt32			height;
	UInt16*			bits;
	BOOL			exist;
} _QDRAW, *QDRAW;

void scrnmng_init (void* ptr) {

	QDRAW		qd;
	
	WinHandle	winh = WinGetDisplayWindow();
	BitmapType*	winb = WinGetBitmap ( winh );
	UInt16*		winbits = BmpGetBits ( winb );

	qd = ptr;
	if ((winb == NULL) || (winbits == NULL)) {
		return;
	}
	qd->bits = (UInt16*)ByteSwap32(winbits+320*(14*2)*2);
}

static void*   arm_qs;

static Err SoundCallback(void* userDataP, SndStreamRef stream, void* bufferP, UInt32 frameCount);


typedef struct {
	SndStreamRef	hdl;
	void*			cb;
	UInt32			rate;
	UInt32			samples;
	UInt32			buffersize;
	Int32			indata;
	Int16*			nextdata;
	Int16*			extendbuffer[2];
	UInt32			pos;
} _QSOUND, *QSOUND;

Err sound_init(void* param) {
	Err		ret, err;
	UInt32	version;
	SndStreamRef	stream;
	QSOUND	qs;

	err = FtrGet(sysFileCSoundMgr, sndFtrIDVersion, &version);
	if (err) {
		return(false);
	}

	qs = arm_qs;	
	ret = (SndStreamCreate(&stream, sndOutput, ByteSwap32(qs->rate), sndInt16Little, sndStereo, SoundCallback, qs, 0, false));
	if (!ret) {
		qs->hdl = ByteSwap32(stream);
		err = SndStreamSetVolume(stream, sndGameVolume);
		if (err) ErrAlert(err);
	} else {
		ErrAlert(ret);
	}
	(void)param;
	return(ret);
}

static Err SoundCallback(void* userDataP, SndStreamRef stream, void* bufferP, UInt32 frameCount) {

	QSOUND	qs;
	UInt32	size;
	UInt32	frames = frameCount*4;
	Int8*	buffer;
	Int8*	dst;
	Int8*	nextbuffer;
	Int8*	buffertofill;
	UInt32	pos, needed, remain, newpos, count;
	Int32	current;
	UInt32	last;

	dst = bufferP;
	qs = userDataP;
	current = ByteSwap32(qs->indata);
	if (current < 0) {
		MemSet(dst, frames, 0);
		return(0);
	}
	
	buffer = (Int8*)ByteSwap32(qs->extendbuffer[current]);
	if (buffer == NULL) {
		MemSet(dst, frames, 0);
		return(0);
	}
	
	size = ByteSwap32(qs->buffersize);
	pos = ByteSwap32(qs->pos);
	newpos = pos + frames;
	if (size > newpos) {
		MemMove(dst, buffer + pos, frames);
		qs->pos = ByteSwap32(newpos);
	} else {
		needed =  size - pos;
		remain = frames - needed;
		MemMove(dst, buffer + pos, needed);
		current++;
		if (current >= 2) {
			current = 0;
		}
		nextbuffer = (Int8*)ByteSwap32(qs->extendbuffer[current]);
		buffertofill = (Int8*)ByteSwap32(qs->nextdata);
		if (nextbuffer == buffertofill) {
			last = *(UInt32*)(buffer+size-4);
			count = remain / 4;
			while (count > 0) {
				*(UInt32*)(dst + needed) = last;
				count--;
				needed += 4;
			//MemSet(dst + needed, remain, 0);
			}
			qs->pos = qs->buffersize;
		}
		else {
			MemMove(dst + needed, nextbuffer, remain);
			qs->pos = ByteSwap32(remain);
			qs->indata = ByteSwap32(current);
		}
	}
	
	(void)stream;
	return(0);
}

PealModule *arm_module = NULL;

int setup(PealModule *m)
{
    int result;
    void *main_arm;
    void **Callback_soundmng;
    void **Callback_scrnmng;
    void **Callback_print;
    void **Callback_file;
	void **arm_systicks;
	void **Callback_newmemory;
	void **Callback_selectfile;
	void **Callback_savefile;
	void **Callback_resumuerror;

	if (m == NULL) return(3);
	arm_module = m;
	
	arm_qs = PealLookupSymbol(m, "QSound");
	if (arm_qs == NULL) return(1);

    // Set an ARM variable
    Callback_resumuerror = (void **)PealLookupSymbol(m, "Callback_resume");
	if (Callback_resumuerror == NULL) return(5);
    *Callback_resumuerror = (void *)ByteSwap32(&resume_error);
	
    Callback_scrnmng = (void **)PealLookupSymbol(m, "Callback_scrnmng");
	if (Callback_scrnmng == NULL) return(5);
    *Callback_scrnmng = (void *)ByteSwap32(&scrnmng_init);
	
    Callback_selectfile = (void **)PealLookupSymbol(m, "Callback_fileselect");
	if (Callback_selectfile == NULL) return(5);
    *Callback_selectfile = (void *)ByteSwap32(&dlgs_selectfile_68k);
	
    Callback_savefile = (void **)PealLookupSymbol(m, "Callback_filesave");
	if (Callback_savefile == NULL) return(5);
    *Callback_savefile = (void *)ByteSwap32(&dlgs_savefile_68k);

    Callback_print = (void **)PealLookupSymbol(m, "Callback_print");
	if (Callback_print == NULL) return(5);
    *Callback_print = (void *)ByteSwap32(&print);
	
    Callback_newmemory = (void **)PealLookupSymbol(m, "Callback_memory");
	if (Callback_newmemory == NULL) return(5);
    *Callback_newmemory = (void *)ByteSwap32(&MemGluePtrNew);
	
    Callback_soundmng = (void **)PealLookupSymbol(m, "Callback_sound");
	if (Callback_soundmng == NULL) return(5);
    *Callback_soundmng = (void *)ByteSwap32(&sound_init);

    Callback_file = (void **)PealLookupSymbol(m, "Callback_vfs");
	if (Callback_file == NULL) return(5);
    *Callback_file = (void *)ByteSwap32(&file_open_68k);
    *(Callback_file+1) = (void *)ByteSwap32(&file_create_68k);
    *(Callback_file+2) = (void *)ByteSwap32(&file_seek_68k);
    *(Callback_file+3) = (void *)ByteSwap32(&file_read_68k);
    *(Callback_file+4) = (void *)ByteSwap32(&file_write_68k);
    *(Callback_file+5) = (void *)ByteSwap32(&file_close_68k);
    *(Callback_file+6) = (void *)ByteSwap32(&file_getsize_68k);
    *(Callback_file+7) = (void *)ByteSwap32(&file_getdatetime_68k);
    *(Callback_file+8) = (void *)ByteSwap32(&file_delete_68k);
    *(Callback_file+9) = (void *)ByteSwap32(&file_attr_68k);
    *(Callback_file+10) = (void *)ByteSwap32(&file_dircreate_68k);
	
	arm_systicks = (void **)PealLookupSymbol(m, "systicks");
	if (arm_systicks == NULL) return(1);
    *arm_systicks = (void *)ByteSwap32(SysTicksPerSecond());

	WinSetDrawWindow(NULL);
    // Call an ARM function that uses global data
    main_arm = PealLookupSymbol(m, "x1main");
	if (main_arm == NULL) {
		return(6);
	}
    result = PealCall(m, main_arm, NULL);
	return(result);
}

static char* const ARMSYMBOL[] = {"HandleMenuChoice", "SetHardKey", "palmkbd_modifiers"};

Boolean FormHandler(EventType* event) {

	void	*arm_call = NULL;
	UInt32	data = 0;
	FormType*	frm;
	Int16	select, base;
	ListType*	list;
	int		num = 0;
				
	switch (event->eType) {
		case popSelectEvent:
			switch (event->data.popSelect.listID) {
				case IDC_KEYLIST1:
				case IDC_KEYLIST2:
				case IDC_KEYLIST3:
				case IDC_KEYLIST4:
					select = event->data.popSelect.selection;
					base = event->data.popSelect.listID - IDC_KEYLIST1;
					frm = FrmGetActiveForm();
					if (select < 12) {
						list = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, IDC_KEYLIST1 + base));
						CtlSetLabel(FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, IDC_HARDKEY1 + base)), LstGetSelectionText(list, select));
					} else {
						CtlSetLabel(FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, IDC_HARDKEY1 + base)), "N/A");
					}
					num = 1;
					data = ((UInt32)select << 16) | event->data.popSelect.listID;
					break;
				default:
					break;
			}
			break;
		case ctlSelectEvent:
			switch (event->data.ctlSelect.controlID) {
				case IDC_FDD0:
					data = IDM_FDD0OPEN;
					break;
				case IDC_FDD1:
					data = IDM_FDD1OPEN;
					break;
				case IDC_NOWAIT:
					data = IDM_NOWAIT;
					break;
				case IDC_DISPCLOCK:
					data = IDM_DISPCLOCK;
					break;
				case IDC_DISPFRAME:
					data = IDM_DISPFRAME;
					break;
				case IDC_KANA:
				case IDC_CAPS:
				case IDC_SHIFT:
				case IDC_CTRL:
				case IDC_GRAPH:
					num = 2;
					data = event->data.ctlSelect.controlID;
					break;
				default:
					break;
			}
			break;
		default:
			break;
	}
	if (data) {
		arm_call = PealLookupSymbol(arm_module, ARMSYMBOL[num]);
		PealCall(arm_module, arm_call, (void*)data);
		return(true);
	}

	return(false);
}

UInt32 PilotMain(UInt16 cmd, void *cmdPBP, UInt16 launchFlags)
{
    PealModule *m;
	UInt32 volIterator = vfsIteratorStart;
    FormType *frmP;
	Err err;
	int	ret;
	const RGBColorType	white = {0, 255, 255, 255};

    // we don't handle search et al.
    if (cmd != sysAppLaunchCmdNormalLaunch) return 0;
    
	if (checkHR() == false) {
		FrmCustomAlert(IDD_ALERT, "cannot run under this device.", NULL, NULL);
		return 0;
	}
	FrmGotoForm(IDD_MAIN);
	frmP = FrmInitForm(IDD_MAIN);
	FrmSetActiveForm(frmP);
	FrmSetEventHandler(frmP, FormHandler);
	WinSetBackColorRGB(&white, NULL);

	while (volIterator != vfsIteratorStop) {
		err = VFSVolumeEnumerate(&volRefNum, &volIterator);
		if (err == errNone) {
			break;
		}
	}
	
	m = load();
	ret = setup(m);
    unload(m);
	switch (ret) {
		case 1:
			FrmCustomAlert(IDD_ALERT, "cannot find symbols.", NULL, NULL);
			break;
		case 2:
			FrmCustomAlert(IDD_ALERT, "cannot create screen.", NULL, NULL);
			break;
		case 3:
			FrmCustomAlert(IDD_ALERT, "cannot load arm code.", NULL, NULL);
			break;
		case 5:
			FrmCustomAlert(IDD_ALERT, "cannot find callback.", NULL, NULL);
			break;
		case 6:
			FrmCustomAlert(IDD_ALERT, "cannot find main.", NULL, NULL);
			break;
		case 0:
		default:
			break;
	}
	
	FrmEraseForm(frmP);
	FrmDeleteForm(frmP);
	FrmCloseAllForms();
	
	(void)launchFlags;
	(void)cmdPBP;
    return 0;
}
