#include	"compiler.h"
#include	"dialogs.h"
#include	"resource.h"

// ---- file select

void* Callback_resume = NULL;

void* Callback_fileselect = NULL;
void* Callback_filesave = NULL;

FormType *waitform = NULL;

BOOL dlgs_selectfile(char *name) {
	UINT32	param = ByteSwap32(name);

	return ((BOOL)(gCall68KFuncP)(gEmulStateP, (unsigned long)Callback_fileselect, &param, 4));
}

BOOL dlgs_selectwritefile(char *name, int size, const char *def) {
	UINT32	param = ByteSwap32(name);

	(void)size;
	(void)def;
	return((BOOL)(gCall68KFuncP)(gEmulStateP, (unsigned long)Callback_filesave, &param, 4));
}

void ResumeErrorDialogProc(void) {
	UINT32	param;
	char	text[] = "cannot resume.";
	 param = ByteSwap32(text);

	(gCall68KFuncP)(gEmulStateP, (unsigned long)Callback_resume, &param, 4);
}

void ResumeWarningDialogProc(char *name) {
	UINT32	param = ByteSwap32(name);

	(gCall68KFuncP)(gEmulStateP, (unsigned long)Callback_resume, &param, 4);
}

void AboutDialogProc(void) {

    FormType *frmP;

	ARM_MenuEraseStatus(NULL);		// Clear the menu status from the display.
	frmP = ARM_FrmInitForm(IDD_ABOUT);
	ARM_FrmDoDialog(frmP);		// Display the About Box.
	ARM_FrmDeleteForm(frmP);
}

void show_progress(void) {
	waitform = ARM_FrmInitForm(IDD_WAIT);
	ARM_FrmDrawForm(waitform);
}

void erase_progress(void) {
	if (waitform) {
		ARM_FrmEraseForm(waitform);
		ARM_FrmDeleteForm(waitform);
		waitform = NULL;
	}
}

