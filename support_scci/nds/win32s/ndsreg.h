
enum tagRegType
{
	RT_SCREEN		= 0,
	RT_VRAM			= 0,
	RT_PALETTE		= 0,

	RT_MAX			= 1
};


// ----

class CNdsRegUpdate
{
public:
	static CNdsRegUpdate& getInstance() { return m_self; }
	CNdsRegUpdate()
	{
		ZeroMemory(m_bUpdate, sizeof(m_bUpdate));
	}
	void set(tagRegType nType) { m_bUpdate[nType] = true; }
	void reset(tagRegType nType) { m_bUpdate[nType] = false; }
	bool get(tagRegType nType) const { return m_bUpdate[nType]; }

private:
	static CNdsRegUpdate m_self;
	bool m_bUpdate[RT_MAX];
};

class CNdsReg8Base
{
public:
	operator UINT8() const { return m_cValue; }
	void set(UINT8 cValue, tagRegType nType);

private:
	UINT8 m_cValue;
};

class CNdsReg16Base
{
public:
	operator UINT16() const { return m_wValue; }
	void set(UINT16 wValue, tagRegType nType);

private:
	UINT16 m_wValue;
};

class CNdsReg32Base
{
public:
	operator UINT32() const { return m_dwValue; }
	void set(UINT32 dwValue, tagRegType nType);

private:
	UINT32 m_dwValue;
};


// ----

template<tagRegType T>
class CNdsReg8 : public CNdsReg8Base
{
public:
	const CNdsReg8<T>& operator=(UINT8 cValue)
	{
		set(wValue, T);
		return *this:
	}
};

template<tagRegType T>
class CNdsReg16 : public CNdsReg16Base
{
public:
	const CNdsReg16<T>& operator=(UINT16 wValue)
	{
		set(wValue, T);
		return *this;
	}
};

template<tagRegType T>
class CNdsReg32 : public CNdsReg32Base
{
public:
	const CNdsReg32<T>& operator=(UINT32 dwValue)
	{
		set(dwValue, T);
		return *this;
	}
};

#define CRegScreen16	CNdsReg16<RT_SCREEN>
#define CRegScreen32	CNdsReg32<RT_SCREEN>
#define CRegVram16		CNdsReg16<RT_VRAM>
#define CRegPalette16	CNdsReg16<RT_PALETTE>

