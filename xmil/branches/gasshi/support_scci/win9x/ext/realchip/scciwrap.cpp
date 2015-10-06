#include	"scciwrap.h"

SoundChip*	scciwrap::m_pOpm = NULL;
SoundChip*	scciwrap::m_pPsg = NULL;
HINSTANCE	scciwrap::m_hScci = NULL;
SoundInterfaceManager *scciwrap::m_pManager = NULL;

// �R���X�g���N�^
scciwrap::scciwrap(){
}

// �f�X�g���N�^
scciwrap::~scciwrap(){
}

// ������
BOOL	scciwrap::initialize(){
	SCCIFUNC getSoundInterfaceManager = NULL;
	// ���d�Ăяo���`�F�b�N
	if(m_pManager != NULL || m_hScci != NULL){
		return FALSE;
	}
	// load scci
	m_hScci = ::LoadLibrary(TEXT("scci"));
	if(m_hScci == NULL){
		m_hScci = NULL;
		return FALSE;
	}
	// �}�l�[�W���[���擾����
	getSoundInterfaceManager = (SCCIFUNC)(::GetProcAddress(m_hScci, "getSoundInterfaceManager"));
	// �}�l�[�W���[���擾�ł��Ȃ��ꍇ�͖߂�
	if(getSoundInterfaceManager == NULL){
		::FreeLibrary(m_hScci);
		m_hScci = NULL;
		return FALSE;
	}
	// �}�l�[�W���[���擾����
	m_pManager = getSoundInterfaceManager();
	if(m_pManager == NULL){
		::FreeLibrary(m_hScci);
		m_hScci = NULL;
		return FALSE;
	}
	// �������������s��
	m_pManager->initializeInstance();
	// �T�E���h�`�b�v���擾����
	m_pOpm = m_pManager->getSoundChip(SC_TYPE_YM2151,SC_CLOCK_4000000);
	m_pPsg = m_pManager->getSoundChip(SC_TYPE_AY8910,SC_CLOCK_4000000);
	return TRUE;
}

// �J��
BOOL	scciwrap::uninitialize(){
	// ����������Ă��Ȃ��ꍇ�̓G���[
	if(m_pManager == NULL){
		return FALSE;
	}
	// �J�����������{����
	m_pManager->releaseAllSoundChip();
	m_pOpm = NULL;
	m_pPsg = NULL;
	// �C���X�^���X�̊J��
	m_pManager->releaseInstance();
	m_pManager = NULL;

	::FreeLibrary(m_hScci);
	m_hScci = NULL;

	return TRUE;

}

// ���W�X�^�ݒ�
BOOL	scciwrap::setOpmReg(BYTE bReg,BYTE bData){
	if(m_pOpm) m_pOpm->setRegister(bReg,bData);
	return TRUE;
}

BOOL	scciwrap::setPsgReg(BYTE bReg,BYTE bData){
	if(m_pPsg) m_pPsg->setRegister(bReg,bData);
	return TRUE;
}

// �`�b�v�g�p�L��
BOOL	scciwrap::isOpm(){
	return	(m_pOpm != NULL) ? TRUE : FALSE;
}

BOOL	scciwrap::isPsg(){
	return	(m_pPsg != NULL) ? TRUE : FALSE;
}

// ���Z�b�g
BOOL	scciwrap::reset(){
	if(m_pManager) m_pManager->reset();
	return TRUE;
}

