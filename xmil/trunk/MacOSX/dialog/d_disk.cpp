#include	"compiler.h"
#include	"strres.h"
#include	"resource.h"
#include	"dosio.h"
#include	"dialog.h"
#include	"dialogs.h"
#include	"pccore.h"
#include	"fddfile.h"
#include	"diskdrv.h"
#include	"newdisk.h"
#include	"xmil.h"
#include	"fdefine.h"

void dialog_changefdd(BYTE drv) {

	char	fname[MAX_PATH];

	if (drv < 4) {
		if (dlgs_selectfile(fname, sizeof(fname), hWndMain, OPEN_FDD)) {
			diskdrv_setfddex(drv, fname, file_getftype(fname), 0);

			switch (drv) {
				case 0:
					EnableMenuItem(GetMenuHandle(IDM_FDD0), IDM_FDD0EJECT);
					break;
				case 1:
					EnableMenuItem(GetMenuHandle(IDM_FDD1), IDM_FDD1EJECT);
					break;
				case 2:
					EnableMenuItem(GetMenuHandle(IDM_FDD2), IDM_FDD2EJECT);
					break;
				case 3:
					EnableMenuItem(GetMenuHandle(IDM_FDD3), IDM_FDD3EJECT);
					break;
				default:
					break;
			}

		}
	}
}


// ---- newdisk

static const char str_newdisk[] = "newdisk.d88";

typedef struct {
	BYTE	fdtype;
	char	label[16+1];
} NEWDISK;

static const EventTypeSpec	hicommandEvents[]={ { kEventClassCommand, kEventCommandProcess },};

static pascal OSStatus cfWinproc(EventHandlerCallRef myHandler, EventRef event, void* userData) {
    OSStatus	err = eventNotHandledErr;
    HICommand	cmd;
	NEWDISK		disk;
	char		disklabel[256];
	char		path[MAX_PATH];
	int			media;
	WindowRef	win;
	
	win = (WindowRef)userData;
    if (GetEventClass(event)==kEventClassCommand && GetEventKind(event)==kEventCommandProcess ) {
        GetEventParameter(event, kEventParamDirectObject, typeHICommand, NULL, sizeof(HICommand), NULL, &cmd);
        switch (cmd.commandID)
        {
            case kHICommandOK:
                getFieldText(getControlRefByID('name', 0, win), disklabel);
                media = GetControl32BitValue(getControlRefByID('mode', 0, win));
				if (media == 0) {
					disk.fdtype = (DISKTYPE_2DD << 4);
				}
				else {
					disk.fdtype = (DISKTYPE_2HD << 4);
				}
				strncpy(disk.label, disklabel, sizeof(disk.label));
				if (dlgs_selectwritefile(path, sizeof(path), str_newdisk, '.D88', win)) {
					newdisk_fdd(path, disk.fdtype, disk.label);
				}
                QuitAppModalLoopForWindow(win);
                err=noErr;
                break;
                
            case kHICommandCancel:
                QuitAppModalLoopForWindow(win);
                err=noErr;
                break;
                
            default:
                break;
        }
    }

	(void)myHandler;
	(void)userData;
    return err;
}

static void makeNibWindow (IBNibRef nibRef) {
    OSStatus		err;
    EventHandlerRef	ref;
	WindowRef		diskWin = NULL;
    
    err = CreateWindowFromNib(nibRef, CFSTR("NewDiskWindow"), &diskWin);
    if (err == noErr) {
        InstallWindowEventHandler (diskWin, NewEventHandlerUPP(cfWinproc), GetEventTypeCount(hicommandEvents), hicommandEvents, (void *)diskWin, &ref);
		ShowWindow(diskWin);
        SetKeyboardFocus(diskWin, getControlRefByID('name', 0, diskWin), kControlFocusPrevPart);
        RunAppModalLoopForWindow(diskWin);
		HideWindow(diskWin);
		DisposeWindow(diskWin);
    }
    return;
}

void dialog_newdisk( void ) {
    OSStatus	err;
    IBNibRef	nibRef;

    err = CreateNibReference(CFSTR("main"), &nibRef);
    if (err ==noErr ) {
        makeNibWindow (nibRef);
        DisposeNibReference ( nibRef);
    }
    return;
}
