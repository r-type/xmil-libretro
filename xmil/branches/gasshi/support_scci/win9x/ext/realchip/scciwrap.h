#pragma once
#include "scci.h"
#include "SCCIDefines.h"

class	scciwrap{
private:
	static	SoundChip *m_pOpm;
	static	SoundChip *m_pPsg;
	static	HINSTANCE	m_hScci;
	static	SoundInterfaceManager *m_pManager;
public:
	// �R���X�g���N�^
	scciwrap();
	// �f�X�g���N�^
	~scciwrap();

	// ������
	static	BOOL	initialize();
	// �J��
	static	BOOL	uninitialize();

	// ���W�X�^�ݒ�
	static	BOOL	setOpmReg(BYTE bReg,BYTE bData);
	static	BOOL	setPsgReg(BYTE bReg,BYTE bData);

	// �`�b�v�g�p�L��
	static	BOOL	isOpm();
	static	BOOL	isPsg();

	// ���Z�b�g
	static	BOOL	reset();

};
