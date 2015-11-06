
#include <nds.h>
#ifdef ARM9
#include <nds/registers_alt.h>
#endif

extern unsigned char	__ipc[0x8000];

#define	IPCBASE			(0x027ff000 + sizeof(TransferRegion))

int nds9main();
int nds7main();

#define ndssys_task()				(1)
#define ndsvideo_vramnotify()		do { } while(0)
