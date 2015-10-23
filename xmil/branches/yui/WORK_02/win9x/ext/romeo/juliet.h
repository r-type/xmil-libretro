
#ifdef __cplusplus
extern "C" {
#endif

BRESULT juliet_load(void);
void juliet_unload(void);

BRESULT juliet_prepare(void);

void juliet_YM2151Reset(void);
BRESULT juliet_YM2151IsEnable(void);
BRESULT juliet_YM2151IsBusy(void);
void juliet_YM2151W(UINT8 addr, UINT8 data);
void juliet_YM2151Enable(BRESULT enable);

void juliet_YMF288Reset(void);
BRESULT juliet_YMF288IsEnable(void);
BRESULT juliet_YMF288IsBusy(void);
void juliet_YMF288A(UINT8 addr, UINT8 data);
void juliet_YMF288B(UINT8 addr, UINT8 data);
void juliet_YMF288Enable(BRESULT enable);


void juliet2_reset(void);
void juliet2_sync(DWORD delayclock);
void juliet2_exec(void);
void juliet2_YM2151W(BYTE addr, BYTE data, DWORD clock);
void juliet2_YMF288A(BYTE addr, BYTE data, DWORD clock);
void juliet2_YMF288B(BYTE addr, BYTE data, DWORD clock);

#ifdef __cplusplus
}
#endif
