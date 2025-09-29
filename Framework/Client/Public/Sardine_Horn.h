#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

#include "IParried.h"

BEGIN(Engine)
class CCollider;
END

BEGIN(Client)

class CSardine_Horn final : public CGameObject, public IParried
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
	CSardine_Horn(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSardine_Horn(const CSardine_Horn& Prototype);
	virtual ~CSardine_Horn() = default;

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
	virtual void On_CollisionEnter(CGameObject* pOther, _uint iGroup) override;
	virtual void On_Collision(CGameObject* pOther, _uint iGroup) override;
	virtual void On_CollisionExit(CGameObject* pOther, _uint iGroup) override;

public:
	CCollider* Get_Collider() { return m_pColliderCom; }

private:
	CCollider* m_pColliderCom{ nullptr };	
	const _float4x4* m_pSocketMatrix{ nullptr };
	CGameObject* m_pOwner{ nullptr };

private:
	_float m_fDamage{ 10.f };

private:
	HRESULT Ready_Components();	

public:
	static CSardine_Horn* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;		
};

END
