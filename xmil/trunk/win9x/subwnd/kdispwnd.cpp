/**
 * @file	kdispwnd.cpp
 * @brief	キーボード クラスの動作の定義を行います
 */

#include "compiler.h"
#include "resource.h"
#include "kdispwnd.h"
#include "ini.h"
#include "menu.h"
#include "sysmng.h"
#include "xmil.h"
#include "generic/keydisp.h"

#if defined(SUPPORT_KEYDISP)

//! 唯一のインスタンスです
CKeyDisplayWnd CKeyDisplayWnd::sm_instance;

/**
 * @brief コンフィグ
 */
struct KeyDisplayConfig
{
	int		posx;
	int		posy;
};

//! コンフィグ
static KeyDisplayConfig s_kdispcfg;

//! タイトル
static const TCHAR s_kdispapp[] = TEXT("Key Display");

/**
 * 設定
 */
static const PFTBL s_kdispini[] =
{
	PFVAL("WindposX", PFTYPE_SINT32,	&s_kdispcfg.posx),
	PFVAL("WindposY", PFTYPE_SINT32,	&s_kdispcfg.posy),
};

//! パレット
static const UINT32 s_kdisppal[KEYDISP_PALS] = {0x00000000, 0xffffffff, 0xf9ff0000};


static UINT8 kdgetpal8(CMNPALFN *self, UINT num);
static UINT32 kdgetpal32(CMNPALFN *self, UINT num);
static UINT16 kdcnvpal16(CMNPALFN *self, RGB32 pal32);

/**
 * 初期化
 */
void CKeyDisplayWnd::Initialize()
{
	keydisp_initialize();
}

/**
 * 解放
 */
void CKeyDisplayWnd::Deinitialize()
{
}

/**
 * コンストラクタ
 */
CKeyDisplayWnd::CKeyDisplayWnd()
{
}

/**
 * デストラクタ
 */
CKeyDisplayWnd::~CKeyDisplayWnd()
{
}

/**
 * 作成
 */
void CKeyDisplayWnd::Create()
{
	if (m_hWnd != NULL)
	{
		return;
	}

	if (!CSubWndBase::Create(IDS_CAPTION_KEYDISP, WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX, s_kdispcfg.posx, s_kdispcfg.posy, KEYDISP_WIDTH, KEYDISP_HEIGHT, NULL, NULL))
	{
		// np2oscfg.keydisp = 0;
		// sysmng_update(SYS_UPDATEOSCFG);
		return;
	}

	keydisp_setmode(KEYDISP_MODEFM);
	ShowWindow(SW_SHOWNOACTIVATE);
	UpdateWindow();

	if (!m_dd2.Create(m_hWnd, KEYDISP_WIDTH, KEYDISP_HEIGHT))
	{
		DestroyWindow();
		return;
	}

	CMNPALFN palfn;
	palfn.get8 = kdgetpal8;
	palfn.get32 = kdgetpal32;
	palfn.cnv16 = kdcnvpal16;
	palfn.userdata = reinterpret_cast<INTPTR>(&m_dd2);
	keydisp_setpal(&palfn);
	kdispwin_draw(0);
	::SetForegroundWindow(hWndMain);
}

/**
 * 描画する
 * @param[in] cnt 進んだフレーム
 */
void CKeyDisplayWnd::Draw(UINT8 cnt)
{
	if (m_hWnd)
	{
		if (!cnt)
		{
			cnt = 1;
		}
		UINT8 flag = keydisp_process(cnt);
		if (flag & KEYDISP_FLAGSIZING)
		{
			OnResize();
		}
		OnDraw(FALSE);
	}
}

static UINT8 kdgetpal8(CMNPALFN *self, UINT num)
{
	if (num < KEYDISP_PALS)
	{
		return s_kdisppal[num] >> 24;
	}
	return 0;
}

static UINT32 kdgetpal32(CMNPALFN *self, UINT num)
{
	if (num < KEYDISP_PALS)
	{
		return s_kdisppal[num] & 0xffffff;
	}
	return 0;
}

static UINT16 kdcnvpal16(CMNPALFN *self, RGB32 pal32)
{
	return (reinterpret_cast<DD2Surface*>(self->userdata))->GetPalette16(pal32);
}

/**
 * CWndProc オブジェクトの Windows プロシージャ (WindowProc) が用意されています
 * @param[in] nMsg 処理される Windows メッセージを指定します
 * @param[in] wParam メッセージの処理で使う付加情報を提供します。このパラメータの値はメッセージに依存します
 * @param[in] lParam メッセージの処理で使う付加情報を提供します。このパラメータの値はメッセージに依存します
 * @return メッセージに依存する値を返します
 */
LRESULT CKeyDisplayWnd::WindowProc(UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	switch (nMsg)
	{
		case WM_PAINT:
			OnPaint();
			break;

		case WM_MOVE:
			if (!(GetWindowLong(m_hWnd, GWL_STYLE) & (WS_MAXIMIZE | WS_MINIMIZE)))
			{
				RECT rc;
				GetWindowRect(&rc);
				s_kdispcfg.posx = rc.left;
				s_kdispcfg.posy = rc.top;
				sysmng_update(SYS_UPDATEOSCFG);
			}
			break;

		case WM_CLOSE:
			// np2oscfg.keydisp = 0;
			// sysmng_update(SYS_UPDATEOSCFG);
			DestroyWindow();
			break;

		case WM_DESTROY:
			m_dd2.Release();
			keydisp_setmode(KEYDISP_MODENONE);
			break;

		default:
			return CSubWndBase::WindowProc(nMsg, wParam, lParam);
	}
	return 0L;
}

/**
 * 描画の時に呼ばれる
 */
void CKeyDisplayWnd::OnPaint()
{
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(&ps);
	OnDraw(TRUE);
	EndPaint(&ps);
}

/**
 * 描画
 * @param[in] redraw 再描画
 */
void CKeyDisplayWnd::OnDraw(BOOL redraw)
{
	RECT rect;
	GetClientRect(&rect);

	RECT draw;
	draw.left = 0;
	draw.top = 0;
	draw.right = min(KEYDISP_WIDTH, rect.right - rect.left);
	draw.bottom = min(KEYDISP_HEIGHT, rect.bottom - rect.top);
	if ((draw.right <= 0) || (draw.bottom <= 0))
	{
		return;
	}
	CMNVRAM* vram = m_dd2.Lock();
	if (vram)
	{
		keydisp_paint(vram, redraw);
		m_dd2.Unlock();
		m_dd2.Blt(NULL, &draw);
	}
}

/**
 * リサイズ
 */
void CKeyDisplayWnd::OnResize()
{
	int width;
	int height;
	keydisp_getsize(&width, &height);
	SetClientSize(width, height);
}

/**
 * 設定読み込み
 */
void kdispwin_readini()
{
	ZeroMemory(&s_kdispcfg, sizeof(s_kdispcfg));
	s_kdispcfg.posx = CW_USEDEFAULT;
	s_kdispcfg.posy = CW_USEDEFAULT;

	TCHAR szPath[MAX_PATH];
	initgetfile(szPath, _countof(szPath));
	ini_read(szPath, s_kdispapp, s_kdispini, _countof(s_kdispini));
}

/**
 * 設定書き込み
 */
void kdispwin_writeini()
{
	TCHAR szPath[MAX_PATH];

	initgetfile(szPath, _countof(szPath));
	ini_write(szPath, s_kdispapp, s_kdispini, _countof(s_kdispini));
}
#endif
