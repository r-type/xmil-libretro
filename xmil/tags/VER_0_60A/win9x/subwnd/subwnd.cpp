/**
 * @file	subwnd.cpp
 * @brief	�T�u �E�B���h�E�̊��N���X�̓���̒�`���s���܂�
 */

#include "compiler.h"
#include "resource.h"
#include "subwnd.h"
#include "soundmng.h"
#include "xmil.h"
#include "misc\tstring.h"

//! �N���X��
static const TCHAR s_szClassName[] = TEXT("Xmil-SubWnd");

/**
 * ������
 * @param[in] hInstance �C���X�^���X
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
 * �R���X�g���N�^
 */
CSubWndBase::CSubWndBase()
{
}

/**
 * �f�X�g���N�^
 */
CSubWndBase::~CSubWndBase()
{
}

/**
 * �E�B���h�E�쐬
 */
BOOL CSubWndBase::Create(UINT nCaptionID, DWORD dwStyle, int x, int y, int nWidth, int nHeight, HWND hwndParent, HMENU nIDorHMenu)
{
	std::tstring rCaption(LoadTString(nCaptionID));
	return CreateEx(0, s_szClassName, rCaption.c_str(), dwStyle, x, y, nWidth, nHeight, hwndParent, nIDorHMenu);
}

/**
 * �E�B���h�E�쐬
 */
BOOL CSubWndBase::Create(LPCTSTR lpCaption, DWORD dwStyle, int x, int y, int nWidth, int nHeight, HWND hwndParent, HMENU nIDorHMenu)
{
	return CreateEx(0, s_szClassName, lpCaption, dwStyle, x, y, nWidth, nHeight, hwndParent, nIDorHMenu);
}

/**
 * CWndProc �I�u�W�F�N�g�� Windows �v���V�[�W�� (WindowProc) ���p�ӂ���Ă��܂�
 * @param[in] nMsg ��������� Windows ���b�Z�[�W���w�肵�܂�
 * @param[in] wParam ���b�Z�[�W�̏����Ŏg���t������񋟂��܂��B���̃p�����[�^�̒l�̓��b�Z�[�W�Ɉˑ����܂�
 * @param[in] lParam ���b�Z�[�W�̏����Ŏg���t������񋟂��܂��B���̃p�����[�^�̒l�̓��b�Z�[�W�Ɉˑ����܂�
 * @return ���b�Z�[�W�Ɉˑ�����l��Ԃ��܂�
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
 * �N���C�A���g �T�C�Y�̐ݒ�
 * @param[in] nWidth ��
 * @param[in] nHeight ����
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
