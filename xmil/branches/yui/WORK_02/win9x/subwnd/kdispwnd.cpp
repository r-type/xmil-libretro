/**
 * @file	kdispwnd.cpp
 * @brief	�L�[�{�[�h �N���X�̓���̒�`���s���܂�
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

//! �B��̃C���X�^���X�ł�
CKeyDisplayWnd CKeyDisplayWnd::sm_instance;

/**
 * @brief �R���t�B�O
 */
struct KeyDisplayConfig
{
	int		posx;
	int		posy;
};

//! �R���t�B�O
static KeyDisplayConfig s_kdispcfg;

//! �^�C�g��
static const TCHAR s_kdispapp[] = TEXT("Key Display");

/**
 * �ݒ�
 */
static const PFTBL s_kdispini[] =
{
	PFVAL("WindposX", PFTYPE_SINT32,	&s_kdispcfg.posx),
	PFVAL("WindposY", PFTYPE_SINT32,	&s_kdispcfg.posy),
};

//! �p���b�g
static const UINT32 s_kdisppal[KEYDISP_PALS] = {0x00000000, 0xffffffff, 0xf9ff0000};


static UINT8 kdgetpal8(CMNPALFN *self, UINT num);
static UINT32 kdgetpal32(CMNPALFN *self, UINT num);
static UINT16 kdcnvpal16(CMNPALFN *self, RGB32 pal32);

/**
 * ������
 */
void CKeyDisplayWnd::Initialize()
{
	keydisp_initialize();
}

/**
 * ���
 */
void CKeyDisplayWnd::Deinitialize()
{
}

/**
 * �R���X�g���N�^
 */
CKeyDisplayWnd::CKeyDisplayWnd()
{
}

/**
 * �f�X�g���N�^
 */
CKeyDisplayWnd::~CKeyDisplayWnd()
{
}

/**
 * �쐬
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
 * �`�悷��
 * @param[in] cnt �i�񂾃t���[��
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
 * CWndProc �I�u�W�F�N�g�� Windows �v���V�[�W�� (WindowProc) ���p�ӂ���Ă��܂�
 * @param[in] nMsg ��������� Windows ���b�Z�[�W���w�肵�܂�
 * @param[in] wParam ���b�Z�[�W�̏����Ŏg���t������񋟂��܂��B���̃p�����[�^�̒l�̓��b�Z�[�W�Ɉˑ����܂�
 * @param[in] lParam ���b�Z�[�W�̏����Ŏg���t������񋟂��܂��B���̃p�����[�^�̒l�̓��b�Z�[�W�Ɉˑ����܂�
 * @return ���b�Z�[�W�Ɉˑ�����l��Ԃ��܂�
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
 * �`��̎��ɌĂ΂��
 */
void CKeyDisplayWnd::OnPaint()
{
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(&ps);
	OnDraw(TRUE);
	EndPaint(&ps);
}

/**
 * �`��
 * @param[in] redraw �ĕ`��
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
 * ���T�C�Y
 */
void CKeyDisplayWnd::OnResize()
{
	int width;
	int height;
	keydisp_getsize(&width, &height);
	SetClientSize(width, height);
}

/**
 * �ݒ�ǂݍ���
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
 * �ݒ菑������
 */
void kdispwin_writeini()
{
	TCHAR szPath[MAX_PATH];

	initgetfile(szPath, _countof(szPath));
	ini_write(szPath, s_kdispapp, s_kdispini, _countof(s_kdispini));
}
#endif
