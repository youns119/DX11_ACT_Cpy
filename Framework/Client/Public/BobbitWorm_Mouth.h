#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CCollider;
END

BEGIN(Client)

class CBobbitWorm_Mouth final : public CGameObject
{
	using super = CGameObject;

public:
	struct DESC
	{
		CGameObject::GAMEOBJECT_DESC PartObjectDesc{};

		const _float4x4* pSocketMatrix{ nullptr };
		CGameObject* pOwner{ nullptr };
		_float fDamage{ 10.f };
	};

private:
	CBobbitWorm_Mouth(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBobbitWorm_Mouth(const CBobbitWorm_Mouth& Prototype);
	virtual ~CBobbitWorm_Mouth() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Components();

public:
	virtual void On_CollisionEnter(CGameObject* pOther, _uint iGroup) override;
	virtual void On_Collision(CGameObject* pOther, _uint iGroup) override;
	virtual void On_CollisionExit(CGameObject* pOther, _uint iGroup) override;

public:
	void Clear_GrabObject() { m_pGrabObject = nullptr; }
	CGameObject* Get_GrabObject() { return m_pGrabObject; }
	CCollider* Get_Collider() { return m_pColliderCom; }

private:
	const _float4x4*	m_pSocketMatrix{ nullptr };
	CGameObject*		m_pOwner{ nullptr };
	CCollider*			m_pColliderCom{ nullptr };	

private:
	_float				m_fDamage{ 10.f };

private:
	CGameObject*		m_pGrabObject{ nullptr };

public:
	static CBobbitWorm_Mouth* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;
};

END
