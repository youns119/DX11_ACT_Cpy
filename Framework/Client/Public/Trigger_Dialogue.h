#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CCollider;
END

BEGIN(Client)

class IObserver;

class CTrigger_Dialogue final : public CGameObject
{
	using super = CGameObject;

public:
	struct DESC
	{
		CGameObject::DESC GameObjectDesc{};

		_float3 vExtents = {};
	};

private:
	CTrigger_Dialogue(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTrigger_Dialogue(const CTrigger_Dialogue& Prototype);
	virtual ~CTrigger_Dialogue() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	virtual void On_CollisionEnter(CGameObject* pOther, _uint iGroup) override;
	virtual void On_Collision(CGameObject* pOther, _uint iGroup) override;
	virtual void On_CollisionExit(CGameObject* pOther, _uint iGroup) override;

	void Add_CollisionEnter_Callback(std::function<void()> Callback);
	void Add_Collision_Callback(std::function<void()> Callback);
	void Add_CollisionExit_Callback(std::function<void()> Callback);

	void Add_CollisionEnter_Observer(IObserver* pObserver);
	void Add_Collision_Observer(IObserver* pObserver);
	void Add_CollisionExit_Observer(IObserver* pObserver);

private:
	CCollider*	m_pColliderCom = { nullptr };

	vector<std::function<void()>> m_CollisionEnterCallbacks;
	vector<std::function<void()>> m_CollisionCallbacks;
	vector<std::function<void()>> m_CollisionExitCallbacks;

	vector<IObserver*> m_CollisionEnterObservers;
	vector<IObserver*> m_CollisionObservers;
	vector<IObserver*> m_CollisionExitObservers;

private:
	HRESULT Ready_Components(DESC& triggerDesc);

public:
	static CTrigger_Dialogue* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;
};
END
