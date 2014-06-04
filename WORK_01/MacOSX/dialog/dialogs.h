
BOOL dlgs_selectfile(char *name, int size, WindowRef parent);
BOOL dlgs_selectwritefile(char *name, int size, const char *def, OSType type, WindowRef parentWindow);

ControlRef getControlRefByID(OSType sign, int id, WindowRef win);
UInt32 getFieldValue(OSType type, WindowRef win);
void getFieldText(ControlRef cRef, char* buffer);
