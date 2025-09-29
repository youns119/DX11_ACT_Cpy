#pragma once

#include "ToolAnimation_Defines.h"
#include "Base.h"

BEGIN(Engine)

class CGameInstance;

END

BEGIN(ToolAnimation)

class CLoader final 
	: public Engine::CBase
{
private :
	CLoader(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CLoader() = default;

public :
	HRESULT Init(LEVELID _eNextLevelID);

public :
	_bool GetFinished() const { return m_bFinished; }
	_float Get_CurrnetProgress() const;

public :
	HRESULT Start_Loading();

#ifdef _DEBUG

public :
	void SetUp_WindowText();
	 
#endif

private :
	CGameInstance* m_pGameInstance{};

private :
	ID3D11Device* m_pDevice{};
	ID3D11DeviceContext* m_pContext{};

private :
	HANDLE m_hThread{};
	CRITICAL_SECTION m_CriticalSection{};

private :
	LEVELID	m_eNextLevelID{ LEVELID::LEVEL_END };

	_bool m_bFinished{};
	_tchar	m_szLoadingText[MAX_PATH]{};

	_uint m_iNumTotalLine{};
	_uint m_iCurrentLine{};

private :
	HRESULT Loading_For_Level_Tool();

public :
	static CLoader* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, LEVELID _eNextLevelID);
	virtual void Free() override;
};

END