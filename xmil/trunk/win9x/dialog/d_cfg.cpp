/**
 * @file	d_cfg.cpp
 * @brief	�R���t�B�O �_�C�A���O �N���X�̓���̒�`���s���܂�
 */

#include "compiler.h"
#include "d_cfg.h"
#include "resource.h"
#include <commctrl.h>
#include "xmil.h"
#include "soundmng.h"
#include "sysmng.h"
#include "pccore.h"

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

	CheckDlgButton(IDC_ALLOWRESIZE, (xmiloscfg.thickframe) ? BST_CHECKED : BST_UNCHECKED);

#if defined(SUPPORT_RESUME)
	CheckDlgButton(IDC_RESUME, (xmiloscfg.resume) ? BST_CHECKED : BST_UNCHECKED);
#else	// defined(SUPPORT_RESUME)
	GetDlgItem(IDC_RESUME).EnableWindow(FALSE);
#endif	// defined(SUPPORT_RESUME)

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

	const UINT8 bAllowResize = (IsDlgButtonChecked(IDC_ALLOWRESIZE) != BST_UNCHECKED) ? 1 : 0;
	if (xmiloscfg.thickframe != bAllowResize)
	{
		xmiloscfg.thickframe = bAllowResize;
		nUpdateFlags |= SYS_UPDATEOSCFG;
	}

#if defined(SUPPORT_RESUME)
	const UINT8 bResume = (IsDlgButtonChecked(IDC_RESUME) != BST_UNCHECKED) ? 1 : 0;
	if (xmiloscfg.resume != bResume)
	{
		xmiloscfg.resume = bResume;
		nUpdateFlags |= SYS_UPDATEOSCFG;
	}
#endif	// defined(SUPPORT_RESUME)

	::sysmng_update(nUpdateFlags);

	CDlgProc::OnOK();
}
