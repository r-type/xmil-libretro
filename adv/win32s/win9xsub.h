
typedef struct {
	UINT8	bios1[0x1000];
	UINT8	bios2[0x8000];
	UINT8	ank[0x0800];
	UINT8	txt[0x1000];
	UINT8	knjx1[0x20000];
	UINT8	knjx1t[0x40000];
} ROMIMG;


#ifdef __cplusplus
extern "C" {
#endif

extern const ROMIMG __extromimage;

void win9xsub_initialize(void);

#ifdef __cplusplus
}
#endif

