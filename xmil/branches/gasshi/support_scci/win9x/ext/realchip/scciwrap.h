#pragma once

class	SoundInterfaceManager;
class	SoundChip;

class	scciwrap{
private:
	static	SoundChip *m_pOpm;
	static	SoundChip *m_pPsg;
	static	HMODULE m_hScci;
	static	SoundInterfaceManager *m_pManager;
public:
	// コンストラクタ
	scciwrap();
	// デストラクタ
	~scciwrap();

	// 初期化
	static	BOOL	initialize();
	// 開放
	static	BOOL	uninitialize();

	// レジスタ設定
	static	BOOL	setOpmReg(BYTE bReg,BYTE bData);
	static	BOOL	setPsgReg(BYTE bReg,BYTE bData);

	// チップ使用有無
	static	BOOL	isOpm();
	static	BOOL	isPsg();

	// リセット
	static	BOOL	reset();

};
