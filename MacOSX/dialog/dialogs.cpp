#include	"compiler.h"
#include	"strres.h"
#include	"dialogs.h"
#include	"dosio.h"
#include	"fdefine.h"
#include	"resource.h"


Handle GetDlgItem(DialogPtr hWnd, short pos) {

	Handle	ret;
	Rect	rct;
	short	s;

	GetDialogItem(hWnd, pos, &s, &ret, &rct);
	return(ret);
}


// ---- file select

static const BYTE pathsep[2] = {0x01, ':'};

static void backpalcalcat(char *dst, int leng, const BYTE *src) {

	int		dlen;
	int		slen;

	if (leng < 2) {
		return;
	}
	slen = src[0];
	dlen = strlen(dst);
	if ((slen + dlen + 1) > leng) {
		if (slen >= leng) {
			slen = leng - 1;
			dlen = 0;
		}
		else {
			dlen = leng - slen - 1;
		}
	}
	dst[slen + dlen] = '\0';
	while(dlen--) {
		dst[slen + dlen] = dst[dlen];
	}
	CopyMemory(dst, (char *)src + 1, slen);
}

static void fsspec2path(FSSpec *fs, char *dst, int leng) {

	CInfoPBRec	cipbr;
	Str255		dname;

	if (!leng) {
		return;
	}
	dst[0] = '\0';
	backpalcalcat(dst, leng, fs->name);
	cipbr.dirInfo.ioVRefNum = fs->vRefNum;
	cipbr.dirInfo.ioDrParID = fs->parID;
	cipbr.dirInfo.ioFDirIndex = -1;
	cipbr.dirInfo.ioNamePtr = dname;
	do {
		cipbr.dirInfo.ioDrDirID = cipbr.dirInfo.ioDrParID;
		if (PBGetCatInfo(&cipbr, FALSE) != noErr) {
			break;
		}
		backpalcalcat(dst, leng, pathsep);
		backpalcalcat(dst, leng, dname);
	} while(cipbr.dirInfo.ioDrDirID != fsRtDirID);
}

static pascal void dummyproc(NavEventCallbackMessage sel, NavCBRecPtr prm,
													NavCallBackUserData ud) {
	switch( sel )
	{
		case kNavCBCancel:
		case kNavCBAccept:
			QuitAppModalLoopForWindow(prm->window);
			break;
		default:
			break;
	}
}

static pascal Boolean NavLaunchServicesFilterProc( AEDesc* theItem, void* info, NavCallBackUserData ioUserData, NavFilterModes filterMode)
{
	#pragma unused( info )
	OSStatus	err			= noErr;
	Boolean		showItem	= false;
    FSSpec		fsSpec;
    int			ret;
    char		name[MAX_PATH];
	AEDesc 		coerceDesc	= { NULL, NULL };
		
	if ( filterMode == kNavFilteringBrowserList ) {
		if ( theItem->descriptorType != typeFSS ) {
			err = AECoerceDesc( theItem, typeFSS, &coerceDesc );
			theItem = &coerceDesc;
		}
		if ( err == noErr ) {
			err = AEGetDescData( theItem, &fsSpec, sizeof(fsSpec) );
        }
		AEDisposeDesc( &coerceDesc );
        
        if (err == noErr) {
            fsspec2path(&fsSpec, name, MAX_PATH);
            if (file_attr(name) == FILEATTR_DIRECTORY) {
                showItem = true;
            }
            else {
                ret = file_getftype(name);
                switch (*(int*)ioUserData) {
                    case OPEN_FDD:
                        if (ret == FTYPE_D88 || ret == FTYPE_BETA) {
                            showItem = true;
                        }
                        break;
                   default:
                        break;
                }
            }
        }
	}
	return( showItem );
}

