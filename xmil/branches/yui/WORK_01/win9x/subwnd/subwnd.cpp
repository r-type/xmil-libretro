/**
 * @file	subwnd.cpp
 * @brief	サブ ウィンドウの基底クラスの動作の定義を行います
 */

#include "compiler.h"
#include "resource.h"
#include "subwnd.h"
#include "soundmng.h"
#include "xmil.h"
#include "misc\tstring.h"

//! クラス名
static const TCHAR s_szClassName[] = TEXT("Xmil-SubWnd");

/**
 * 初期化
 * @param[in] hInstance インスタンス
 */
void CSubWndBase::Initialize(HINSTANCE hInstance)
{
	WNDCLASS wc;
	ZeroMemory(&wc, sizeof(wc));
	wc.style = CS_BYTEALIGNCLIENT | CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = ::DefWindowProc;
	wc.hInstance = hInstance;
	wc.hIcon = ::LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
	wc.hCursor = ::LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = static_cast<HBRUSH>(::GetStockObject(NULL_BRUSH));
	wc.lpszClassName = s_szClassName;
	RegisterClass(&wc);
}

/**
 * コンストラクタ
 */
CSubWndBase::CSubWndBase()
{
}

/**
 * デストラクタ
 */
CSubWndBase::~CSubWndBase()
{
}

/**
 * ウィンドウ作成
 */
BOOL CSubWndBase::Create(UINT nCaptionID, DWORD dwStyle, int x, int y, int nWidth, int nHeight, HWND hwndParent, HMENU nIDorHMenu)
{
	std::tstring rCaption(LoadTString(nCaptionID));
	return CreateEx(0, s_szClassName, rCaption.c_str(), dwStyle, x, y, nWidth, nHeight, hwndParent, nIDorHMenu);
}

/**
 * ウィンドウ作成
 */
BOOL CSubWndBase::Create(LPCTSTR lpCaption, DWORD dwStyle, int x, int y, int nWidth, int nHeight, HWND hwndParent, HMENU nIDorHMenu)
{
	return CreateEx(0, s_szClassName, lpCaption, dwStyle, x, y, nWidth, nHeight, hwndParent, nIDorHMenu);
}

/**
 * CWndProc オブジェクトの Windows プロシージャ (WindowProc) が用意されています
 * @param[in] nMsg 処理される Windows メッセージを指定します
 * @param[in] wParam メッセージの処理で使う付加情報を提供します。このパラメータの値はメッセージに依存します
 * @param[in] lParam メッセージの処理で使う付加情報を提供します。このパラメータの値はメッセージに依存します
 * @return メッセージに依存する値を返します
 */
LRESULT CSubWndBase::WindowProc(UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	switch (nMsg)
	{
		case WM_KEYDOWN:
		case WM_KEYUP:
			::SendMessage(hWndMain, nMsg, wParam, lParam);
			break;

		case WM_ENTERMENULOOP:
			soundmng_disable(SNDPROC_SUBWIND);
			break;

		case WM_EXITMENULOOP:
			soundmng_enable(SNDPROC_SUBWIND);
			break;

		case WM_ENTERSIZEMOVE:
			soundmng_disable(SNDPROC_SUBWIND);
			break;

		case WM_EXITSIZEMOVE:
			soundmng_enable(SNDPROC_SUBWIND);
			break;

		case WM_CLOSE:
			DestroyWindow();
			break;

		default:
			return CWndProc::WindowProc(nMsg, wParam, lParam);
	}
	return 0L;
}

/**
 * クライアント サイズの設定
 * @param[in] nWidth 幅
 * @param[in] nHeight 高さ
 */
void CSubWndBase::SetClientSize(int nWidth, int nHeight)
{
	RECT rectDisktop;
	::SystemParametersInfo(SPI_GETWORKAREA, 0, &rectDisktop, 0);
	int scx = ::GetSystemMetrics(SM_CXSCREEN);
	int scy = ::GetSystemMetrics(SM_CYSCREEN);

	UINT cnt = 2;
	do
	{
		RECT rectWindow;
		GetWindowRect(&rectWindow);

		RECT rectClient;
		GetClientRect(&rectClient);
		int w = nWidth + (rectWindow.right - rectWindow.left) - (rectClient.right - rectClient.left);
		int h = nHeight + (rectWindow.bottom - rectWindow.top) - (rectClient.bottom - rectClient.top);

		int x = rectWindow.left;
		int y = rectWindow.top;
		if (scx < w)
		{
			x = (scx - w) / 2;
		}
		else
		{
			if ((x + w) > rectDisktop.right)
			{
				x = rectDisktop.right - w;
			}
			if (x < rectDisktop.left)
			{
				x = rectDisktop.left;
			}
		}
		if (scy < h)
		{
			y = (scy - h) / 2;
		}
		else
		{
			if ((y + h) > rectDisktop.bottom)
			{
				y = rectDisktop.bottom - h;
			}
			if (y < rectDisktop.top)
			{
				y = rectDisktop.top;
			}
		}
		MoveWindow(x, y, w, h, TRUE);
	} while(--cnt);
}
