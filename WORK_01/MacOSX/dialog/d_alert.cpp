/*
 *  macalert.cpp
 *  np2
 *
 *  Created by tk800 on Fri Oct 31 2003.
 *
 */

#include "compiler.h"
#include "resource.h"

static SInt16 showCautionAlert(CFStringRef title, CFStringRef string, CFStringRef button) {
    DialogRef	ret;
    AlertStdCFStringAlertParamRec	param;
    DialogItemIndex	hit;
    
    GetStandardAlertDefaultParams(&param, kStdCFStringAlertVersionOne);
    param.movable = true;
    param.cancelButton = kAlertStdAlertCancelButton;
    param.cancelText = param.defaultText;
    if (button) param.defaultText = button;
   
    CreateStandardAlert(kAlertCautionAlert, title, string, &param, &ret);
    SysBeep(0);
    RunStandardAlert(ret, NULL, &hit);
    return(hit);
}

void ResumeErrorDialogProc(void) {
    DialogRef	ret;
    AlertStdCFStringAlertParamRec	param;
    DialogItemIndex	hit;
    
    GetStandardAlertDefaultParams(&param, kStdCFStringAlertVersionOne);   
    param.movable = true;
    CreateStandardAlert(kAlertStopAlert, CFCopyLocalizedString(CFSTR("Couldn't restart"), "Resume Error Message"), 
                                         CFCopyLocalizedString(CFSTR("An error occured when reading the xmil.sav file. X millennium couldn't restart."), "Resume Error Description"),
                                         &param, &ret);
    SysBeep(0);
    RunStandardAlert(ret, NULL, &hit);
}

int ResumeWarningDialogProc(const char *string) {
    SInt16	ret;
    
    ret = showCautionAlert(	CFCopyLocalizedString(CFSTR("The Save Data file is conflicting"), "bad data"), 
                            CFStringCreateWithCString(NULL, string, CFStringGetSystemEncoding()), 
                            CFCopyLocalizedString(CFSTR("Continue"), "OK"));
    if (ret = kAlertStdAlertOKButton) {
        return(IDOK);
    }
    return(IDCANCEL);
}
