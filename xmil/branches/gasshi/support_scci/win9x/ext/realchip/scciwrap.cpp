#include	"scciwrap.h"

SoundChip*	scciwrap::m_pOpm = NULL;
SoundChip*	scciwrap::m_pPsg = NULL;
HINSTANCE	scciwrap::m_hScci = NULL;
SoundInterfaceManager *scciwrap::m_pManager = NULL;

// コンストラクタ
scciwrap::scciwrap(){
}

// デストラクタ
scciwrap::~scciwrap(){
}

// 初期化
BOOL	scciwrap::initialize(){
	SCCIFUNC getSoundInterfaceManager = NULL;
	// 多重呼び出しチェック
	if(m_pManager != NULL || m_hScci != NULL){
		return FALSE;
	}
	// load scci
	m_hScci = ::LoadLibrary(TEXT("scci"));
	if(m_hScci == NULL){
		m_hScci = NULL;
		return FALSE;
	}
	// マネージャーを取得する
	getSoundInterfaceManager = (SCCIFUNC)(::GetProcAddress(m_hScci, "getSoundInterfaceManager"));
	// マネージャーが取得できない場合は戻る
	if(getSoundInterfaceManager == NULL){
		::FreeLibrary(m_hScci);
		m_hScci = NULL;
		return FALSE;
	}
	// マネージャーを取得する
	m_pManager = getSoundInterfaceManager();
	if(m_pManager == NULL){
		::FreeLibrary(m_hScci);
		m_hScci = NULL;
		return FALSE;
	}
	// 初期化処理を行う
	m_pManager->initializeInstance();
	// サウンドチップを取得する
	m_pOpm = m_pManager->getSoundChip(SC_TYPE_YM2151,SC_CLOCK_4000000);
	m_pPsg = m_pManager->getSoundChip(SC_TYPE_AY8910,SC_CLOCK_4000000);
	return TRUE;
}

// 開放
BOOL	scciwrap::uninitialize(){
	// 初期化されていない場合はエラー
	if(m_pManager == NULL){
		return FALSE;
	}
	// 開放処理を実施する
	m_pManager->releaseAllSoundChip();
	m_pOpm = NULL;
	m_pPsg = NULL;
	// インスタンスの開放
	m_pManager->releaseInstance();
	m_pManager = NULL;

	::FreeLibrary(m_hScci);
	m_hScci = NULL;

	return TRUE;

}

// レジスタ設定
BOOL	scciwrap::setOpmReg(BYTE bReg,BYTE bData){
	if(m_pOpm) m_pOpm->setRegister(bReg,bData);
	return TRUE;
}

BOOL	scciwrap::setPsgReg(BYTE bReg,BYTE bData){
	if(m_pPsg) m_pPsg->setRegister(bReg,bData);
	return TRUE;
}

// チップ使用有無
BOOL	scciwrap::isOpm(){
	return	(m_pOpm != NULL) ? TRUE : FALSE;
}

BOOL	scciwrap::isPsg(){
	return	(m_pPsg != NULL) ? TRUE : FALSE;
}

// リセット
BOOL	scciwrap::reset(){
	if(m_pManager) m_pManager->reset();
	return TRUE;
}

