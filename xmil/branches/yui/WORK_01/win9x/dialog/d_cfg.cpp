/**
 * @file	d_cfg.cpp
 * @brief	�R���t�B�O �_�C�A���O �N���X�̓���̒�`���s���܂�
 */

#include "compiler.h"
#include "d_cfg.h"
#include "resource.h"
#include <commctrl.h>
#include "soundmng.h"
#include "sysmng.h"
#include "pccore.h"
#include "palettes.h"

//! �N�����v��`
#define LIMITS(v, n, m)		(((v) > (m))? (m) : (((v) < (n)) ? (n) : (v)))

static const UINT32 ratehz[] = {48000, 44100, 33075, 32000, 24000, 22050, 16000, 11025};

/**
 * �R���X�g���N�^
 * @param[in] hwndParent �e�E�B���h�E
 */
CConfigDlg::CConfigDlg(HWND hwndParent)
	: CDlgProc(IDD_CONFIG, hwndParent)
{
}

/**
 * ���̃��\�b�h�� WM_INITDIALOG �̃��b�Z�[�W�ɉ������ČĂяo����܂�
 * @retval TRUE �ŏ��̃R���g���[���ɓ��̓t�H�[�J�X��ݒ�
 * @retval FALSE ���ɐݒ��
 */
BOOL CConfigDlg::OnInitDialog()
{
	for (UINT i = 0; i < _countof(ratehz); i++)
	{
		TCHAR szBuffer[16];
		wsprintf(szBuffer, TEXT("%d"), ratehz[i]);
		SendDlgItemMessage(IDC_SAMPLERATE, CB_ADDSTRING, NULL, reinterpret_cast<LPARAM>(szBuffer));
	}

#if !defined(DISABLE_SOUND)
	SetDlgItemInt(IDC_SAMPLERATE, xmilcfg.samplingrate);
	SetDlgItemInt(IDC_SNDBUFFER, xmilcfg.delayms);
	SetDlgItemInt(IDC_SEEKVOL, xmilcfg.MOTORVOL);
#else	// !defined(DISABLE_SOUND)
	static UINT s_nSoundIDs[] = {IDC_SAMPLERATE, IDC_SNDBUFFER, IDC_SEEKVOL};
	for (size_t i = 0; i < _countof(s_nSoundIDs); i++)
	{
		GetDlgItem(s_nSoundIDs[i]).EnableWindow(FALSE);
	}
#endif	// !defined(DISABLE_SOUND)

	CheckDlgButton(IDC_SKIPLINE, (xmilcfg.skipline) ? BST_CHECKED : BST_UNCHECKED);
	SendDlgItemMessage(IDC_SKIPLIGHT, TBM_SETRANGE, TRUE, MAKELONG(0, 256));
	SendDlgItemMessage(IDC_SKIPLIGHT, TBM_SETPOS, TRUE, xmilcfg.skiplight);
	SetDlgItemInt(IDC_LIGHTSTR, xmilcfg.skiplight);

	return TRUE;
}

/**
 * ���[�U�[�� OK �̃{�^�� (IDOK ID ���̃{�^��) ���N���b�N����ƌĂяo����܂�
 */
void CConfigDlg::OnOK()
{
	UINT nUpdateFlags = 0;

#if !defined(DISABLE_SOUND)
	UINT nSamplingRate = GetDlgItemInt(IDC_SAMPLERATE, NULL, FALSE);
	nSamplingRate = LIMITS(nSamplingRate, 11025, 55500);
	if (xmilcfg.samplingrate != nSamplingRate)
	{
		xmilcfg.samplingrate = nSamplingRate;
		nUpdateFlags |= SYS_UPDATECFG;
		corestat.soundrenewal = TRUE;
	}

	UINT nDelayMs = GetDlgItemInt(IDC_SNDBUFFER, NULL, FALSE);
	nDelayMs = LIMITS(nDelayMs, 100, 1000);
	if (xmilcfg.delayms != nDelayMs)
	{
		xmilcfg.delayms = nDelayMs;
		nUpdateFlags |= SYS_UPDATECFG;
		corestat.soundrenewal = TRUE;
	}

	UINT nMotorVol = GetDlgItemInt(IDC_SEEKVOL, NULL, FALSE);
	nMotorVol = LIMITS(nMotorVol, 0, 100);
	if (xmilcfg.MOTORVOL != nMotorVol)
	{
		xmilcfg.MOTORVOL = nMotorVol;
		soundmng_pcmvolume(SOUND_PCMSEEK, nMotorVol);
		soundmng_pcmvolume(SOUND_PCMSEEK1, nMotorVol);
		nUpdateFlags |= SYS_UPDATEOSCFG;
	}
#endif	// !defined(DISABLE_SOUND)

	bool bRenewal = false;
	const UINT8 bSkipline = (IsDlgButtonChecked(IDC_SKIPLINE) != BST_UNCHECKED) ? 1 : 0;
	if (xmilcfg.skipline != bSkipline)
	{
		xmilcfg.skipline = bSkipline;
		bRenewal = true;
	}
	UINT nSkipLight = SendDlgItemMessage(IDC_SKIPLIGHT, TBM_GETPOS);
	if (nSkipLight > 256)
	{
		nSkipLight = 256;
	}
	if (xmilcfg.skiplight != nSkipLight)
	{
		xmilcfg.skiplight = nSkipLight;
		bRenewal = true;
	}
	if (bRenewal)
	{
		pal_reset();
		nUpdateFlags |= SYS_UPDATECFG;
	}
	::sysmng_update(nUpdateFlags);

	CDlgProc::OnOK();
}

/**
 * CWndProc �I�u�W�F�N�g�� Windows �v���V�[�W�� (WindowProc) ���p�ӂ���Ă��܂�
 * @param[in] nMsg ��������� Windows ���b�Z�[�W���w�肵�܂�
 * @param[in] wParam ���b�Z�[�W�̏����Ŏg���t������񋟂��܂��B���̃p�����[�^�̒l�̓��b�Z�[�W�Ɉˑ����܂�
 * @param[in] lParam ���b�Z�[�W�̏����Ŏg���t������񋟂��܂��B���̃p�����[�^�̒l�̓��b�Z�[�W�Ɉˑ����܂�
 * @return ���b�Z�[�W�Ɉˑ�����l��Ԃ��܂�
 */
LRESULT CConfigDlg::WindowProc(UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	switch (nMsg)
	{
		case WM_HSCROLL:
			if (reinterpret_cast<HWND>(lParam) == GetDlgItem(IDC_SKIPLIGHT))
			{
				SetDlgItemInt(IDC_LIGHTSTR, SendDlgItemMessage(IDC_SKIPLIGHT, TBM_GETPOS));
				return TRUE;
			}
			break;
	}
	return CDlgProc::WindowProc(nMsg, wParam, lParam);
}
