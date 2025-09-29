#pragma once

#include "ToolAnimation_Defines.h"
#include "Base.h"

BEGIN(Engine)

class CGameInstance;

END

BEGIN(ToolAnimation)

class CImgui_Manager;

class CMainApp final
	: public CBase
{
private:
	CMainApp();
	virtual ~CMainApp() = default;

public:
	HRESULT Init();
	void Pre_Update(_float _fTimeDelta);
	void Update(_float _fTimeDelta);
	void Post_Update(_float _fTimeDelta);
	HRESULT Render();

private:
	CGameInstance* m_pGameInstance{};

private:
	ID3D11Device* m_pDevice{};
	ID3D11DeviceContext* m_pContext{};

private:
	HRESULT Start_Level(LEVELID _eStartLevel);

public:
	static CMainApp* Create();
	virtual void Free() override;
};

END