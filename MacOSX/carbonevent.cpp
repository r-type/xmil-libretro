/*
 *  carbonevent.cpp
 *  xmil
 *
 *  Created by tk800 on 04/08/10.
 *
 */

#include "compiler.h"
#include "xmil.h"
#include "carbonevent.h"
#include "mousemng.h"
#include "mackbd.h"
#include "sysmng.h"
#include "taskmng.h"
#include "soundmng.h"
#include "scrndraw.h"
#include "resource.h"
#include "scrnmng.h"

extern bool wrapperMouseDown(EventRef event);

static const EventTypeSpec appEventList[] = {
                {kEventClassCommand, 	kEventCommandProcess },
				{kEventClassAppleEvent,	kEventAppleEvent},
				{kEventClassMouse,		kEventMouseDown},
				{kEventClassMouse,		kEventMouseMoved},
				{kEventClassMouse,		kEventMouseDragged},
				{kEventClassMouse,		kEventMouseUp},
				{kEventClassKeyboard,	kEventRawKeyModifiersChanged},
			};

static const EventTypeSpec windEventList[] = {
				{kEventClassWindow,		kEventWindowClose},
//				{kEventClassWindow,		kEventWindowActivated},
//				{kEventClassWindow,		kEventWindowToolbarSwitchMode},
				{kEventClassWindow,		kEventWindowDragStarted},
				{kEventClassWindow,		kEventWindowDragCompleted},
				{kEventClassWindow,		kEventWindowShown},
				{kEventClassKeyboard,	kEventRawKeyDown},
				{kEventClassKeyboard,	kEventRawKeyUp},
				{kEventClassKeyboard,	kEventRawKeyRepeat},
			};



static pascal OSStatus appevent (EventHandlerCallRef myHandlerChain, EventRef event, void* userData)
{
    OSStatus	result = eventNotHandledErr;
    
    long		eventClass;
    UInt32		whatHappened;
    eventClass = GetEventClass(event);
    whatHappened = GetEventKind(event);

    EventRecord	eve;
    ConvertEventRefToEventRecord( event,&eve );
    if (IsDialogEvent(&eve)) return result;

    UInt32		modif;
    GetEventParameter (event, kEventParamKeyModifiers, typeUInt32, NULL, sizeof(UInt32), NULL, &modif);

    HIPoint		delta;
    EventMouseButton buttonKind;
    GetEventParameter (event, kEventParamMouseButton, typeMouseButton, NULL, sizeof(EventMouseButton), NULL, &buttonKind);
        
    switch (eventClass)
    {
        case kEventClassCommand:
            if (GetEventKind(event)==kEventCommandProcess) {
                HICommand	cmd;
                GetEventParameter(event, kEventParamDirectObject, typeHICommand, NULL, sizeof(HICommand), NULL, &cmd);
                if (cmd.commandID == kHICommandAppHelp) {
                    ICInstance inst;
                    long start, fin;
                    const char	urlStr[] = "http://retropc.net/tk800/xmil/help/help.html";

                    ICStart(&inst, 'xMil');
                    start = 0;
                    fin = strlen(urlStr);
                    ICLaunchURL(inst, "\p", urlStr, strlen(urlStr), &start, &fin);
                    ICStop(inst);
                }
            }
            break;

        case kEventClassAppleEvent:  
            if (whatHappened == kEventAppleEvent) {
				EventRecord	eve;
				ConvertEventRefToEventRecord( event, &eve );
                AEProcessAppleEvent(&eve);
            }
            break;

        case kEventClassMouse: 
            switch (whatHappened)
            {
                case kEventMouseMoved:
                case kEventMouseDragged:
                    GetEventParameter (event, kEventParamMouseDelta, typeHIPoint, NULL, sizeof(HIPoint), NULL, &delta);
                    mousemng_callback(delta);
                    result = noErr;
                    break;
                case kEventMouseDown:
                    if (buttonKind == kEventMouseButtonSecondary || modif & cmdKey) {
                        mousemng_buttonevent(MOUSEMNG_RIGHTDOWN);
                    }
                    else {
                        wrapperMouseDown(event);
                    }
                    result = noErr;
                    break;
                case kEventMouseUp:
                    if (buttonKind == kEventMouseButtonSecondary || modif & cmdKey) {
                        mousemng_buttonevent(MOUSEMNG_RIGHTUP);
                    }
                    else if (buttonKind == kEventMouseButtonTertiary) {
                        mousemng_toggle(MOUSEPROC_SYSTEM);
                        //menu_setmouse(np2oscfg.MOUSE_SW ^ 1);
                        sysmng_update(SYS_UPDATECFG);
                    }
                    else {
                        mousemng_buttonevent(MOUSEMNG_LEFTUP);
                    }
                    result = noErr;
                    break;    
                }
                break;
		case kEventClassKeyboard:
                if (GetEventKind(event)==kEventRawKeyModifiersChanged) {
					mackbd_modifiers(modif);
					result = noErr;
				}
				break;
        default:
            break; 
    }

	(void)myHandlerChain;
	(void)userData;
    return result; 
}

