
Handle GetDlgItem(DialogPtr hWnd, short pos);

BOOL dlgs_selectfile(char *name, int size, WindowRef parent, int opentype);
BOOL dlgs_selectwritefile(char *name, int size, const char *def, OSType type, WindowRef parentWindow);

ControlRef getControlRefByID(OSType sign, int id, WindowRef win);
void getFieldText(ControlRef cRef, char* buffer);

#define	OPEN_FDD	0
#define	OPEN_FONT	1
