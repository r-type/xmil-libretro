
void AboutDialogProc(void);
void ConfigDialogProc(void);
void SoundDialogProc(void);
void OptionDialogProc(void);
void ScrnOptionDialogProc(void);
void DipswDialogProc(void);

void ResumeErrorDialogProc(void);
int ResumeWarningDialogProc(const char *string);

void dialog_newdisk(void);
void dialog_changefdd(REG8 drv);
void dialog_writebmp(void);
void dialog_x1f(void);

void show_progress(void);
void erase_progress(void);
