#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "IParried.h"

BEGIN(Engine)
class CCollider;
END

BEGIN(Client)

class CInkerton_Nutcracker final : public CGameObject, public IParried
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
	CInkerton_Nutcracker(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CInkerton_Nutcracker(const CInkerton_Nutcracker& Prototype);
	virtual ~CInkerton_Nutcracker() = default;

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
	void Parried() override;

public:
	virtual CGameObject* Get_Parried() override { return m_pOwner; }

public:
	void Clear_GrabObject() { m_pGrabObject = nullptr; }
	CGameObject* Get_GrabObject() { return m_pGrabObject; }
	CCollider* Get_Collider() { return m_pColliderCom; }
	CCollider* Get_HandleCollider() { return m_pHandleColliderCom; }

private:
	const _float4x4*	m_pSocketMatrix{ nullptr };
	_float4x4			m_pGrabMatrix{};
	CGameObject*		m_pOwner{ nullptr };
	CCollider*			m_pColliderCom{ nullptr };
	CCollider*			m_pHandleColliderCom{ nullptr };

private:	
	_float				m_fDamage{ 10.f };

private:
	CGameObject*		m_pGrabObject{ nullptr };

public:
	static CInkerton_Nutcracker* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;	
};

END
