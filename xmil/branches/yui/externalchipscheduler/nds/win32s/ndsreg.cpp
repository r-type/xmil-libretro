#include "compiler.h"
#include "libnds.h"


CNdsRegUpdate CNdsRegUpdate::m_self;


static bool s_bUpdate[RT_MAX];

void CNdsReg8Base::set(UINT8 cValue, tagRegType nType)
{
	m_cValue = cValue;
	CNdsRegUpdate::getInstance().set(nType);
}

void CNdsReg16Base::set(UINT16 wValue, tagRegType nType)
{
	m_wValue = wValue;
	CNdsRegUpdate::getInstance().set(nType);
}

void CNdsReg32Base::set(UINT32 dwValue, tagRegType nType)
{
	m_dwValue = dwValue;
	CNdsRegUpdate::getInstance().set(nType);
}
