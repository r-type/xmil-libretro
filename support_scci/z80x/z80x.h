
#ifdef __cplusplus
extern "C" {
#endif

extern UINT8 ZSPtable[256];
extern UINT8 z80inc_flag[256];
extern UINT8 z80dec_flag[256];
extern UINT8 z80szc_flag[512];

extern const UINT8 cycles_main[256];
extern const UINT8 cycles_xx[256];
extern const UINT8 cycles_ed[256];


#define Z80RPUB	void __fastcall
#define	Z80XASM __declspec(naked) static void __fastcall
typedef void (__fastcall * Z80XOPTBL)(void);


// ---- for test

extern const Z80XOPTBL opcode_main2[256];
extern const Z80XOPTBL opcode_dd2[256];
extern const Z80XOPTBL opcode_ed2[256];
extern const Z80XOPTBL opcode_fd2[256];

void __fastcall _code_cb2(void);
void __fastcall	_ixcb_proc2(void);
void __fastcall	_iycb_proc2(void);

#ifdef __cplusplus
}
#endif

