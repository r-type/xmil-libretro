#include "compiler.h"
#include "libnds.h"
#include "dosio.h"


	bool	__nds_avail;
	HWND	__nds_hWnd;

static const TCHAR s_szClassName[] = _T("Windebug-NDS");
static const TCHAR s_szCaptionName[] = _T("NDS-Window");

static LRESULT CALLBACK __NdsProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
		case WM_PAINT:
			{
				PAINTSTRUCT ps;
				HDC hdc = BeginPaint(hWnd, &ps);
				ndsvideo_update();
				EndPaint(hWnd, &ps);
			}
			break;

		case WM_KEYDOWN:
			ndsinput_keydown(static_cast<UINT16>(LOWORD(wParam)));
			break;

		case WM_KEYUP:
			ndsinput_keyup(static_cast<UINT16>(LOWORD(wParam)));
			break;

		case WM_CLOSE:
			DestroyWindow(hWnd);
			break;

		case WM_DESTROY:
			PostQuitMessage(0);
			break;

		default:
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	return 0;
}


// ----

int _tmain(int argc, TCHAR **argv, char **envp)
{
	HWND hWnd = FindWindow(s_szClassName, NULL);
	if (hWnd != NULL)
	{
		ShowWindow(hWnd, SW_RESTORE);
		SetForegroundWindow(hWnd);
		return 0;
	}

	TCHAR szModuleFile[MAX_PATH];
	GetModuleFileName(NULL, szModuleFile, NELEMENTS(szModuleFile));

	dosio_init();
	file_setcd(szModuleFile);

	HINSTANCE hInstance = reinterpret_cast<HINSTANCE>(GetWindowLong(HWND_DESKTOP, GWL_HINSTANCE));

	WNDCLASS wc;
	ZeroMemory(&wc, sizeof(wc));
	wc.style = CS_BYTEALIGNCLIENT | CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = __NdsProc;
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
	wc.lpszMenuName = NULL;
	wc.lpszClassName = s_szClassName;
	if (!RegisterClass(&wc))
	{
		return 0;
	}
	hWnd = CreateWindow(s_szClassName, s_szCaptionName,
						WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION |
						WS_MINIMIZEBOX,
						CW_USEDEFAULT, CW_USEDEFAULT, 240, 160,
						NULL, NULL, hInstance, NULL);
	__nds_hWnd = hWnd;
	if (hWnd == NULL)
	{
		return -1;
	}
	ShowWindow(hWnd, SW_SHOW);
	UpdateWindow(hWnd);

	ndsinput_initialize();

	__nds_avail = true;

	do
	{
		if (!ndsvideo_initialize(hWnd))
		{
			DestroyWindow(hWnd);
			break;
		}

		nds9main();

	} while(0);

//	ndsvideo_deinitialize();

	dosio_term();
	return 0;
}

bool ndssys_task()
{
	if (__nds_avail)
	{
		CNdsRegUpdate &reg = CNdsRegUpdate::getInstance();
		if (reg.get(RT_SCREEN))
		{
			ndsvideo_bufferupdate();
			reg.reset(RT_SCREEN);
		}

		MSG msg;
		while(PeekMessage(&msg, 0, 0, 0, PM_NOREMOVE))
		{
			if (!GetMessage(&msg, NULL, 0, 0))
			{
				__nds_avail = false;
				break;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return __nds_avail;
}

