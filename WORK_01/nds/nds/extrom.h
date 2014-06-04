
struct tagRomImg
{
	UINT8	bios1[0x1000];
	UINT8	bios2[0x8000];
	UINT8	ank[0x0800];
	UINT8	txt[0x1000];
	UINT8	knjx1[0x20000];
	UINT8	knjx1t[0x40000];

	UINT32	uDisk1Size;
	UINT32	uDisk2Size;
	UINT8	sDisk1[512*1024];
	UINT8	sDisk2[512*1024];
};
typedef struct tagRomImg		ROMIMG;


#ifdef __cplusplus
extern "C"
{
#endif

extern const ROMIMG __extromimage;

#ifdef __cplusplus
}
#endif

#define extrom_initialize()

