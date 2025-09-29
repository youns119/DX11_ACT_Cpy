#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

#include "IParried.h"

BEGIN(Engine)

class CModel;
class CShader;
class CCollider;
END

BEGIN(Client)

class CSplitter_Weapon final : public CGameObject, public IParried
{
	using super = CGameObject;

public:
	struct DESC
	{
		CGameObject::GAMEOBJECT_DESC PartObjectDesc{};

		const _float4x4* pSocketMatrix{ nullptr };

		_float fDamage{ 10 };

		CGameObject* pOwner{};
	};

private:
	CSplitter_Weapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSplitter_Weapon(const CSplitter_Weapon& Prototype);
	virtual ~CSplitter_Weapon() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	virtual void On_CollisionEnter(CGameObject* pOther, _uint iGroup) override;
	virtual void On_Collision(CGameObject* pOther, _uint iGroup) override;
	virtual void On_CollisionExit(CGameObject* pOther, _uint iGroup) override;

public:
	CCollider* Get_UpperCollider() { return m_pUpperColliderCom; }
	CCollider* Get_LowerCollider() { return m_pLowerColliderCom; }

private:
	HRESULT Ready_Components();

public:
	virtual void Parried() override;

public:
	virtual CGameObject* Get_Parried() override { return m_pOwner; }

private:	
	CShader*		m_pShaderCom{ nullptr };
	CCollider*		m_pUpperColliderCom { nullptr };
	CCollider*		m_pLowerColliderCom { nullptr };

	// 매달릴 본의 매트릭스 ? 
	const _float4x4* m_pSocketMatrix{ nullptr };

private:
	_float m_fDamage{ 10 };

private:
	CGameObject* m_pOwner{};

public:
	static CSplitter_Weapon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;
};

END
