
#ifdef __cplusplus
extern "C" {
#endif

LRESULT CALLBACK advuser_proc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);

BRESULT advuser_initialize(void);
void advuser_deinitialize(void);

#ifdef __cplusplus
}
#endif