static pascal OSStatus windowevent(EventHandlerCallRef myHandler,  EventRef event, void* userData)
{
    WindowRef	window;
    UInt32		whatHappened;
    OSStatus	result = eventNotHandledErr;    
    long		eventClass;
    
    GetEventParameter(event, kEventParamDirectObject, typeWindowRef, NULL,
                         sizeof(window), NULL, &window);
    eventClass = GetEventClass(event);
    whatHappened = GetEventKind(event);
        
    switch (eventClass)
    {        
        case kEventClassKeyboard:
            UInt32 key;
            GetEventParameter (event, kEventParamKeyCode, typeUInt32, NULL, sizeof(UInt32), NULL, &key);
            UInt32 modif;
            GetEventParameter (event, kEventParamKeyModifiers, typeUInt32, NULL, sizeof(UInt32), NULL, &modif);
            switch (whatHappened)
            {
                case kEventRawKeyUp:
                    mackbd_keyup(key);
                    result = noErr;
                    break;
                case kEventRawKeyRepeat:
                    mackbd_keydown(key, TRUE);
                    result = noErr;
                    break;
                case kEventRawKeyDown:
                    if (modif & cmdKey) {
                        wrapperMouseDown(event);
                    }
                    else {
                        mackbd_keydown(key, TRUE);
                    }
                    result = noErr;
                    break;
                default: 
                    break;             
            }
            break;
            
        case kEventClassWindow:
            switch (whatHappened)
            {
                case kEventWindowClose:
                    taskmng_exit();
                    result = noErr;
                    break;
                case kEventWindowActivated:
                    DisableAllMenuItems(GetMenuHandle(IDM_EDIT));
                    break;
//                case kEventWindowToolbarSwitchMode:
//                    toolwin_open();
//                    break;
                case kEventWindowDragStarted:
                    soundmng_stop();
                    break;
                case kEventWindowDragCompleted:
                    soundmng_play();
                    break;
                case kEventWindowShown:
                    scrndraw_redraw();
                    break;
            }
            break;
        default: 
            break;                
    }

	(void)myHandler;
	(void)userData;
    return result;
}

static void setUpCarbonEvent(void) {

	InstallStandardEventHandler(GetWindowEventTarget(hWndMain));
	InstallApplicationEventHandler(NewEventHandlerUPP(appevent),
								GetEventTypeCount(appEventList),
								appEventList, 0, NULL);
	InstallWindowEventHandler(hWndMain, NewEventHandlerUPP(windowevent),
								GetEventTypeCount(windEventList),
								windEventList, 0, NULL);
}

bool window_initialize(void) {
    OSStatus	err;
    IBNibRef	nibRef;

    err = CreateNibReference(CFSTR("main"), &nibRef);
    if (err ==noErr ) {
        CreateWindowFromNib(nibRef, CFSTR("MainWindow"), &hWndMain);
        DisposeNibReference ( nibRef);
    }
    else {
         return(false);
    }
    
#if 0
    if (np2oscfg.winx != -1 && np2oscfg.winy != -1) {
        MoveWindow(hWndMain, np2oscfg.winx, np2oscfg.winy, false);
    }
#endif

    setUpCarbonEvent();
/*
    if (backupwidth) scrnmng_setwidth(0, backupwidth);
    if (backupheight) scrnmng_setheight(0, backupheight);
	CFStringRef title = CFStringCreateWithCString(NULL, np2oscfg.titles, kCFStringEncodingUTF8);
	if (title) {
		SetWindowTitleWithCFString(hWndMain, title);
		CFRelease(title);
	}
*/
	scrnmng_initialize();
	ShowWindow(hWndMain);
    return(true);
}
