#include	"compiler.h"
#include	"strres.h"
#include	"resource.h"
#include	"xmil.h"
#include	"sysmng.h"
#include	"dialog.h"
#include	"dialogs.h"
#include	"pccore.h"
#include	"palettes.h"


enum {kTabMasterSig = 'PRTT',kTabMasterID = 1000,kTabPaneSig= 'PRTB',kPrefControlsSig = 'PREF'};
enum {kDummyValue = 0,kMaxNumTabs= 3};

static void setupJoyConfig(OSType type, WindowRef win);
static void setTitle(OSType type, char* elementName, WindowRef win);
static pascal OSStatus PrefsTabEventHandlerProc( EventHandlerCallRef inCallRef, EventRef inEvent, void* inUserData );
static void SetInitialTabState(WindowRef theWindow);

static void setConfig(WindowRef win) {
    UInt32	dval;
    UInt16	wval;
    UInt8	mval;
    bool	bval;
	UINT	update = 0;
	UINT	renewal = 0;
    
    dval=GetControlValue(getControlRefByID('Rate', 0, win));
    if (dval==1) {
        wval = 11025;
    }
    else if (dval==2) {
        wval = 22050;
    }
    else {
        wval = 44100;
    }
	if (xmilcfg.samplingrate != wval) {
		xmilcfg.samplingrate = wval;
		update |= SYS_UPDATECFG;
		corestat.soundrenewal = 1;
	}
    
    dval = getFieldValue('Bufr', win);
	if (dval < 50) {
		dval = 50;
	}
	else if (dval > 1000) {
		dval = 1000;
	}
    if (dval != xmilcfg.delayms) {
		xmilcfg.delayms = dval;
		update |= SYS_UPDATECFG;
		corestat.soundrenewal = 1;
    }
//    dval = getFieldValue('seek');
//    if (dval != xmilcfg.MOTORVOL) {
//	INIT_RENEWAL xmilcfg.MOTORVOL = dval;
//    }
    dval = getFieldValue('slte', win);
	if (dval > 255) {
		dval = 255;
	}
    if (dval != xmilcfg.skiplight) {
		xmilcfg.skiplight = dval;
		renewal = 1;
    }
    bval=GetControlValue(getControlRefByID('slin', 0, win));
    if (bval != xmilcfg.skipline) {
		xmilcfg.skipline = bval;
		renewal = 1;
    }
	if (renewal) {
		pal_reset();
		update |= SYS_UPDATECFG;
	}
	sysmng_update(update);
}

#define	setCheckBox(a,b)	SetControlValue(getControlRefByID(b,0,win), a)
#define	setValue(a,b)		setCheckBox(a,b)

static void setControlText(long data, OSType sign, WindowRef win) {
    Str255	title;

    NumToString(data, title);
    SetControlData(getControlRefByID(sign, 0, win), kControlNoPart, kControlStaticTextTextTag, *title, title+1);
}

static void setName (OSType type, WindowRef win) {
    char elementName[256] = "----";
//    if (getJoypadName(type, elementName)) {
        if (elementName) {
            setTitle(type, elementName, win);
        }
//    }
}

static void initPrefWindow(WindowRef win) {
    int	data;

    if (xmilcfg.samplingrate == 11025) {
        data=1;
    }
    else if (xmilcfg.samplingrate == 22050) {
        data=2;
    }
    else {
        data=3;
    }
    setValue(data, 'Rate');
    setControlText(xmilcfg.delayms, 'Bufr', win);
//    setControlText(xmilcfg.MOTORVOL, 'seek');
    setControlText(xmilcfg.skiplight, 'slte', win);
	setValue(xmilcfg.skipline, 'slin');

//    setName(JOYPAD_UP, win);
//    setName(JOYPAD_DOWN, win);
//    setName(JOYPAD_LEFT, win);
//    setName(JOYPAD_RIGHT, win);
//    setName(JOYPAD_ABUTTON, win);
//    setName(JOYPAD_BBUTTON, win);

//    initTemporal();
}

static pascal OSStatus joyWinproc(EventHandlerCallRef myHandler, EventRef event, void* userData) {
    OSStatus	err = eventNotHandledErr;
    HICommand	cmd;

    if (GetEventClass(event)==kEventClassCommand && GetEventKind(event)==kEventCommandProcess ) {
        GetEventParameter(event, kEventParamDirectObject, typeHICommand, NULL, sizeof(HICommand), NULL, &cmd);
        switch (cmd.commandID)
        {
            case 'notJ':
                HideSheetWindow((WindowRef)userData);
                DisposeWindow((WindowRef)userData);
                err = noErr;
                break;
            default:
                break;
        }
    }

    return err;
}

static OSErr pushJoy (OSType type, WindowRef win) {
    setupJoyConfig(type, win);
    return noErr;
}