BOOL dlgs_selectfile(char *name, int size, WindowRef parent, int opentype) {

	NavDialogRef				navWin;
	BOOL						ret;
	OSErr						err;
    NavDialogCreationOptions	optNav;
	NavReplyRecord				reply;
	NavEventUPP					proc;
	long						count;
	long						i;
	FSSpec						fss;
    NavObjectFilterUPP			navFilterProc;

	ret = FALSE;
	err = NavGetDefaultDialogCreationOptions(&optNav);
	if (err != noErr) {
		goto fsel_exit;
	}
    optNav.clientName = CFSTR("X millennium");
	if (parent) {
		optNav.modality=kWindowModalityWindowModal;
		optNav.parentWindow=parent;
	}
    switch (opentype) {
        case OPEN_FONT:
            optNav.message = CFCopyLocalizedString(CFSTR("Choose font file."),"FontSelect Message");
            break;
        default:
            break;
    }
	proc = NewNavEventUPP(dummyproc);
    navFilterProc = NewNavObjectFilterUPP( NavLaunchServicesFilterProc );
    ret=NavCreateGetFileDialog(&optNav,NULL,proc,NULL,navFilterProc,&opentype,&navWin);
    NavDialogRun(navWin);
    RunAppModalLoopForWindow(NavDialogGetWindow(navWin));
    NavDialogGetReply(navWin, &reply);
    if (optNav.message) CFRelease(optNav.message);
    NavDialogDispose(navWin);
	DisposeNavEventUPP(proc);
    DisableAllMenuItems(GetMenuHandle(IDM_EDIT));
	
	if ((!reply.validRecord) && (ret != noErr)) {
		goto fsel_exit;
	}
	err = AECountItems(&reply.selection, &count);
	if (err == noErr) {
		for (i=1; i<= count; i++) {
			err = AEGetNthPtr(&reply.selection, i, typeFSS, NULL, NULL,
													&fss, sizeof(fss), NULL);
			if (err == noErr) {
				fsspec2path(&fss, name, size);
				ret = TRUE;
				break;
			}
		}
		err = NavDisposeReply(&reply);
	}

fsel_exit:
	return(ret);
}

BOOL dlgs_selectwritefile(char *name, int size, const char *def, OSType type, WindowRef parentWindow) {
	OSType				sign='XMil';
	NavEventUPP			eventUPP;
	NavReplyRecord		reply;
	DescType			rtype;
	OSErr				ret;
	AEKeyword			key;
	Size				len;
    FSRef				parent;
    UniCharCount		ulen;
    UniChar*			buffer = NULL;
	NavDialogRef		navWin;
	FSSpec				fsc;
    NavDialogCreationOptions	copt;

	InitCursor();
    NavGetDefaultDialogCreationOptions(&copt);
    copt.clientName = CFSTR("X millennium");
    copt.parentWindow = parentWindow;
    copt.saveFileName = CFStringCreateWithCString(NULL, def, CFStringGetSystemEncoding());
    copt.optionFlags += kNavPreserveSaveFileExtension;
    copt.modality = kWindowModalityWindowModal;
    switch (type) {
        case 'BMP ':
            copt.message = CFCopyLocalizedString(CFSTR("Save the screen as BMP file."),"ScreenShot Message");
            break;
        default:
            break;
    }
	eventUPP=NewNavEventUPP( dummyproc );
    NavCreatePutFileDialog(&copt, type, sign, eventUPP, NULL, &navWin);
    
    NavDialogRun(navWin);
    RunAppModalLoopForWindow(NavDialogGetWindow(navWin));
    
    NavDialogGetReply(navWin, &reply);
    NavDialogDispose(navWin);
	DisposeNavEventUPP(eventUPP);

	if( reply.validRecord)
	{
		ret=AEGetNthPtr( &(reply.selection),1,typeFSRef,&key,&rtype,(Ptr)&parent,(long)sizeof(FSRef),&len );
        ulen = (UniCharCount)CFStringGetLength(reply.saveFileName);
        buffer = (UniChar*)NewPtr(ulen);
        CFStringGetCharacters(reply.saveFileName, CFRangeMake(0, ulen), buffer);
        ret = FSCreateFileUnicode(&parent, ulen, buffer, kFSCatInfoNone, NULL, NULL, &fsc);
        DisposePtr((Ptr)buffer);
		NavDisposeReply( &reply );
        if (ret == noErr) {
			fsspec2path(&fsc, name, size);
            return true;
        }
	}
	return( false );
}

void getFieldText(ControlRef cRef, char* buffer) {
    Size	size, outSize;
    GetControlDataSize(cRef, kControlNoPart, kControlStaticTextTextTag, &size);
    GetControlData(cRef, kControlNoPart, kControlStaticTextTextTag, size, buffer, &outSize);
    *(buffer+outSize)=NULL;
}

ControlRef getControlRefByID(OSType sign, int id, WindowRef win) {
    ControlRef	conRef;
    ControlID	conID;

    conID.signature=sign;
    conID.id=id;
    GetControlByID(win, &conID, &conRef);
    return conRef;
}    
