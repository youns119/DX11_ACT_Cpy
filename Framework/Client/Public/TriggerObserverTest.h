#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Observer.h"

BEGIN(Client)
class CTriggerObserverTest final : public CGameObject, public IObserver
{
	using super = CGameObject;

public:
	struct DESC
	{
		CGameObject::DESC GameObjectDesc {};
		vector<_wstring> TriggerNames;
	};

	CTriggerObserverTest(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTriggerObserverTest(const CTriggerObserverTest& Prototype);
	virtual ~CTriggerObserverTest() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;

	virtual void On_Notify() override;

private:
	_bool m_isNotified { false };
	_vec2 m_vTimeAcc {0.f, 3.f};
	//CRenderer::FOG_PARAMS m_FogParams {};
	_vec4 m_FogColor {};

private:

public:
	static CTriggerObserverTest* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;
};
END
