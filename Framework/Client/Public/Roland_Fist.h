#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

#include "IParried.h"

BEGIN(Engine)
class CCollider;
END

BEGIN(Client)

class CRoland_Fist final : public CGameObject, public IParried
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
	CRoland_Fist(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRoland_Fist(const CRoland_Fist& Prototype);
	virtual ~CRoland_Fist() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual void Parried() override;

public:
	virtual CGameObject* Get_Parried() override { return m_pOwner; }

public:
	CCollider* Get_Collider() { return m_pColliderCom; }
	_bool Is_Hit() { return m_bIsHit; }
	void Set_Hit(_bool _bIsHit) { m_bIsHit = _bIsHit; }

private:
	CCollider*			m_pColliderCom{ nullptr };	
	const _float4x4*	m_pSocketMatrix{ nullptr };
	CGameObject*		m_pOwner{ nullptr };

private:
	_float				m_fDamage{ 20.f };
	_bool				m_bIsHit{ false };

private:
	virtual void On_CollisionEnter(CGameObject* pOther, _uint iGroup) override;
	virtual void On_Collision(CGameObject* pOther, _uint iGroup) override;
	virtual void On_CollisionExit(CGameObject* pOther, _uint iGroup) override;

private:
	HRESULT Ready_Components();	

public:
	static CRoland_Fist* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;	
};

END