static pascal OSStatus pfWinproc(EventHandlerCallRef myHandler, EventRef event, void* userData) {
    OSStatus	err = eventNotHandledErr;
    HICommand	cmd;
    
    if (GetEventClass(event)==kEventClassCommand && GetEventKind(event)==kEventCommandProcess ) {
        GetEventParameter(event, kEventParamDirectObject, typeHICommand, NULL, sizeof(HICommand), NULL, &cmd);
        switch (cmd.commandID)
        {
            case kHICommandCancel:
                QuitAppModalLoopForWindow((WindowRef)userData);
                err=noErr;
                break;
                
            case kHICommandOK:
                setConfig((WindowRef)userData);
//                changeJoyPadSetup();                
                QuitAppModalLoopForWindow((WindowRef)userData);
                err=noErr;
                break;

//            case JOYPAD_UP:
//            case JOYPAD_DOWN:
//            case JOYPAD_LEFT:
//            case JOYPAD_RIGHT:
//            case JOYPAD_ABUTTON:
//            case JOYPAD_BBUTTON:
			case 'jpup':
                err=pushJoy(cmd.commandID, (WindowRef)userData);
                break;

            default :
                break;
        }
    }

    return err;
}
    
static	IBNibRef	nibRef;

static void makeNibWindow (IBNibRef nibRef) {
    OSStatus	err;
	WindowRef	prefWin;

    err = CreateWindowFromNib(nibRef, CFSTR("PreferenceWindow"), &prefWin);
    if (err == noErr) {
        initPrefWindow(prefWin);
        SetInitialTabState(prefWin);
        EventTypeSpec	tabControlEvents[] ={ { kEventClassControl, kEventControlHit } };
        InstallControlEventHandler( getControlRefByID(kTabMasterSig,kTabMasterID,prefWin),  PrefsTabEventHandlerProc , GetEventTypeCount(tabControlEvents), tabControlEvents, prefWin, NULL );
        EventTypeSpec	list[]={ { kEventClassCommand, kEventCommandProcess } };
        EventHandlerRef	ref;
        EventHandlerUPP winhandler = NewEventHandlerUPP(pfWinproc);
        InstallWindowEventHandler (prefWin, winhandler, 1, list, (void *)prefWin, &ref);
        ShowWindow(prefWin);
        err=RunAppModalLoopForWindow(prefWin);
        RemoveEventHandler (ref);
        DisposeEventHandlerUPP (winhandler);
        HideWindow(prefWin);
        DisposeWindow(prefWin);
    }
}

void ConfigDialogProc( void ) {
    OSStatus	err;

    err = CreateNibReference(CFSTR("main"), &nibRef);
    if (err ==noErr ) {
        makeNibWindow (nibRef);
        DisposeNibReference ( nibRef);
    }
}

static void setupJoyConfig (OSType type, WindowRef win) {
    OSStatus	err;
	WindowRef	joyWin;
    char elementName[256] = "----";

    err = CreateWindowFromNib(nibRef, CFSTR("InputWindow"), &joyWin);
    if (err == noErr) {
        EventTypeSpec	list[]={ { kEventClassCommand, kEventCommandProcess } };
        EventHandlerRef	ref;
        InstallWindowEventHandler (joyWin, NewEventHandlerUPP(joyWinproc), 1, list, (void *)joyWin, &ref);
        ShowSheetWindow(joyWin, win);

//        if (setJoypad(type, elementName)) {
//            setTitle(type, elementName);
//        }

        HideSheetWindow(joyWin);
        DisposeWindow(joyWin);
    }
}

static void setTitle(OSType type, char* elementName, WindowRef win) {
    Str255 str;
    mkstr255(str, elementName);
    SetControlTitle(getControlRefByID(type, 0, win), str);
}

static UInt16 lastPaneSelected = 1;	// static, to keep track of it long term (in a more complex application
                                        // you might store this in a data structure in the window refCon)                                            

static void SetInitialTabState(WindowRef theWindow)
{
    int tabList[] = {kDummyValue, kTabMasterID,kTabMasterID+1,kTabMasterID+2};
    short qq=0;
    for(qq=0;qq<kMaxNumTabs+1;qq++)
    SetControlVisibility( getControlRefByID(  kTabPaneSig,  tabList[qq], theWindow), false, true );  
    SetControlValue(getControlRefByID(kTabMasterSig,kTabMasterID,theWindow),lastPaneSelected );
    SetControlVisibility( getControlRefByID(  kTabPaneSig,  tabList[lastPaneSelected], theWindow), true, true );
}

static pascal OSStatus PrefsTabEventHandlerProc( EventHandlerCallRef inCallRef, EventRef inEvent, void* inUserData )
{
    WindowRef theWindow = (WindowRef)inUserData;  // get the windowRef, passed around as userData    
    short controlValue = 1;
    controlValue = GetControlValue( getControlRefByID(kTabMasterSig,kTabMasterID,theWindow) );
    if ( controlValue != lastPaneSelected )
    {
        int tabList[] = {kDummyValue, kTabMasterID,kTabMasterID+1,kTabMasterID+2,kTabMasterID+3};
        SetControlVisibility( getControlRefByID(  kTabPaneSig,  tabList[lastPaneSelected], theWindow), false, true );
        SetControlVisibility( getControlRefByID(  kTabPaneSig,  tabList[controlValue], theWindow), true, true );    
        Draw1Control( getControlRefByID(kTabMasterSig,kTabMasterID,theWindow) );		
        lastPaneSelected= controlValue;    
    }
    return( eventNotHandledErr );
}
